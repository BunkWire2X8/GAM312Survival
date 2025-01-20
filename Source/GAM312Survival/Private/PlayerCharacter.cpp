#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "BerryBush.h"
#include "MineableResource.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and attach camera
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh(), "head");
    FirstPersonCamera->bUsePawnControlRotation = true;

    // Ensure rotations are in first person style
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize stats
    CurrentHealth = InitializeStat(CurrentHealth, MaxHealth);
    CurrentHunger = InitializeStat(CurrentHunger, MaxHunger);
    CurrentStamina = InitializeStat(CurrentStamina, MaxStamina);

    // Start periodic hunger decrease
    GetWorld()->GetTimerManager().SetTimer(
        HungerTimerHandle,
        this,
        &APlayerCharacter::UpdateHunger,
        HungerUpdateInterval,
        true
    );

    // Start stamina restoration timer
    GetWorld()->GetTimerManager().SetTimer(
        StaminaRestoreTimerHandle,
        this,
        &APlayerCharacter::UpdateStamina,
        StaminaUpdateInterval,
        true
    );
}

// Initializes stat to its maximum value if not set or if somehow gone past maximum value
float APlayerCharacter::InitializeStat(float CurrentValue, float MaxValue)
{
    return (CurrentValue == NULL || CurrentValue > MaxValue) ? MaxValue : CurrentValue;
}

void APlayerCharacter::UpdateHunger()
{
    // Decrease hunger stat
    SetHunger(CurrentHunger - HungerDecreaseRate);

    // Apply starvation damage if hunger is zero
    if (CurrentHunger <= 0.0f)
    {
        SetHealth(CurrentHealth - StarvationDamageRate);
    }
}

// If stamina is draining, decrease it; otherwise restore it
void APlayerCharacter::UpdateStamina()
{
    float staminaChange = bIsStaminaDraining
        ? -(StaminaDecreaseRate * StaminaUpdateInterval)
        : (StaminaRestoreRate * StaminaUpdateInterval);

    SetStamina(CurrentStamina + staminaChange);
}

void APlayerCharacter::ToggleStaminaDrain()
{
    bIsStaminaDraining = !bIsStaminaDraining;
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // Clears stat timers
    GetWorld()->GetTimerManager().ClearTimer(HungerTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(StaminaRestoreTimerHandle);
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Debug stats display
    if (bShowDebugStats)
    {
        FString StatsText = FString::Printf(TEXT(
            "Health: %.1f\nHunger: %.1f\nStamina: %.1f\n"
            "Inventory:\n"
            "- Wood: %d\n"
            "- Stone: %d\n"
            "- Berries: %d"
        ),
            CurrentHealth, CurrentHunger, CurrentStamina,
            CurrentWood, CurrentStone, CurrentBerries);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, StatsText);
        }
    }
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind movement functions
    PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerCharacter::MoveProgressive);
    PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerCharacter::MoveStrafe);

    // Bind look functions
    PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APlayerCharacter::LookHorizontal);
    PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APlayerCharacter::LookHorizontal);
    PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APlayerCharacter::LookVertical);
    PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APlayerCharacter::LookVertical);

    // Bind jump functions
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    // Bind interaction
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::CheckInteraction);
}

void APlayerCharacter::CheckInteraction()
{
    FVector Start = FirstPersonCamera->GetComponentLocation();
    FVector Forward = FirstPersonCamera->GetForwardVector();
    FVector End = Start + (Forward * InteractionRange);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

    if (bHit)
    {
        // Check if we hit a berry bush
        ABerryBush* BerryBush = Cast<ABerryBush>(HitResult.GetActor());
        if (BerryBush && !BerryBush->bIsCollected)
        {
            BerryBush->CollectBerry();
            SetBerries(GetBerries() + 1);
            return;
        }

        // Check if we hit a mineable resource
        AMineableResource* Resource = Cast<AMineableResource>(HitResult.GetActor());
        if (Resource && !Resource->IsDepleted())
        {
            int32 AmountMined = Resource->MineChunk();

            // Add to inventory based on resource type
            switch (Resource->ResourceType)
            {
            case EResourceType::Wood:
                SetWood(GetWood() + AmountMined);
                break;
            case EResourceType::Stone:
                SetStone(GetStone() + AmountMined);
                break;
            case EResourceType::Berry:
                SetBerries(GetBerries() + AmountMined);
                break;
            default:
                break;
            }
            return;
        }
    }
}

// Movement Functions

// Move forward or backward
void APlayerCharacter::MoveProgressive(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

// Move left or right
void APlayerCharacter::MoveStrafe(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

// Look Functions

// Rotate up or down
void APlayerCharacter::LookVertical(float Value)
{
    AddControllerPitchInput(Value);
}

// Rotate left or right
void APlayerCharacter::LookHorizontal(float Value)
{
    AddControllerYawInput(Value);
}

// Getters

// Returns current health
float APlayerCharacter::GetHealth() const { return CurrentHealth; }

// Returns current hunger
float APlayerCharacter::GetHunger() const { return CurrentHunger; }

// Returns current stamina
float APlayerCharacter::GetStamina() const { return CurrentStamina; }

// Returns current wood count
int APlayerCharacter::GetWood() const { return CurrentWood; }

// Returns current stone count
int APlayerCharacter::GetStone() const { return CurrentStone; }

// Returns current berry count
int APlayerCharacter::GetBerries() const { return CurrentBerries; }

// Setters

// Sets current health
void APlayerCharacter::SetHealth(float NewHealth)
{
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}

// Sets current hunger
void APlayerCharacter::SetHunger(float NewHunger)
{
    CurrentHunger = FMath::Clamp(NewHunger, 0.0f, MaxHunger);
}

// Sets current stamina
void APlayerCharacter::SetStamina(float NewStamina)
{
    CurrentStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
}

// Sets current wood count
void APlayerCharacter::SetWood(int NewWood)
{
    CurrentWood = FMath::Clamp(NewWood, 0, MaxItemSlot);
}

// Sets current stone count
void APlayerCharacter::SetStone(int NewStone)
{
    CurrentStone = FMath::Clamp(NewStone, 0, MaxItemSlot);
}

// Sets current berry count
void APlayerCharacter::SetBerries(int NewBerries)
{
    CurrentBerries = FMath::Clamp(NewBerries, 0, MaxItemSlot);
}

// Debug
void APlayerCharacter::ToggleDebugStats()
{
    bShowDebugStats = !bShowDebugStats;
}
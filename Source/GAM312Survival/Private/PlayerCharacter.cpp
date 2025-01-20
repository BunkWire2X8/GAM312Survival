#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "BerryBush.h"
#include "MineableResource.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and attach the first-person camera component
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh(), "head");
    FirstPersonCamera->bUsePawnControlRotation = true;

    // Configure character rotation settings in first person style
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize player stats
    CurrentHealth = InitializeStat(CurrentHealth, MaxHealth);
    CurrentHunger = InitializeStat(CurrentHunger, MaxHunger);
    CurrentStamina = InitializeStat(CurrentStamina, MaxStamina);

    // Initialize periodic stat updates
    GetWorld()->GetTimerManager().SetTimer(
        HungerTimerHandle,
        this,
        &APlayerCharacter::UpdateHunger,
        HungerUpdateInterval,
        true
    );
    GetWorld()->GetTimerManager().SetTimer(
        StaminaRestoreTimerHandle,
        this,
        &APlayerCharacter::UpdateStamina,
        StaminaUpdateInterval,
        true
    );
}

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

void APlayerCharacter::UpdateStamina()
{
    float staminaChange = bIsStaminaDraining
        ? -(StaminaDecreaseRate * StaminaUpdateInterval)
        : (StaminaRestoreRate * StaminaUpdateInterval);

    SetStamina(CurrentStamina + staminaChange);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // Cleanup timers
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

    // Movement bindings
    PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerCharacter::MoveProgressive);
    PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerCharacter::MoveStrafe);

    // Look bindings
    PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APlayerCharacter::LookHorizontal);
    PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APlayerCharacter::LookHorizontal);
    PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APlayerCharacter::LookVertical);
    PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APlayerCharacter::LookVertical);

    // Action bindings
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::CheckInteraction);
}

void APlayerCharacter::CheckInteraction()
{
    // Setup interaction trace
    FVector Start = FirstPersonCamera->GetComponentLocation();
    FVector Forward = FirstPersonCamera->GetForwardVector();
    FVector End = Start + (Forward * InteractionRange);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

    if (bHit)
    {
        // Handle berry bush interaction
        if (ABerryBush* BerryBush = Cast<ABerryBush>(HitResult.GetActor()))
        {
            if (!BerryBush->bIsCollected)
            {
                BerryBush->CollectBerry();
                SetBerries(GetBerries() + 1);
                return;
            }
        }

        // Handle mineable resource interaction
        if (AMineableResource* Resource = Cast<AMineableResource>(HitResult.GetActor()))
        {
            if (!Resource->IsDepleted())
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
                }
            }
        }
    }
}

// Movement Implementation

void APlayerCharacter::MoveProgressive(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void APlayerCharacter::MoveStrafe(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

// Look Implementation

void APlayerCharacter::LookVertical(float Value)
{
    AddControllerPitchInput(Value);
}

void APlayerCharacter::LookHorizontal(float Value)
{
    AddControllerYawInput(Value);
}

void APlayerCharacter::ToggleStaminaDrain()
{
    bIsStaminaDraining = !bIsStaminaDraining;
}

// Stat Getters

float APlayerCharacter::GetHealth() const { return CurrentHealth; }
float APlayerCharacter::GetHunger() const { return CurrentHunger; }
float APlayerCharacter::GetStamina() const { return CurrentStamina; }

// Inventory Getters

int APlayerCharacter::GetWood() const { return CurrentWood; }
int APlayerCharacter::GetStone() const { return CurrentStone; }
int APlayerCharacter::GetBerries() const { return CurrentBerries; }

// Stat Setters

void APlayerCharacter::SetHealth(float NewHealth)
{
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}

void APlayerCharacter::SetHunger(float NewHunger)
{
    CurrentHunger = FMath::Clamp(NewHunger, 0.0f, MaxHunger);
}

void APlayerCharacter::SetStamina(float NewStamina)
{
    CurrentStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
}

// Inventory Setters

void APlayerCharacter::SetWood(int NewWood)
{
    CurrentWood = FMath::Clamp(NewWood, 0, MaxItemSlot);
}

void APlayerCharacter::SetStone(int NewStone)
{
    CurrentStone = FMath::Clamp(NewStone, 0, MaxItemSlot);
}

void APlayerCharacter::SetBerries(int NewBerries)
{
    CurrentBerries = FMath::Clamp(NewBerries, 0, MaxItemSlot);
}

void APlayerCharacter::ToggleDebugStats()
{
    bShowDebugStats = !bShowDebugStats;
}
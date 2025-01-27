#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "BerryBush.h"
#include "MineableResource.h"
#include "GameFramework/PlayerController.h"

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

    // Initialize menu variables
    bIsMenuOpen = false;
    MenuWidgetInstance = nullptr;
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

    if (MenuWidgetInstance)
    {
        MenuWidgetInstance->RemoveFromParent();
        MenuWidgetInstance = nullptr;
    }
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Making sure preview model renders
    if (bIsBuildingMode && PreviewBuildable)
    {
        UpdatePreview();
    }

    // Debug stats display
    if (bShowDebugStats && GEngine)
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

        if (bIsBuildingMode) {
            StatsText += "\nCurrently Building";
        }

        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, StatsText);
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
    PlayerInputComponent->BindAction("Open Menu", IE_Pressed, this, &APlayerCharacter::ToggleMenu);

    // Build bindings
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::PlaceBuildable);
    PlayerInputComponent->BindAction("Open Menu", IE_Pressed, this, &APlayerCharacter::CancelBuilding);
    PlayerInputComponent->BindAction("Rotate Buildable Left", IE_Pressed, this, &APlayerCharacter::RotatePreviewLeft);
    PlayerInputComponent->BindAction("Rotate Buildable Right", IE_Pressed, this, &APlayerCharacter::RotatePreviewRight);
}


void APlayerCharacter::RotatePreviewLeft() { RotatePreviewYaw(-15.0f); }

void APlayerCharacter::RotatePreviewRight(){ RotatePreviewYaw(15.0f); }

void APlayerCharacter::RotatePreviewYaw(float Value)
{
    if (bIsBuildingMode && PreviewBuildable)
    {
        FRotator NewRotation = PreviewBuildable->GetActorRotation();
        NewRotation.Yaw += Value;
        PreviewBuildable->SetActorRotation(NewRotation);
    }
}

void APlayerCharacter::StartBuilding(TSubclassOf<ABuildableBase> BuildableToPlace)
{
    if (BuildableToPlace && !bIsBuildingMode)
    {
        CancelBuilding();
        bIsBuildingMode = true;
        bIsMenuOpen = false;

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        PreviewBuildable = GetWorld()->SpawnActor<ABuildableBase>(BuildableToPlace, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

        if (PreviewBuildable)
        {
            PreviewBuildable->SetActorEnableCollision(false);
            PreviewBuildable->BuildableMesh->SetMaterial(0, PreviewMaterial);
        }
    }
}

void APlayerCharacter::UpdatePreview()
{
    if (!PreviewBuildable) return;

    FVector Start = FirstPersonCamera->GetComponentLocation();
    FVector End = Start + FirstPersonCamera->GetForwardVector() * InteractionRange * 2;

    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
    {
        PreviewBuildable->SetActorLocation(Hit.Location + Hit.Normal * 10.0f);
    }
}

void APlayerCharacter::PlaceBuildable()
{
    if (!PreviewBuildable || !bIsBuildingMode) return;

    bool bCanAfford = false;
    int32 AvailableResource = 0;

    switch (PreviewBuildable->MaterialType)
    {
    case EMaterialType::Wooden:
        AvailableResource = CurrentWood;
        break;
    case EMaterialType::Stone:
        AvailableResource = CurrentStone;
        break;
    }

    if (PreviewBuildable->CanAfford(AvailableResource))
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

        if (ABuildableBase* NewBuildable = GetWorld()->SpawnActor<ABuildableBase>(PreviewBuildable->GetClass(), PreviewBuildable->GetActorTransform(), SpawnParams))
        {
            switch (PreviewBuildable->MaterialType)
            {
            case EMaterialType::Wooden:
                SetWood(CurrentWood - PreviewBuildable->ConstructionCost);
                break;
            case EMaterialType::Stone:
                SetStone(CurrentStone - PreviewBuildable->ConstructionCost);
                break;
            }
        }
    }
}

void APlayerCharacter::CancelBuilding()
{
    if (PreviewBuildable)
    {
        PreviewBuildable->Destroy();
        PreviewBuildable = nullptr;
    }
    bIsBuildingMode = false;
}

void APlayerCharacter::ToggleMenu()
{
    if (!MenuWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("MenuWidgetClass is not set!"));
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not retrieve PlayerController!"));
        return;
    }

    bIsMenuOpen = !bIsMenuOpen;

    if (bIsMenuOpen)
    {
        // Create placeholder menu widget if not already created
        if (!MenuWidgetInstance)
        {
            MenuWidgetInstance = CreateWidget<UUserWidget>(PlayerController, MenuWidgetClass);
            if (!MenuWidgetInstance)
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to create menu widget!"));
                return;
            }
        }

        // Add to viewport
        MenuWidgetInstance->AddToViewport();

        // Show cursor
        PlayerController->bShowMouseCursor = true;
    }
    else
    {
        // Remove from viewport
        if (MenuWidgetInstance)
        {
            MenuWidgetInstance->RemoveFromParent();
        }

        // Hide cursor
        PlayerController->bShowMouseCursor = false;
    }
}

void APlayerCharacter::CheckInteraction()
{
    if (bIsMenuOpen || bIsBuildingMode) return;

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
                // Deny collection if stamina is low
                if (10 > GetStamina()) return;

                BerryBush->CollectBerry();

                // Drain stamina
                SetStamina(GetStamina() - 10.0);

                SetBerries(GetBerries() + 1);
                return;
            }
        }

        // Handle mineable resource interaction
        if (AMineableResource* Resource = Cast<AMineableResource>(HitResult.GetActor()))
        {
            if (!Resource->IsDepleted())
            {
                // Deny collection if stamina is low
                if (Resource->GetCurrentChunkAmount() * 10 > GetStamina()) return;

                int32 AmountMined = Resource->MineChunk();

                // Drain stamina
                SetStamina(GetStamina() - float(AmountMined * 10));

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
                return;
            }
        }
    }
}

// Movement Implementation

void APlayerCharacter::MoveProgressive(float Value)
{
    if (Value != 0.0f && !bIsMenuOpen)
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void APlayerCharacter::MoveStrafe(float Value)
{
    if (Value != 0.0f && !bIsMenuOpen)
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

// Look Implementation

void APlayerCharacter::LookVertical(float Value)
{
    if (Value != 0.0f && !bIsMenuOpen) {
        AddControllerPitchInput(Value);
    }
}

void APlayerCharacter::LookHorizontal(float Value)
{
    if (Value != 0.0f && !bIsMenuOpen) {
        AddControllerYawInput(Value);
    }
}

// Jump Implementation

void APlayerCharacter::Jump()
{
    if (!bIsMenuOpen) {
        Super::Jump();
    }
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
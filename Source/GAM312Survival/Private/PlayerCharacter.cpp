#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "BerryBush.h"
#include "MineableResource.h"
#include "GameFramework/PlayerController.h"

APlayerCharacter::APlayerCharacter()
{
    // Configure base character settings
    PrimaryActorTick.bCanEverTick = true;

    // First-person camera setup
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh(), "head"); // Attach to skeleton mesh head socket
    FirstPersonCamera->bUsePawnControlRotation = true; // Camera follows controller rotation

    // Configure character movement behavior
    GetCharacterMovement()->bOrientRotationToMovement = false; // Don't rotate toward movement direction
    bUseControllerRotationPitch = false; // Prevent pitch rotation from controller
    bUseControllerRotationYaw = true;    // Allow yaw rotation from controller
    bUseControllerRotationRoll = false;  // Prevent roll rotation from controller

    // Initialize UI state
    bIsMenuOpen = false;
    MenuWidgetInstance = nullptr;
}

void APlayerCharacter::ShowEndGameWidget(bool bWon)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC || bHasEnded) return;

    bHasEnded = true; // Prevent multiple triggers

    // Cleanup existing UI
    if (MenuWidgetInstance) MenuWidgetInstance->RemoveFromParent();
    if (StatsWidgetInstance) StatsWidgetInstance->RemoveFromParent();

    // Create appropriate end-game widget
    TSubclassOf<UUserWidget> WidgetClass = bWon ? WinWidgetClass : LoseWidgetClass;
    if (UUserWidget* EndWidget = CreateWidget<UUserWidget>(PC, WidgetClass))
    {
        EndWidget->AddToViewport();
    }

    // Lock gameplay input and show cursor
    PC->bShowMouseCursor = true;
    PC->SetInputMode(FInputModeUIOnly());
    if (APawn* Pawn = PC->GetPawn())
    {
        Pawn->DisableInput(PC);
    }
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize survival stats with safe values
    CurrentHealth = InitializeStat(CurrentHealth, MaxHealth);
    CurrentHunger = InitializeStat(CurrentHunger, MaxHunger);
    CurrentStamina = InitializeStat(CurrentStamina, MaxStamina);

    // Set up recurring stat updates
    GetWorld()->GetTimerManager().SetTimer(
        HungerTimerHandle,
        this,
        &APlayerCharacter::UpdateHunger,
        HungerUpdateInterval,
        true // Loop indefinitely
    );

    GetWorld()->GetTimerManager().SetTimer(
        StaminaRestoreTimerHandle,
        this,
        &APlayerCharacter::UpdateStamina,
        StaminaUpdateInterval,
        true // Loop indefinitely
    );

    // Create persistent stats HUD widget
    if (StatsWidgetClass)
    {
        StatsWidgetInstance = CreateWidget<UPlayerStatsWidget>(GetWorld(), StatsWidgetClass);
        if (StatsWidgetInstance)
        {
            StatsWidgetInstance->AddToViewport();
        }
    }
}

float APlayerCharacter::InitializeStat(float CurrentValue, float MaxValue)
{
    // Ensure valid starting values
    return (CurrentValue <= 0 || CurrentValue > MaxValue) ? MaxValue : CurrentValue;
}

void APlayerCharacter::UpdateHunger()
{
    // Gradually reduce hunger
    SetHunger(CurrentHunger - HungerDecreaseRate);

    // Apply damage when starving
    if (CurrentHunger <= 0.0f)
    {
        SetHealth(CurrentHealth - StarvationDamageRate);
    }
}

void APlayerCharacter::UpdateStamina()
{
    // Calculate stamina change based on current state
    float staminaChange = bIsStaminaDraining
        ? -(StaminaDecreaseRate * StaminaUpdateInterval)  // Active drain
        : (StaminaRestoreRate * StaminaUpdateInterval);   // Passive recovery

    SetStamina(CurrentStamina + staminaChange);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // Cleanup timers to prevent lingering updates
    GetWorld()->GetTimerManager().ClearTimer(HungerTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(StaminaRestoreTimerHandle);

    // Cleanup existing UI
    if (MenuWidgetInstance) MenuWidgetInstance->RemoveFromParent();
    if (StatsWidgetInstance) StatsWidgetInstance->RemoveFromParent();
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update building preview position
    if (bIsBuildingMode && PreviewBuildable)
    {
        UpdatePreview();
    }

    // Display debug information
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

    // Camera control bindings
    PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APlayerCharacter::LookHorizontal);
    PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APlayerCharacter::LookHorizontal);
    PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APlayerCharacter::LookVertical);
    PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APlayerCharacter::LookVertical);

    // Action bindings
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::CheckInteraction);
    PlayerInputComponent->BindAction("Open Menu", IE_Pressed, this, &APlayerCharacter::ToggleMenu);

    // Building system bindings
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::PlaceBuildable);
    PlayerInputComponent->BindAction("Open Menu", IE_Pressed, this, &APlayerCharacter::CancelBuilding);
    PlayerInputComponent->BindAction("Rotate Buildable Left", IE_Pressed, this, &APlayerCharacter::RotatePreviewLeft);
    PlayerInputComponent->BindAction("Rotate Buildable Right", IE_Pressed, this, &APlayerCharacter::RotatePreviewRight);
}

void APlayerCharacter::RotatePreviewLeft()  { RotatePreviewYaw(-15.0f); }
void APlayerCharacter::RotatePreviewRight() { RotatePreviewYaw( 15.0f); }

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
        CancelBuilding(); // Cleanup existing preview
        bIsBuildingMode = true;
        bIsMenuOpen = false;

        // Spawn preview actor
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        PreviewBuildable = GetWorld()->SpawnActor<ABuildableBase>(
            BuildableToPlace, 
            FVector::ZeroVector, 
            FRotator::ZeroRotator, 
            SpawnParams
        );

        if (PreviewBuildable)
        {
            PreviewBuildable->SetActorEnableCollision(false); // Disable physics
            PreviewBuildable->BuildableMesh->SetMaterial(0, PreviewMaterial); // Apply ghost material
        }
    }
}

void APlayerCharacter::UpdatePreview()
{
    if (!PreviewBuildable) return;

    // Calculate preview position based on camera look direction
    FVector Start = FirstPersonCamera->GetComponentLocation();
    FVector End = Start + FirstPersonCamera->GetForwardVector() * InteractionRange * 2;

    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
    {
        PreviewBuildable->SetActorLocation(Hit.Location + Hit.Normal * 10.0f); // Offset from surface
    }
}

void APlayerCharacter::PlaceBuildable()
{
    if (!PreviewBuildable || !bIsBuildingMode) return;

    // Check resource availability
    bool bCanAfford = false;
    int32 AvailableResource = 0;

    switch (PreviewBuildable->MaterialType)
    {
    case EMaterialType::Wooden: AvailableResource = CurrentWood; break;
    case EMaterialType::Stone: AvailableResource = CurrentStone; break;
    }

    if (PreviewBuildable->CanAfford(AvailableResource))
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

        if (ABuildableBase* NewBuildable = GetWorld()->SpawnActor<ABuildableBase>(
            PreviewBuildable->GetClass(), 
            PreviewBuildable->GetActorTransform(), 
            SpawnParams))
        {
            // Deduct resources
            switch (PreviewBuildable->MaterialType)
            {
                case EMaterialType::Wooden: SetWood(CurrentWood - PreviewBuildable->ConstructionCost); break;
                case EMaterialType::Stone: SetStone(CurrentStone - PreviewBuildable->ConstructionCost); break;
            }

            NewBuildable->PlayPlacementEffect(); // Visual feedback
            BuildPartsCount++; // Track objective progress
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
    if (!MenuWidgetClass) return;

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController) return;

    bIsMenuOpen = !bIsMenuOpen;

    if (bIsMenuOpen)
    {
        // Create menu widget if needed
        if (!MenuWidgetInstance)
        {
            MenuWidgetInstance = CreateWidget<UUserWidget>(PlayerController, MenuWidgetClass);
        }

        MenuWidgetInstance->AddToViewport();
        if (StatsWidgetInstance) StatsWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        
        // Enable UI input mode
        PlayerController->bShowMouseCursor = true;
    }
    else
    {
        MenuWidgetInstance->RemoveFromParent();
        if (StatsWidgetInstance) StatsWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        
        // Restore game input mode
        PlayerController->bShowMouseCursor = false;
    }
}

void APlayerCharacter::CheckInteraction()
{
    if (bIsMenuOpen || bIsBuildingMode) return;

    // Perform interaction trace
    FVector Start = FirstPersonCamera->GetComponentLocation();
    FVector End = Start + (FirstPersonCamera->GetForwardVector() * InteractionRange);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
    {
        // Berry Bush interaction
        if (ABerryBush* BerryBush = Cast<ABerryBush>(HitResult.GetActor()))
        {
            if (!BerryBush->bIsCollected && GetStamina() >= 3.0f)
            {
                BerryBush->CollectBerry();
                SetStamina(GetStamina() - 3.0f);
                SetBerries(GetBerries() + 1);
            }
        }
        // Mineable Resource interaction
        else if (AMineableResource* Resource = Cast<AMineableResource>(HitResult.GetActor()))
        {
            if (!Resource->IsDepleted() && GetStamina() >= Resource->GetCurrentChunkAmount() * 3.0f)
            {
                int32 AmountMined = Resource->MineChunk();
                SetStamina(GetStamina() - AmountMined * 3.0f);
                
                switch (Resource->ResourceType)
                {
                    case EResourceType::Wood: SetWood(GetWood() + AmountMined); break;
                    case EResourceType::Stone: SetStone(GetStone() + AmountMined); break;
                    case EResourceType::Berry: SetBerries(GetBerries() + AmountMined); break;
                }
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

// Camera Control

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

// Jump Handling

void APlayerCharacter::Jump()
{
    if (!bIsMenuOpen) {
        Super::Jump();
    }
}

// Stamina Management

void APlayerCharacter::ToggleStaminaDrain()
{
    bIsStaminaDraining = !bIsStaminaDraining;
}

// Stat Getters

float APlayerCharacter::GetHealth() const { return CurrentHealth; }
float APlayerCharacter::GetHunger() const { return CurrentHunger; }
float APlayerCharacter::GetStamina() const { return CurrentStamina; }

float APlayerCharacter::GetMaxHealth() const { return MaxHealth; }
float APlayerCharacter::GetMaxHunger() const { return MaxHunger; }
float APlayerCharacter::GetMaxStamina() const { return MaxStamina; }

// Inventory Getters

int APlayerCharacter::GetWood() const { return CurrentWood; }
int APlayerCharacter::GetStone() const { return CurrentStone; }
int APlayerCharacter::GetBerries() const { return CurrentBerries; }
int APlayerCharacter::GetTotalMaterialsCollected() const { return TotalMaterialsCollected; }
int APlayerCharacter::GetBuildPartsCount() const { return BuildPartsCount; }

// Stat Setters (With clamping)

void APlayerCharacter::SetHealth(float NewHealth)
{
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    if (CurrentHealth <= 0.0f) ShowEndGameWidget(false);
}

void APlayerCharacter::SetHunger(float NewHunger)
{
    CurrentHunger = FMath::Clamp(NewHunger, 0.0f, MaxHunger);
}

void APlayerCharacter::SetStamina(float NewStamina)
{
    CurrentStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
}

// Inventory Setters (With clamping and material tracking)
void APlayerCharacter::SetWood(int NewWood)
{
    int Delta = FMath::Clamp(NewWood, 0, MaxItemSlot) - CurrentWood;
    CurrentWood = NewWood;
    if (Delta > 0) TotalMaterialsCollected += Delta;
}

void APlayerCharacter::SetStone(int NewStone)
{
    int Delta = FMath::Clamp(NewStone, 0, MaxItemSlot) - CurrentStone;
    CurrentStone = NewStone;
    if (Delta > 0) TotalMaterialsCollected += Delta;
}

void APlayerCharacter::SetBerries(int NewBerries)
{
    int Delta = FMath::Clamp(NewBerries, 0, MaxItemSlot) - CurrentBerries;
    CurrentBerries = NewBerries;
    if (Delta > 0) TotalMaterialsCollected += Delta;
}

// Debug
void APlayerCharacter::ToggleDebugStats()
{
    bShowDebugStats = !bShowDebugStats;
}

void APlayerCharacter::SetTimeLeft(float TimeLeft)
{
    StatsWidgetInstance->ObjectivesWidget->SetTimeLeft(TimeLeft);
}
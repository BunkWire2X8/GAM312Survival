
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

// Initializes stat to its maximum value if not set or if somehow gone past maximum value
float APlayerCharacter::InitializeStat(float CurrentValue, float MaxValue)
{
    return (CurrentValue == NULL || CurrentValue > MaxValue) ? MaxValue : CurrentValue;
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Debug stats display
    if (bShowDebugStats)
    {
        FString StatsText = FString::Printf(TEXT("Health: %.1f\nHunger: %.1f\nStamina: %.1f"),
            CurrentHealth, CurrentHunger, CurrentStamina);

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

// Debug
void APlayerCharacter::ToggleDebugStats()
{
    bShowDebugStats = !bShowDebugStats;
}
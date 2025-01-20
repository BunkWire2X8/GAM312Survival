#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"

/**
  * @class APlayerCharacter
  * @brief Main player character class that handles movement, inventory, and survival mechanics
  *
  * This class represents the playable character in the survival game, managing:
  * - Basic movement and camera controls
  * - Survival stats (health, hunger, stamina)
  * - Inventory system
  * - Interaction with the game world
  */
UCLASS()
class GAM312SURVIVAL_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    /* Constructor for the PlayerCharacter */
    APlayerCharacter();

protected:
    /* Called when the game starts or when spawned */
    virtual void BeginPlay() override;

    // Helper Functions

    /**
     * @brief Initializes a stat to its proper value
     * @param CurrentValue - The current value of the stat
     * @param MaxValue - The maximum allowed value for the stat
     * @return The properly initialized value
     */
    UFUNCTION()
    float InitializeStat(float CurrentValue, float MaxValue);

    // Player Stats Configuration

    /* Maximum health the player can have */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float MaxHealth = 100.0f;

    /* Maximum hunger level */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float MaxHunger = 100.0f;

    /* Maximum stamina level */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float MaxStamina = 100.0f;

    /* Current health level */
    UPROPERTY(EditAnywhere, Category = "Player Stats")
    float CurrentHealth;

    /* Current hunger level */
    UPROPERTY(EditAnywhere, Category = "Player Stats")
    float CurrentHunger;

    /* Current stamina level */
    UPROPERTY(EditAnywhere, Category = "Player Stats")
    float CurrentStamina;

    /* Timer handle for hunger updates */
    FTimerHandle HungerTimerHandle;
    /* Timer handle for stamina restoration */
    FTimerHandle StaminaRestoreTimerHandle;

    /* How quickly hunger decreases (units per update) */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float HungerDecreaseRate = 1.0f;

    /* How often hunger updates (in seconds) */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float HungerUpdateInterval = 1.0f;

    /* Damage taken per hunger update when starving */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float StarvationDamageRate = 5.0f;

    /* How quickly stamina restores (units per second) */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float StaminaRestoreRate = 10.0f;

    /* How quickly stamina drains (units per second) */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float StaminaDecreaseRate = 15.0f;

    /* How often stamina updates (in seconds) */
    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float StaminaUpdateInterval = 0.05f;

    /* Updates hunger and applies starvation damage if needed */
    UFUNCTION()
    void UpdateHunger();

    /* Updates stamina based on whether it's being drained or restored */
    UFUNCTION()
    void UpdateStamina();

    // Player Inventory Configuration

    /* Maximum amount of each item that can be carried */
    UPROPERTY(EditDefaultsOnly, Category = "Player Inventory")
    int MaxItemSlot = 999;

    /* Current amount of wood carried */
    UPROPERTY(EditAnywhere, Category = "Player Inventory")
    int CurrentWood;

    /* Current amount of stone carried */
    UPROPERTY(EditAnywhere, Category = "Player Inventory")
    int CurrentStone;

    /* Current amount of berries carried */
    UPROPERTY(EditAnywhere, Category = "Player Inventory")
    int CurrentBerries;

    // Interaction Configuration

    /* Maximum distance at which player can interact with objects */
    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
    float InteractionRange = 200.0f;

    /* Performs interaction ray trace and handles results */
    UFUNCTION()
    void CheckInteraction();

public:
    /* Called every frame */
    virtual void Tick(float DeltaTime) override;

    /* Binds functionality to input */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /* Cleanup when gameplay ends */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /* First person camera component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    UCameraComponent* FirstPersonCamera;

    // Movement Functions

    /* Handles forward/backward movement */
    UFUNCTION(BlueprintCallable, Category = "Player Movement")
    void MoveProgressive(float Value);

    /* Handles left/right movement */
    UFUNCTION(BlueprintCallable, Category = "Player Movement")
    void MoveStrafe(float Value);

    // Look Functions

    /* Handles left/right looking */
    UFUNCTION(BlueprintCallable, Category = "Player Rotation")
    void LookHorizontal(float Value);

    /* Handles up/down looking */
    UFUNCTION(BlueprintCallable, Category = "Player Rotation")
    void LookVertical(float Value);

    // Player Stats

    /* Whether stamina is currently being drained */
    UPROPERTY(EditAnywhere, Category = "Player Stats")
    bool bIsStaminaDraining = false;

    /* Toggles stamina drain state */
    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void ToggleStaminaDrain();

    // Getters

    /* Get current health value */
    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetHealth() const;

    /* Get current hunger value */
    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetHunger() const;

    /* Get current stamina value */
    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetStamina() const;

    /* Get current wood count */
    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    int GetWood() const;

    /* Get current stone count */
    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    int GetStone() const;

    /* Get current berries count */
    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    int GetBerries() const;

    // Setters

    /* Set current health value */
    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetHealth(float NewHealth);

    /* Set current hunger value */
    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetHunger(float NewHunger);

    /* Set current stamina value */
    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetStamina(float NewStamina);

    /* Set current wood count */
    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    void SetWood(int NewWood);

    /* Set current stone count */
    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    void SetStone(int NewStone);

    /* Set current berries count */
    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    void SetBerries(int NewBerries);

    // Debug

    /* Whether to show debug stats on screen */
    UPROPERTY(EditDefaultsOnly, Category = "Debug")
    bool bShowDebugStats = false;

    /* Toggles debug stats display */
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ToggleDebugStats();
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GAM312SURVIVAL_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

protected:
    virtual void BeginPlay() override;
    
    // Helper functions

    UFUNCTION()
    float InitializeStat(float CurrentValue, float MaxValue);

    // Player Stats

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float MaxHunger = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Player Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Player Stats")
    float CurrentHunger;

    UPROPERTY(EditAnywhere, Category = "Player Stats")
    float CurrentStamina;

    FTimerHandle HungerTimerHandle;
    FTimerHandle StaminaRestoreTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float HungerDecreaseRate = 1.0f;  // Units per hunger update

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float HungerUpdateInterval = 1.0f;  // How often hunger updates (in seconds)

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float StarvationDamageRate = 5.0f;  // Damage per hunger update when starving

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float StaminaRestoreRate = 10.0f;  // Units per second while resting

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float StaminaDecreaseRate = 15.0f;  // Units per second while draining

    UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
    float StaminaUpdateInterval = 0.05f;  // How often stamina updates (in seconds)

    UFUNCTION()
    void UpdateHunger();

    UFUNCTION()
    void UpdateStamina();

    // Player Inventory

    UPROPERTY(EditDefaultsOnly, Category = "Player Inventory")
    int MaxItemSlot = 999;

    UPROPERTY(EditAnywhere, Category = "Player Inventory")
    int CurrentWood;

    UPROPERTY(EditAnywhere, Category = "Player Inventory")
    int CurrentStone;

    UPROPERTY(EditAnywhere, Category = "Player Inventory")
    int CurrentBerries;

    // Interaction
    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
    float InteractionRange = 200.0f;  // How far the player can interact

    UFUNCTION()
    void CheckInteraction();  // Performs the line trace and interaction logic


public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // First Person Camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    UCameraComponent* FirstPersonCamera;

    // Movement Functions

    UFUNCTION(BlueprintCallable, Category = "Player Movement")
    void MoveProgressive(float Value);
    UFUNCTION(BlueprintCallable, Category = "Player Movement")
    void MoveStrafe(float Value);

    // Look Functions

    UFUNCTION(BlueprintCallable, Category = "Player Rotation")
    void LookHorizontal(float Value);
    UFUNCTION(BlueprintCallable, Category = "Player Rotation")
    void LookVertical(float Value);

    // Player Stats

    UPROPERTY(EditAnywhere, Category = "Player Stats")
    bool bIsStaminaDraining = false;

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void ToggleStaminaDrain();
    
    // Getters

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetHunger() const;

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetStamina() const;

    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    int GetWood() const;

    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    int GetStone() const;

    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    int GetBerries() const;

    // Setters

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetHealth(float NewHealth);

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetHunger(float NewHunger);

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetStamina(float NewStamina);

    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    void SetWood(int NewWood);

    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    void SetStone(int NewStone);

    UFUNCTION(BlueprintCallable, Category = "Player Inventory")
    void SetBerries(int NewBerries);

    // Debug

    UPROPERTY(EditDefaultsOnly, Category = "Debug")
    bool bShowDebugStats = false;

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ToggleDebugStats();
};
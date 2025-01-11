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


public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

    
    // Getters

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetHunger() const;

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    float GetStamina() const;

    // Setters

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetHealth(float NewHealth);

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetHunger(float NewHunger);

    UFUNCTION(BlueprintCallable, Category = "Player Stats")
    void SetStamina(float NewStamina);
};
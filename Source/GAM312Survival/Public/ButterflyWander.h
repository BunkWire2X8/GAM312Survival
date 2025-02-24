#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "ButterflyWander.generated.h"

/**
 * @class AButterflyWander
 * @brief Butterfly with wandering and bobbing behavior
 *
 * Uses Paper2D flipbooks for visualization and sinusoidal motion for bobbing effect and lateral movement.
 * Implements smooth movement between random points within a defined radius.
 */
UCLASS()
class GAM312SURVIVAL_API AButterflyWander : public AActor
{
    GENERATED_BODY()

public:
    AButterflyWander();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    /* Root component for transformation */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* RootComp;

    /* Visual representation of the butterfly */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UPaperFlipbookComponent* FlipbookComponent;

    /* Current target location for movement */
    UPROPERTY(VisibleInstanceOnly, Category = "Movement")
    FVector TargetLocation;

    /* Time accumulator for bobbing and lateral movement calculation */
    UPROPERTY(VisibleInstanceOnly, Category = "Movement")
    float TotalTime = 0.0f;

    /* Stores initial spawn location for radius constraint */
    FVector StartingLocation;

    /* Timer handle for wander updates */
    FTimerHandle WanderTimerHandle;

    UPROPERTY(VisibleInstanceOnly, Category = "Movement")
    FVector CurrentRightDir;

    UPROPERTY(VisibleInstanceOnly, Category = "Movement")
    float InitialDistanceToTarget;

    /**
     * @brief Generates new random target location within wander radius
     *
     * Called at regular intervals to update movement destination
     */
    UFUNCTION()
    void UpdateTargetLocation();

public:
    /* Movement speed */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1"))
    float MoveSpeed = 100.0f;

    /* Maximum wander radius from starting position */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "10.0"))
    float WanderRadius = 300.0f;

    /* Time between target updates (seconds) */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1"))
    float WanderInterval = 3.0f;

    /* Bobbing oscillation frequency */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1"))
    float BobbingFrequency = 1.0f;

    /* Bobbing vertical amplitude */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0"))
    float BobbingAmplitude = 4.0f;

    /* Lateral movement frequency */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1"))
    float LateralFrequency = 0.05f;

    /* Lateral movement amplitude */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0"))
    float LateralAmplitude = 1.5f;

    /* Vertical target variation */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0"))
    float ZVariationRange = 3.0f;
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "ButterflyWander.generated.h"

/**
 * @class AButterflyWander
 * @brief Butterfly with wandering and bobbing behavior
 *
 * Uses Paper2D flipbooks for visualization and sinusoidal motion for bobbing effect.
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
    /** Root component for transformation */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* RootComp;

    /** Visual representation of the butterfly */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UPaperFlipbookComponent* FlipbookComponent;

    /** Current target location for movement */
    UPROPERTY(VisibleInstanceOnly, Category = "Movement")
    FVector TargetLocation;

    /** Time accumulator for bobbing calculation */
    UPROPERTY(VisibleInstanceOnly, Category = "Movement")
    float BobbingTime = 0.0f;

    /** Timer handle for wander updates */
    FTimerHandle WanderTimerHandle;

    /**
     * @brief Generates new random target location within wander radius
     *
     * Called at regular intervals to update movement destination
     */
    UFUNCTION()
    void UpdateTargetLocation();

    // Editable parameters with sensible defaults
public:
    /** Movement speed in cm/s */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1"))
    float MoveSpeed = 100.0f;

    /** Maximum wander radius from starting position (cm) */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "10.0"))
    float WanderRadius = 500.0f;

    /** Time between target updates (seconds) */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1"))
    float WanderInterval = 3.0f;

    /** Bobbing oscillation frequency (Hz) */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1"))
    float BobbingFrequency = 5.0f;

    /** Bobbing vertical amplitude (cm) */
    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0"))
    float BobbingAmplitude = 10.0f;
};
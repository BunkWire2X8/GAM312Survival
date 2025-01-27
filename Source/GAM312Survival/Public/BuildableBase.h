#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildableBase.generated.h"

/**
 * @enum EMaterialType
 * @brief Defines construction material types for buildables
 */
UENUM(BlueprintType)
enum class EMaterialType : uint8
{
    Wooden  UMETA(DisplayName = "Wooden"),
    Stone   UMETA(DisplayName = "Stone")
};

/**
 * @class ABuildableBase
 * @brief Base class for all placeable structures in the game
 *
 * Handles core functionality for:
 * - Construction requirements visualization
 * - Placement validation
 * - Resource costs
 * - Material type management
 */
UCLASS()
class GAM312SURVIVAL_API ABuildableBase : public AActor
{
    GENERATED_BODY()

public:
    ABuildableBase();

    /**
     * @brief Primary visual component for the buildable
     * @tooltip Mesh representing the structure
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* BuildableMesh;

    /**
     * @brief Type of material required for construction
     * @tooltip Determines which resource is consumed
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
    EMaterialType MaterialType;

    /**
     * @brief Number of resources needed to build
     * @tooltip Cost in player inventory items
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
    int32 ConstructionCost = 10;

    /**
     * @brief Checks if player has enough resources to build
     * @param AvailableResources - Player's current resource count
     * @return True if available resources >= construction cost
     */
    UFUNCTION(BlueprintCallable, Category = "Construction")
    bool CanAfford(int32 AvailableResources) const
    {
        return AvailableResources >= ConstructionCost;
    }

protected:
    /**
     * @brief Range for valid placement checks
     * @tooltip Maximum distance from ground for valid placement
     */
    UPROPERTY(EditDefaultsOnly, Category = "Construction")
    float GroundCheckDistance = 100.0f;

    /**
     * @brief Validates potential build location
     * @param OutHit - Hit result from placement check
     * @return True if location is valid for building
     */
    UFUNCTION(BlueprintCallable, Category = "Construction")
    virtual bool IsValidPlacement(FHitResult& OutHit) const;
};
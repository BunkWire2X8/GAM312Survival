#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveVector.h"
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
 * @enum EBuildableType
 * @brief Defines types of buildable structures
 */
UENUM(BlueprintType)
enum class EBuildableType : uint8
{
    Wall    UMETA(DisplayName = "Wall"),
    Floor   UMETA(DisplayName = "Floor"),
    Slant   UMETA(DisplayName = "Slant")
};

/**
 * @class ABuildableBase
 * @brief Base class for all placeable structures in the game
 *
 * Handles core functionality for:
 * - Construction requirements visualization
 * - Placement validation
 * - Resource costs
 * - Material and structure type management
 */
UCLASS()
class GAM312SURVIVAL_API ABuildableBase : public AActor
{
    GENERATED_BODY()

public:
    ABuildableBase();

    /**
     * @brief Primary visual component for the buildable
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
     * @brief Type of buildable structure
     * @tooltip Determines which mesh to use
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
    EBuildableType BuildableType;

    /**
     * @brief Number of resources needed to build
     * @tooltip Cost in player inventory items
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
    int32 ConstructionCost = 10;

    /**
     * @brief Gets the construction cost
     * @return The number of resources required to build
     */
    UFUNCTION(BlueprintCallable, Category = "Construction")
    int32 GetConstructionCost() const { return ConstructionCost; }

    /**
     * @brief Checks if player has enough resources to build
     * @param AvailableResources - Player's current resource count
     * @return True if available resources >= construction cost
     */
    UFUNCTION(BlueprintCallable, Category = "Construction")
    bool CanAfford(int32 AvailableResources) const { return AvailableResources >= ConstructionCost; }

    /**
     * @brief Gets the buildable type as a string
     * @return Name of the buildable type enum
     */
    UFUNCTION(BlueprintCallable, Category = "Construction")
    FString GetBuildableTypeString() const;

    /**
     * @brief Gets the material type as a string
     * @return Name of the material type enum
     */
    UFUNCTION(BlueprintCallable, Category = "Construction")
    FString GetMaterialTypeString() const;

    /* Timeline for placement animation */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UTimelineComponent* ScaleTimeline;

    /* Curve defining scale animation */
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UCurveVector* ScaleCurve = LoadObject<UCurveVector>(nullptr, TEXT("/Game/Blueprints/Buildables/ScaleCurve"));

    /* Trigger placement effect animation */
    UFUNCTION(BlueprintCallable, Category = "Construction")
    void PlayPlacementEffect();

protected:
    /* Called when the game starts or when spawned */
    virtual void BeginPlay() override;

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

    /**
     * @brief Updates mesh based on current material and buildable types
     * @tooltip Loads appropriate mesh from content directory
     */
    void UpdateMesh();

    /* Timeline update callback */
    UFUNCTION()
    void UpdateScale(FVector Scale);

    /* Timeline finished callback */
    UFUNCTION()
    void OnScaleTimelineCompleted();

private:
    FOnTimelineVector ScaleTimelineInterp;
};
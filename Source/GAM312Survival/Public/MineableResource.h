#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MineableResource.generated.h"

/**
 * @enum EResourceType
 * @brief Defines the various types of resources that can be mined in the game
 */
UENUM(BlueprintType)
enum class EResourceType : uint8
{
    Wood,   ///< Wood resource type
    Stone,  ///< Stone resource type
    Berry   ///< Berry resource type
};

/**
 * @struct FResourceState
 * @brief Represents a single state of a resource, including its visual mesh and quantity
 * Each resource can have multiple states (e.g., five wooden logs, three wooden logs, one wooden log)
 */
USTRUCT(BlueprintType)
struct FResourceState
{
    GENERATED_BODY()

    /* The 3D mesh representation for this resource state */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    UStaticMesh* ResourceMesh = nullptr;

    /* The amount of resource available in this state */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource", Meta = (ClampMin = "0"))
    int32 ResourceAmount = 0;
};

/**
 * @class AMineableResource
 * @brief An actor representing a resource that can be mined/harvested in the game world
 *
 * This class handles the visual representation and state management of mineable resources.
 * It supports multiple states of depletion (e.g., full -> partially mined -> depleted)
 * and handles the transition between these states as the resource is mined.
 */
UCLASS(ClassGroup = (Custom), Meta = (BlueprintSpawnableComponent))
class GAM312SURVIVAL_API AMineableResource : public AActor
{
    GENERATED_BODY()

public:
    /* Constructor for the MineableResource */
    AMineableResource();

    /* The type of resource this actor represents */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    EResourceType ResourceType;

    /**
     * @brief Mines a specific amount from the resource
     * @param AmountToMine - The amount to attempt to mine
     * @return The amount of resource mined (may be less than requested if near depletion)
     */
    UFUNCTION(BlueprintCallable, Category = "Mining")
    int32 Mine(int32 AmountToMine);

    /**
     * @brief Mines an entire chunk of the resource, transitioning to the next state
     * @return The amount of resource mined
     */
    UFUNCTION(BlueprintCallable, Category = "Mining")
    int32 MineChunk();

    /**
     * @brief Gets the amount of resource in the current chunk
     * @return The amount of resource in the current chunk (difference between current and next state)
     */
    UFUNCTION(BlueprintPure, Category = "Mining")
    int32 GetCurrentChunkAmount() const;

    /**
     * @brief Checks if the resource has been fully depleted
     * @return True if no resources remain, false otherwise
     */
    UFUNCTION(BlueprintPure, Category = "Mining")
    bool IsDepleted() const;

    /**
     * @brief Gets the current amount of resource remaining
     * @return The remaining resource amount
     */
    UFUNCTION(BlueprintPure, Category = "Mining")
    int32 GetRemainingResource() const;

protected:
    /* Called when the game starts or when spawned */
    virtual void BeginPlay() override;

#if WITH_EDITOR
    /* Handles property changes in the editor */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    /* The visual mesh component for the resource */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> ResourceMesh;

    /* Array of possible states for this resource (from full to depleted) */
    UPROPERTY(EditAnywhere, Category = "Resource Configuration")
    TArray<FResourceState> ResourceStates;

    /* Current state index in the ResourceStates array */
    UPROPERTY(EditAnywhere, Category = "Resource Configuration", Meta = (ClampMin = "0"))
    int32 CurrentStateIndex;

    /* Initial state index when the resource spawns */
    UPROPERTY(EditAnywhere, Category = "Resource Configuration", Meta = (ClampMin = "0"))
    int32 InitialStateIndex;

    /* Whether to reset to initial state on BeginPlay */
    UPROPERTY(EditAnywhere, Category = "Resource Configuration")
    bool bResetOnBeginPlay;

    /* Current amount of resource remaining */
    UPROPERTY(VisibleAnywhere, Category = "Resource State")
    int32 RemainingResource;

    /* Updates the visual mesh based on current state */
    void UpdateMeshState();

    /* Ensures state indices are within valid range */
    void ValidateIndices();

    /* Updates the resource state based on remaining amount */
    void UpdateStateBasedOnResource();
};
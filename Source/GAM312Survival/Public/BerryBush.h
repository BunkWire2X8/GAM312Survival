#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BerryBush.generated.h"

/**
 * @class ABerryBush
 * @brief Represents a berry bush actor in the game world that players can harvest
 * 
 * This class manages a bush with collectable berries that regrow over time.
 * It includes both visual representation (meshes) and growth mechanics.
 * The berries visually grow back using both scaling and material effects.
 */
UCLASS()
class GAM312SURVIVAL_API ABerryBush : public AActor
{
    GENERATED_BODY()

public:
    /* Constructor for the BerryBush */
    ABerryBush();

protected:
    /* Called when the game starts or when spawned */
    virtual void BeginPlay() override;

    /* Called every frame to update berry growth */
    virtual void Tick(float DeltaTime) override;

    /* Static mesh component for the bush base structure */
    UPROPERTY(EditAnywhere, Category = "Meshes", Meta = (ToolTip = "The main bush mesh that remains visible at all times"))
    UStaticMeshComponent* BushMesh;

    /* Static mesh component for the berries */
    UPROPERTY(EditAnywhere, Category = "Meshes", Meta = (ToolTip = "The berry mesh that scales and potentially disappears when harvested"))
    UStaticMeshComponent* BerryMesh;

    /* Time in seconds for berries to fully regrow */
    UPROPERTY(EditAnywhere, Category = "Growth", Meta = (ClampMin = "0.0", ToolTip = "Duration in seconds for berries to completely regrow"))
    float RegrowthTime = 10.0f;

    /* Material parameter name for controlling berry growth visualization */
    UPROPERTY(EditAnywhere, Category = "Growth", Meta = (ToolTip = "Parameter name in the material that controls growth visualization"))
    FName GrowthParameterName = TEXT("Growth");

    /* Dynamic material instance for berry growth effects */
    UPROPERTY()
    UMaterialInstanceDynamic* BerryMaterialInstance;

    /* Current progress of berry regrowth (0.0 to 1.0) */
    float RegrowthProgress = 1.0f;

public:
   /**
    * @brief Collects berries from the bush if available
    *
    * When called, this function will:
    * 1. Check if berries are available
    * 2. If available, mark as collected and reset growth progress
    * 3. Trigger visual updates through scaling and material effects
    */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void CollectBerry();

    /* Tracks whether berries have been collected and are currently regrowing */
    UPROPERTY(EditAnywhere, Category = "Growth", Meta = (ToolTip = "True if berries have been collected and are regrowing"))
    bool bIsCollected = false;
};
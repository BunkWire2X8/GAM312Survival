#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BerryBush.generated.h"

UCLASS()
class GAM312SURVIVAL_API ABerryBush : public AActor
{
    GENERATED_BODY()

public:
    ABerryBush();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // The bush base mesh (always visible)
    UPROPERTY(EditAnywhere, Category = "Meshes")
    UStaticMeshComponent* BushMesh;

    // The berry mesh (will disappear when collected)
    UPROPERTY(EditAnywhere, Category = "Meshes")
    UStaticMeshComponent* BerryMesh;

    // Time it takes for berry to fully regrow
    UPROPERTY(EditAnywhere, Category = "Growth")
    float RegrowthTime = 10.0f;

    // Name of the growth parameter in the material
    UPROPERTY(EditAnywhere, Category = "Growth")
    FName GrowthParameterName = TEXT("Growth");

    // Material instance that will be created at runtime
    UPROPERTY()
    UMaterialInstanceDynamic* BerryMaterialInstance;

    // Current regrowth progress (0-1)
    float RegrowthProgress = 1.0f;

public:
    // Function to collect the berry
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void CollectBerry();

    // Is the berry currently collected?
    UPROPERTY(EditAnywhere, Category = "Growth")
    bool bIsCollected = false;
};
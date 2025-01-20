#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MineableResource.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
    Wood,
    Stone,
    Berry
};

// Represents a single state of the resource with its mesh and amount
USTRUCT(BlueprintType)
struct FResourceState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    UStaticMesh* ResourceMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource", Meta = (ClampMin = "0"))
    int32 ResourceAmount = 0;
};

UCLASS(ClassGroup = (Custom), Meta = (BlueprintSpawnableComponent))
class GAM312SURVIVAL_API AMineableResource : public AActor
{
    GENERATED_BODY()

public:
    AMineableResource();
    
    // Resource Type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    EResourceType ResourceType;

    // Mining interface

    UFUNCTION(BlueprintCallable, Category = "Mining")
    int32 Mine(int32 AmountToMine);

    UFUNCTION(BlueprintCallable, Category = "Mining")
    int32 MineChunk();

    UFUNCTION(BlueprintPure, Category = "Mining")
    bool IsDepleted() const;

    UFUNCTION(BlueprintPure, Category = "Mining")
    int32 GetRemainingResource() const;

protected:
    virtual void BeginPlay() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    // Core components

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> ResourceMesh;

    // Resource configuration

    UPROPERTY(EditAnywhere, Category = "Resource Configuration")
    TArray<FResourceState> ResourceStates;

    UPROPERTY(EditAnywhere, Category = "Resource Configuration", Meta = (ClampMin = "0"))
    int32 CurrentStateIndex;

    UPROPERTY(EditAnywhere, Category = "Resource Configuration", Meta = (ClampMin = "0"))
    int32 InitialStateIndex;

    UPROPERTY(EditAnywhere, Category = "Resource Configuration")
    bool bResetOnBeginPlay;

    // Runtime state

    UPROPERTY(VisibleAnywhere, Category = "Resource State")
    int32 RemainingResource;

    // Helper functions

    void UpdateMeshState();
    void ValidateIndices();
    void UpdateStateBasedOnResource();
};
#include "BuildableBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"

ABuildableBase::ABuildableBase()
{
    PrimaryActorTick.bCanEverTick = false;

    BuildableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildableMesh"));
    SetRootComponent(BuildableMesh);
}

void ABuildableBase::BeginPlay()
{
    Super::BeginPlay();
    UpdateMesh();
}

int32 ABuildableBase::GetConstructionCost() const
{
    return ConstructionCost;
}

bool ABuildableBase::CanAfford(int32 Inventory) const
{
    return Inventory >= ConstructionCost;
}

FString ABuildableBase::GetBuildableTypeString() const
{
    return StaticEnum<EBuildableType>()->GetNameStringByValue(static_cast<int64>(static_cast<uint8>(BuildableType)));
}

FString ABuildableBase::GetMaterialTypeString() const
{
    return StaticEnum<EMaterialType>()->GetNameStringByValue(static_cast<int64>(static_cast<uint8>(MaterialType)));
}

void ABuildableBase::UpdateMesh()
{
    FString MeshPath;
    
    const FString TypeString = GetBuildableTypeString().ToLower();
    const FString MaterialString = GetMaterialTypeString().ToLower();

    MeshPath = FString::Printf(TEXT("/Game/Assets/Models/Building/%s_%s"), *MaterialString, *TypeString);

    if (UStaticMesh* LoadedMesh = LoadObject<UStaticMesh>(nullptr, *MeshPath))
    {
        BuildableMesh->SetStaticMesh(LoadedMesh);
    }
}
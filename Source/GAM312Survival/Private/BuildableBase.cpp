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

FString ABuildableBase::GetBuildableTypeString() const
{
    switch (BuildableType)
    {
    case EBuildableType::Wall:
        return TEXT("Wall");
    case EBuildableType::Floor:
        return TEXT("Floor");
    case EBuildableType::Slant:
        return TEXT("Slant");
    default:
        return TEXT("Unknown");
    }
}

FString ABuildableBase::GetMaterialTypeString() const
{
    switch (MaterialType)
    {
    case EMaterialType::Wooden:
        return TEXT("Wooden");
    case EMaterialType::Stone:
        return TEXT("Stone");
    default:
        return TEXT("Unknown");
    }
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

bool ABuildableBase::IsValidPlacement(FHitResult& OutHit) const
{
    // Implementation for placement validation
    return true;
}
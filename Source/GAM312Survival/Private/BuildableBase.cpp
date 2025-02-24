#include "BuildableBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"

ABuildableBase::ABuildableBase()
{
    // Set this actor to never tick
    PrimaryActorTick.bCanEverTick = false;

    // Initialize main mesh component and set as root
    BuildableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildableMesh"));
    SetRootComponent(BuildableMesh);

    // Initialize timeline component for scale animations
    ScaleTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ScaleTimeline"));
    ScaleTimeline->SetLooping(false);
}

void ABuildableBase::BeginPlay()
{
    Super::BeginPlay();

    // Configure timeline if curve is available
    if (ScaleCurve)
    {
        // Bind timeline update callback
        ScaleTimelineInterp.BindUFunction(this, FName("UpdateScale"));
        ScaleTimeline->AddInterpVector(ScaleCurve, ScaleTimelineInterp);

        // Bind timeline completion callback
        FOnTimelineEvent TimelineFinished;
        TimelineFinished.BindUFunction(this, FName("OnScaleTimelineCompleted"));
        ScaleTimeline->SetTimelineFinishedFunc(TimelineFinished);
    }

    // Load appropriate mesh based on initial settings
    UpdateMesh();
}

FString ABuildableBase::GetBuildableTypeString() const
{
    // Convert buildable type enum to readable string
    switch (BuildableType)
    {
    case EBuildableType::Wall:    return TEXT("Wall");
    case EBuildableType::Floor:   return TEXT("Floor");
    case EBuildableType::Slant:   return TEXT("Slant");
    default:                      return TEXT("Unknown");
    }
}

FString ABuildableBase::GetMaterialTypeString() const
{
    // Convert material type enum to readable string
    switch (MaterialType)
    {
    case EMaterialType::Wooden:   return TEXT("Wooden");
    case EMaterialType::Stone:    return TEXT("Stone");
    default:                      return TEXT("Unknown");
    }
}

void ABuildableBase::UpdateMesh()
{
    // Construct mesh path based on current material and type
    FString MeshPath;
    const FString TypeString = GetBuildableTypeString().ToLower();
    const FString MaterialString = GetMaterialTypeString().ToLower();

    // Format: "/Game/Assets/Models/Building/[Material]_[Type]"
    // Example: "/Game/Assets/Models/Building/wooden_wall"
    MeshPath = FString::Printf(TEXT("/Game/Assets/Models/Building/%s_%s"), *MaterialString, *TypeString);

    // Attempt to load and apply mesh
    if (UStaticMesh* LoadedMesh = LoadObject<UStaticMesh>(nullptr, *MeshPath))
    {
        BuildableMesh->SetStaticMesh(LoadedMesh);
    }
}

bool ABuildableBase::IsValidPlacement(FHitResult& OutHit) const
{
    // Base implementation always returns valid
    // Derived classes should override with specific checks:
    // - Ground proximity
    // - Collision detection
    // - Snapping to grid
    return true;
}

void ABuildableBase::PlayPlacementEffect()
{
    // Start scale animation if timeline is available
    if (ScaleTimeline)
    {
        ScaleTimeline->PlayFromStart();
    }
}

void ABuildableBase::UpdateScale(FVector Scale)
{
    // Apply current timeline scale value to mesh
    BuildableMesh->SetWorldScale3D(Scale);
}

void ABuildableBase::OnScaleTimelineCompleted()
{
    // Ensure final scale is reset after animation
    BuildableMesh->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
}
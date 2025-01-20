#include "BerryBush.h"

ABerryBush::ABerryBush()
{
    // Enable tick for regrowth mechanics
    PrimaryActorTick.bCanEverTick = true;

    // Initialize and setup the bush base mesh
    BushMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BushMesh"));
    RootComponent = BushMesh;

    // Initialize and setup the berry mesh as child of bush
    BerryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BerryMesh"));
    BerryMesh->SetupAttachment(BushMesh);
}

void ABerryBush::BeginPlay()
{
    Super::BeginPlay();

    // Setup dynamic material instance for berry growth effects
    if (BerryMesh)
    {
        // Retrieve the base material from the berry mesh
        UMaterialInterface* Material = BerryMesh->GetMaterial(0);
        if (Material)
        {
            // Create dynamic instance for runtime modification
            BerryMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
            if (BerryMaterialInstance)
            {
                // Apply dynamic material and set initial growth state
                BerryMesh->SetMaterial(0, BerryMaterialInstance);
                BerryMaterialInstance->SetScalarParameterValue(GrowthParameterName, RegrowthProgress);
            }
        }
    }
}

void ABerryBush::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Handle berry regrowth when collected
    if (bIsCollected)
    {
        // Progress the regrowth based on time
        RegrowthProgress += DeltaTime / RegrowthTime;

        // Cap regrowth at 100% and mark as available
        if (RegrowthProgress >= 1.0f)
        {
            RegrowthProgress = 1.0f;
            bIsCollected = false;
        }

        // Update visual representation
        // Scale the berry mesh based on growth progress
        FVector NewScale = FVector(RegrowthProgress);
        BerryMesh->SetRelativeScale3D(NewScale);

        // Update material effects if available
        if (BerryMaterialInstance)
        {
            BerryMaterialInstance->SetScalarParameterValue(GrowthParameterName, RegrowthProgress);
        }
    }
}

void ABerryBush::CollectBerry()
{
    // Only allow collection if berries are available
    if (!bIsCollected)
    {
        bIsCollected = true;
        RegrowthProgress = 0.0f;
    }
}
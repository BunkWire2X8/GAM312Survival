#include "BerryBush.h"

ABerryBush::ABerryBush()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and setup the bush mesh
    BushMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BushMesh"));
    RootComponent = BushMesh;

    // Create and setup the berry mesh
    BerryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BerryMesh"));
    BerryMesh->SetupAttachment(BushMesh);
}

void ABerryBush::BeginPlay()
{
    Super::BeginPlay();

    // Create dynamic material instance
    if (BerryMesh)
    {
        // Get the first material from the mesh
        UMaterialInterface* Material = BerryMesh->GetMaterial(0);
        if (Material)
        {
            // Create the dynamic material instance
            BerryMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
            if (BerryMaterialInstance)
            {
                // Apply the dynamic material instance to the mesh
                BerryMesh->SetMaterial(0, BerryMaterialInstance);

                // Initialize the growth parameter
                BerryMaterialInstance->SetScalarParameterValue(GrowthParameterName, RegrowthProgress);
            }
        }
    }
}

void ABerryBush::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // If berry is collected, handle regrowth
    if (bIsCollected)
    {
        RegrowthProgress += DeltaTime / RegrowthTime;

        if (RegrowthProgress >= 1.0f)
        {
            RegrowthProgress = 1.0f;
            bIsCollected = false;
        }

        // Update both scale and material parameter
        FVector NewScale = FVector(RegrowthProgress);
        BerryMesh->SetRelativeScale3D(NewScale);

        if (BerryMaterialInstance)
        {
            BerryMaterialInstance->SetScalarParameterValue(GrowthParameterName, RegrowthProgress);
        }
    }
}

void ABerryBush::CollectBerry()
{
    if (!bIsCollected)
    {
        bIsCollected = true;
        RegrowthProgress = 0.0f;
    }
}
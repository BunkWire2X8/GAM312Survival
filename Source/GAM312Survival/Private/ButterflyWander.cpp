#include "ButterflyWander.h"
#include "Math/UnrealMathUtility.h"

AButterflyWander::AButterflyWander()
{
    PrimaryActorTick.bCanEverTick = true;

    // Setup components
    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = RootComp;

    FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbookComponent"));
    FlipbookComponent->SetupAttachment(RootComponent);
}

void AButterflyWander::BeginPlay()
{
    Super::BeginPlay();

    // Store initial spawn location
    StartingLocation = GetActorLocation();
    TargetLocation = StartingLocation;

    // Start wander timer timer with random interval
    GetWorldTimerManager().SetTimer(
        WanderTimerHandle,
        this,
        &AButterflyWander::UpdateTargetLocation,
        FMath::RandRange(WanderInterval * 0.5f, WanderInterval * 1.5f),
        false
    );

    // Make sure that wandering starts immediately
    UpdateTargetLocation();
}

void AButterflyWander::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector NewLocation = FMath::VInterpConstantTo(
        GetActorLocation(),
        TargetLocation,
        DeltaTime,
        MoveSpeed
    );

    // Calculate remaining distance to target
    float DistanceToTarget = FVector::Dist2D(NewLocation, TargetLocation);
    float LateralScale = (InitialDistanceToTarget > 0.0f) 
        ? FMath::Clamp(DistanceToTarget / InitialDistanceToTarget, 0.0f, 1.0f)
        : 0.0f;

    // Increment total time
    TotalTime += DeltaTime;

    // Apply scaled lateral movement
    float LateralOffset = FMath::Sin(TotalTime * LateralFrequency * 2 * PI) * LateralAmplitude * LateralScale;
    NewLocation += CurrentRightDir * LateralOffset;

    SetActorLocation(NewLocation);

    // Update bobbing effect
    float ZOffset = FMath::Sin(TotalTime * BobbingFrequency * 2 * PI) * BobbingAmplitude;
    FlipbookComponent->SetRelativeLocation(FVector(0, 0, ZOffset));
}

void AButterflyWander::UpdateTargetLocation()
{
    // Random angle, radius, and vertical variation for movement
    float Angle = FMath::RandRange(0.0f, 2 * PI);
    float Radius = FMath::RandRange(0.0f, WanderRadius);
    float ZVariation = FMath::RandRange(-ZVariationRange, ZVariationRange);

    FVector2D CirclePoint = FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius;
    TargetLocation = FVector(
        StartingLocation.X + CirclePoint.X,
        StartingLocation.Y + CirclePoint.Y,
        StartingLocation.Z + ZVariation
    );

    // Calculate initial direction to target and right direction
    FVector ToTarget = (TargetLocation - StartingLocation).GetSafeNormal();
    ToTarget.Z = 0.0f; // Restrict to horizontal plane
    CurrentRightDir = FVector::CrossProduct(ToTarget, FVector::UpVector).GetSafeNormal();

    // Store initial distance to target
    InitialDistanceToTarget = FVector::Dist2D(GetActorLocation(), TargetLocation);

    // Set next timer with random interval
    GetWorldTimerManager().SetTimer(
        WanderTimerHandle,
        this,
        &AButterflyWander::UpdateTargetLocation,
        FMath::RandRange(WanderInterval * 0.5f, WanderInterval * 1.5f),
        false
    );
}

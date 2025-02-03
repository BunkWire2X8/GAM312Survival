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

    // Initial target is current location
    TargetLocation = GetActorLocation();

    // Start wander timer
    GetWorldTimerManager().SetTimer(
        WanderTimerHandle,
        this,
        &AButterflyWander::UpdateTargetLocation,
        WanderInterval,
        true
    );
}

void AButterflyWander::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    
    // Smooth movement towards target
    const FVector CurrentLocation = GetActorLocation();
    const FVector NewLocation = FMath::VInterpConstantTo(
        CurrentLocation,
        TargetLocation,
        DeltaTime,
        MoveSpeed
    );
    SetActorLocation(NewLocation);

    // Update bobbing effect
    BobbingTime += DeltaTime;
    const float ZOffset = FMath::Sin(BobbingTime * BobbingFrequency * 2 * PI) * BobbingAmplitude;
    FlipbookComponent->SetRelativeLocation(FVector(0, 0, ZOffset));
}

void AButterflyWander::UpdateTargetLocation()
{
    const FVector CurrentLocation = GetActorLocation();
    const float Angle = FMath::RandRange(0.0f, 2 * PI);
    const float Radius = FMath::RandRange(0.0f, WanderRadius);

    const FVector2D CirclePoint = FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius;
    TargetLocation = FVector(
        CurrentLocation.X + CirclePoint.X,
        CurrentLocation.Y + CirclePoint.Y,
        CurrentLocation.Z // Maintain original Z position
    );
}
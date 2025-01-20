#include "MineableResource.h"

AMineableResource::AMineableResource()
{
    // Initialize components
    ResourceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ResourceMesh"));
    RootComponent = ResourceMesh;
    CurrentStateIndex = 0;
}

void AMineableResource::BeginPlay()
{
    Super::BeginPlay();

    // Reset to initial state if configured
    if (bResetOnBeginPlay)
    {
        CurrentStateIndex = InitialStateIndex;
    }

    ValidateIndices();
    UpdateMeshState();
}

void AMineableResource::ValidateIndices()
{
    if (ResourceStates.Num() > 0)
    {
        // Ensure indices stay within valid range
        CurrentStateIndex = FMath::Clamp(CurrentStateIndex, 0, ResourceStates.Num() - 1);
        InitialStateIndex = FMath::Clamp(InitialStateIndex, 0, ResourceStates.Num() - 1);
    }
}

void AMineableResource::UpdateMeshState()
{
    if (ResourceStates.IsValidIndex(CurrentStateIndex))
    {
        // Update resource amount and mesh for current state
        RemainingResource = ResourceStates[CurrentStateIndex].ResourceAmount;

        if (ResourceStates[CurrentStateIndex].ResourceMesh)
        {
            ResourceMesh->SetStaticMesh(ResourceStates[CurrentStateIndex].ResourceMesh);
            ResourceMesh->SetVisibility(true);
        }
        else
        {
            ResourceMesh->SetStaticMesh(nullptr);
            ResourceMesh->SetVisibility(false);
        }
    }
}

#if WITH_EDITOR
void AMineableResource::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName PropertyName = (PropertyChangedEvent.Property != nullptr)
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    if (ResourceStates.Num() > 0)
    {
        ValidateIndices();

        // Update visual state when relevant properties change in editor
        if (PropertyName == GET_MEMBER_NAME_CHECKED(AMineableResource, CurrentStateIndex) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(AMineableResource, ResourceStates) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(FResourceState, ResourceMesh) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(FResourceState, ResourceAmount))
        {
            UpdateMeshState();
        }
    }
}
#endif

void AMineableResource::UpdateStateBasedOnResource()
{
    // Iterate through states from most depleted to most full
    for (int32 i = ResourceStates.Num() - 1; i >= 0; --i)
    {
        if (RemainingResource <= ResourceStates[i].ResourceAmount)
        {
            if (CurrentStateIndex != i)
            {
                CurrentStateIndex = i;
                UpdateMeshState();
            }
            break;
        }
    }
}

int32 AMineableResource::MineChunk()
{
    if (IsDepleted()) return 0;

    // Calculate mining amount based on difference between current and next state
    int32 NextStateThreshold = ResourceStates[CurrentStateIndex + 1].ResourceAmount;
    int32 AmountToMine = RemainingResource - NextStateThreshold;

    return Mine(AmountToMine);
}

int32 AMineableResource::Mine(int32 AmountToMine)
{
    if (IsDepleted()) return 0;

    // Ensure we don't mine more than what's available
    int32 ActualMined = FMath::Min(AmountToMine, RemainingResource);
    RemainingResource -= ActualMined;

    UpdateStateBasedOnResource();
    return ActualMined;
}

bool AMineableResource::IsDepleted() const
{
    return RemainingResource <= 0;
}

int32 AMineableResource::GetRemainingResource() const
{
    return RemainingResource;
}
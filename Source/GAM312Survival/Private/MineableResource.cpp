#include "MineableResource.h"

AMineableResource::AMineableResource()
{
    ResourceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ResourceMesh"));
    RootComponent = ResourceMesh;
    CurrentStateIndex = 0;
}

void AMineableResource::BeginPlay()
{
    Super::BeginPlay();

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
        CurrentStateIndex = FMath::Clamp(CurrentStateIndex, 0, ResourceStates.Num() - 1);
        InitialStateIndex = FMath::Clamp(InitialStateIndex, 0, ResourceStates.Num() - 1);
    }
}

void AMineableResource::UpdateMeshState()
{
    if (ResourceStates.IsValidIndex(CurrentStateIndex))
    {
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

        // Update mesh in-editor if relevant properties changed
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

    // Get the threshold for the next lower state
    int32 NextStateThreshold = ResourceStates[CurrentStateIndex + 1].ResourceAmount;

    // Calculate how much to mine: difference between current remaining and next state threshold
    int32 AmountToMine = RemainingResource - NextStateThreshold;

    return Mine(AmountToMine);
}

int32 AMineableResource::Mine(int32 AmountToMine)
{
    if (IsDepleted()) return 0;

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

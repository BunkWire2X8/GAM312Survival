#include "ObjectivesWidget.h"
#include "PlayerCharacter.h"
#include "Components/TextBlock.h"

void UObjectivesWidget::NativeConstruct()
{
    Super::NativeConstruct();
    PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
}

void UObjectivesWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (PlayerCharacter.IsValid())
    {
        RefreshObjectiveDisplay();
        CheckEndCondition(InDeltaTime);
    }
    else
    {
        PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
    }
}

void UObjectivesWidget::RefreshObjectiveDisplay()
{
    if (!PlayerCharacter.IsValid()) return;

    // Format materials objective text
    FString MaterialsText = FString::Printf(TEXT("Collect Materials: %d/500"),
                                            PlayerCharacter->GetTotalMaterialsCollected());
    MaterialsObjectiveText->SetText(FText::FromString(MaterialsText));

    // Format building objective text
    FString BuildText = FString::Printf(TEXT("Build Parts: %d/5"),
                                        PlayerCharacter->GetBuildPartsCount());
    BuildObjectiveText->SetText(FText::FromString(BuildText));

    float RemainingSeconds = FMath::Max(0.0f, LosingTime - TimeElapsed);
    int32 Minutes = FMath::FloorToInt(RemainingSeconds / 60);
    int32 Seconds = FMath::FloorToInt(RemainingSeconds) % 60;

    FString TimerString = FString::Printf(TEXT("Time Remaining: %02d:%02d"), Minutes, Seconds);
    TimeRemainingText->SetText(FText::FromString(TimerString));
}

void UObjectivesWidget::CheckEndCondition(float InDeltaTime)
{
    // Handle completion status
    bool bAllComplete = PlayerCharacter->GetTotalMaterialsCollected() >= 500 &&
                        PlayerCharacter->GetBuildPartsCount() >= 5;

    if (bAllComplete)
    {
        PlayerCharacter->ShowEndGameWidget(true); // 'true' means win
    }
    else if (!bHasTriggeredTimeout)
    {
        TimeElapsed += InDeltaTime;
        if (TimeElapsed >= LosingTime)
        {
            bHasTriggeredTimeout = true;
            PlayerCharacter->ShowEndGameWidget(false); // 'false' means lose
        }
    }
}

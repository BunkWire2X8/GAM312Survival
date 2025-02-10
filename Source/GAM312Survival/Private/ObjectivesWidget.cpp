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

    // Handle completion status
    bool bAllComplete = PlayerCharacter->GetTotalMaterialsCollected() >= 500 &&
                        PlayerCharacter->GetBuildPartsCount() >= 5;

    CompletionStatusText->SetVisibility(
        bAllComplete ? ESlateVisibility::Visible : ESlateVisibility::Hidden
    );

    if (bAllComplete)
    {
        CompletionStatusText->SetText(FText::FromString("ALL OBJECTIVES COMPLETED!"));
    }
}
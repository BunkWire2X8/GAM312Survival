#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectivesWidget.generated.h"

class UTextBlock;
class APlayerCharacter;

/**
 * @class UObjectivesWidget
 * @brief Displays real-time progress towards game completion objectives. Only contains bare-miminum functionality because I don't have the time to get all fancy.
 */
UCLASS()
class GAM312SURVIVAL_API UObjectivesWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /* Initializes widget and locates player character reference */
    virtual void NativeConstruct() override;

    /* Updates objective progress every frame */
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    /* Text element for materials collection progress */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MaterialsObjectiveText;

    /* Text element for building progress */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BuildObjectiveText;

    /* Text element showing completion status */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CompletionStatusText;

private:
    /* Cached reference to player character */
    TWeakObjectPtr<APlayerCharacter> PlayerCharacter;

    /* Updates all objective display elements */
    void RefreshObjectiveDisplay();
};
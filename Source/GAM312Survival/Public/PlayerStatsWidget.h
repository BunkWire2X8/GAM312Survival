#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectivesWidget.h"
#include "PlayerStatsWidget.generated.h"

// Forward declarations
class UTextBlock;
class APlayerCharacter;

/**
  * @class UPlayerStatsWidget
  * @brief Widget class that displays real-time player statistics (health, hunger, stamina)
  *
  * This widget shows formatted numerical values for the player's survival stats
  * and updates them using native widget tick. Requires binding to text elements in UMG.
  */
UCLASS()
class GAM312SURVIVAL_API UPlayerStatsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * @brief Initializes widget and attempts to find player character
     * @brief Called when widget is added to viewport
     */
    virtual void NativeConstruct() override;

    /**
     * @brief Main update loop for stat values
     * @param MyGeometry - Widget's geometric data
     * @param InDeltaTime - Time since last frame
     */
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /* Widget displaying and managing objectives */
    UPROPERTY(meta = (BindWidget))
    UObjectivesWidget* ObjectivesWidget;

protected:
    /* Text element displaying formatted health value */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthValueText;

    /* Text element displaying formatted hunger value */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HungerValueText;

    /* Text element displaying formatted stamina value */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* StaminaValueText;

private:
    /* Cached reference to player character for stat access */
    TWeakObjectPtr<APlayerCharacter> PlayerCharacter;

    /* Formatting helper function */
    FText FormatStatText(float CurrentValue, float MaxValue, const FString& Label) const;
};
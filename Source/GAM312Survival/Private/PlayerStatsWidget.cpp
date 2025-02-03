#include "PlayerStatsWidget.h"
#include "PlayerCharacter.h"
#include "Components/TextBlock.h"

void UPlayerStatsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Safely get player character reference
    PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
}

void UPlayerStatsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (PlayerCharacter.IsValid() && HealthValueText && HungerValueText && StaminaValueText)
    {
        // Format and update health display
        HealthValueText->SetText(FormatStatText(
            PlayerCharacter->GetHealth(),
            PlayerCharacter->GetMaxHealth(),
            TEXT("Health")
        ));

        // Format and update hunger display
        HungerValueText->SetText(FormatStatText(
            PlayerCharacter->GetHunger(),
            PlayerCharacter->GetMaxHunger(),
            TEXT("Hunger")
        ));

        // Format and update stamina display
        StaminaValueText->SetText(FormatStatText(
            PlayerCharacter->GetStamina(),
            PlayerCharacter->GetMaxStamina(),
            TEXT("Stamina")
        ));
    }
    else
    {
        PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
    }
}

FText UPlayerStatsWidget::FormatStatText(float CurrentValue, float MaxValue, const FString& Label) const
{
    return FText::FromString(
        FString::Printf(TEXT("%s: %.1f/%.1f"),
            *Label,
            CurrentValue,
            MaxValue
        )
    );
}
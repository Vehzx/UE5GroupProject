#include "YouWinWidget.h"
#include "Components/TextBlock.h"

void UYouWinWidget::SetYouWinStats(int32 GoldEarned, int32 GoldSpent, int32 EnemiesKilled)
{
    if (GoldEarnedText)
        GoldEarnedText->SetText(FText::FromString(FString::Printf(TEXT("Gold Earned: %d"), GoldEarned)));

    if (GoldSpentText)
        GoldSpentText->SetText(FText::FromString(FString::Printf(TEXT("Gold Spent: %d"), GoldSpent)));

    if (EnemiesKilledText)
        EnemiesKilledText->SetText(FText::FromString(FString::Printf(TEXT("Enemies Killed: %d"), EnemiesKilled)));
}
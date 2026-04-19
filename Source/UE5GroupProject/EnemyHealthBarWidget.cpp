#include "EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UEnemyHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UEnemyHealthBarWidget::SetHealth(float Current, float Max)
{
    // Update the progress bar if the binding succeeded.
    if (HealthBar)
    {
        HealthBar->SetPercent(Current / Max);
    }
}
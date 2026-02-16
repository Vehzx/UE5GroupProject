#include "EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UEnemyHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UEnemyHealthBarWidget::SetHealth(float Current, float Max)
{
    // Keep logs for now, testing purposes! -Luke
    UE_LOG(LogTemp, Warning, TEXT("SetHealth called: %f / %f"), Current, Max);

    // Update the progress bar if the binding succeeded.
    if (HealthBar)
    {
        HealthBar->SetPercent(Current / Max);
    }
}
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

UCLASS()
class UE5GROUPPROJECT_API UGameOverWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    void SetGameOverStats(int32 GoldEarned, int32 GoldSpent, int32 EnemiesKilled);

    // BindWidget text fields
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* GoldEarnedText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldSpentText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EnemiesKilledText;
};
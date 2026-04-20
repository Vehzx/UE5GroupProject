#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YouWinWidget.generated.h"

UCLASS()
class UE5GROUPPROJECT_API UYouWinWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetYouWinStats(int32 GoldEarned, int32 GoldSpent, int32 EnemiesKilled);

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* GoldEarnedText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldSpentText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EnemiesKilledText;
};
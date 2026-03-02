#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoldManager.generated.h"

UCLASS()
class UE5GROUPPROJECT_API AGoldManager : public AActor
{
    GENERATED_BODY()

public:
    AGoldManager();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gold")
    int32 CurrentGold = 200;

    UFUNCTION(BlueprintCallable, Category = "Gold")
    bool SpendGold(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Gold")
    void AddGold(int32 Amount);
};
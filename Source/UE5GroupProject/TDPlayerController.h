#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDPlayerController.generated.h"


class ATowerBase;

UCLASS()
class UE5GROUPPROJECT_API ATDPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ATDPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void PlayerTick(float DeltaTime) override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    // Set this in editor to choose which tower to place
    UPROPERTY(EditDefaultsOnly, Category = "Build")
    TSubclassOf<ATowerBase> TowerToBuild;

    // The preview tower actor
    UPROPERTY()
    ATowerBase* PreviewTower = nullptr;

    bool bIsPlacing = false;

    void StartBuildMode();
    void CancelBuildMode();
    void ConfirmPlaceTower();

    //placement settings
    UPROPERTY(EditDefaultsOnly, Category = "Build")
    float GridSize = 200.f;
};
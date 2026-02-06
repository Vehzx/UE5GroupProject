#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TowerBase.generated.h"

class ANPCBase;

UCLASS()
class UE5GROUPPROJECT_API ATowerBase : public AActor
{
    GENERATED_BODY()

public:
    ATowerBase();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
    float Range = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
    float FireRate = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
    float Damage = 10.f;

private:
    float TimeSinceLastShot = 0.f;

    UPROPERTY()
    ANPCBase* CurrentTarget;

    ANPCBase* FindTarget();
    void FireAtTarget();
};

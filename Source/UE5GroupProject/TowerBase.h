#pragma once

#include "CoreMinimal.h"
#include "GoldManager.h"
#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "TowerBase.generated.h"

class ANPCBase;

UCLASS()
class UE5GROUPPROJECT_API ATowerBase : public AActor
{
    GENERATED_BODY()

public:
    ATowerBase();

    UFUNCTION(BlueprintCallable, Category = "Build")
    void SetPreview(bool bPreview);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
    UStaticMeshComponent* TowerMesh;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
    USceneComponent* MuzzlePoint;

    // --- Tower Stats ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Stats")
    float Range = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Stats")
    float FireRate = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Stats")
    float TowerDamage = 10.f;

    UPROPERTY(EditAnywhere, Category = "Tower")
    TSubclassOf<AProjectile> ProjectileClass;

    // --- Upgrade Values (editable per tower) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    float FireRateUpgradeAmount = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    float RangeUpgradeAmount = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    float DamageUpgradeAmount = 5.f;

    // --- Upgrade Functions ---
    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void UpgradeFireRate(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void UpgradeRange(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void UpgradeDamage(float Amount);

    // --- Blueprint Wrappers ---
    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void ApplyFireRateUpgrade();

    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void ApplyRangeUpgrade();

    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void ApplyDamageUpgrade();

    // --- Upgrade Costs ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    int32 FireRateUpgradeCost = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    int32 RangeUpgradeCost = 40;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    int32 DamageUpgradeCost = 60;

    UPROPERTY()
    AGoldManager* GoldManager;
private:

    UPROPERTY()
    bool bIsPreview = false;

    float TimeSinceLastShot = 0.f;

    UPROPERTY()
    ANPCBase* CurrentTarget;

    ANPCBase* FindTarget();
    void FireAtTarget();
};
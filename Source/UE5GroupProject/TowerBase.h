#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "TowerBase.generated.h"

class ANPCBase;
class AGoldManager;
class UDecalComponent;

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

    UFUNCTION()
    void OnTowerClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
    USceneComponent* MuzzlePoint;

    // --- Range Indicator ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
    UDecalComponent* RangeDecal;

    UFUNCTION(BlueprintCallable, Category = "Tower")
    void UpdateRangeDecalSize();

    UFUNCTION(BlueprintCallable, Category = "Tower")
    void SetRangeVisible(bool bShow);

    // --- Tower Stats ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Stats")
    float Range = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Stats")
    float FireRate = 1.f; // seconds between shots

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Stats")
    float TowerDamage = 10.f;

    UPROPERTY(EditAnywhere, Category = "Tower")
    TSubclassOf<AProjectile> ProjectileClass;

    // --- Upgrade Values (per tower) ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    float FireRateUpgradeAmount = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    float RangeUpgradeAmount = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    float DamageUpgradeAmount = 5.f;

    // --- Upgrade Costs ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    int32 FireRateUpgradeCost = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    int32 RangeUpgradeCost = 40;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower|Upgrades")
    int32 DamageUpgradeCost = 60;

    // --- Raw stat upgrade functions ---
    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void UpgradeFireRate(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void UpgradeRange(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void UpgradeDamage(float Amount);

    // --- Gold-checked wrappers for UI / gameplay ---
    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void ApplyFireRateUpgrade();

    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void ApplyRangeUpgrade();

    UFUNCTION(BlueprintCallable, Category = "Tower|Upgrades")
    void ApplyDamageUpgrade();

private:
    UPROPERTY()
    bool bIsPreview = false;

    float TimeSinceLastShot = 0.f;

    UPROPERTY()
    ANPCBase* CurrentTarget = nullptr;

    UPROPERTY()
    AGoldManager* GoldManager = nullptr;

    ANPCBase* FindTarget();
    void FireAtTarget();
};
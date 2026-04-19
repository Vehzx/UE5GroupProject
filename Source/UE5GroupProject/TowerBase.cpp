#include "TowerBase.h"
#include "NPCBase.h"
#include "Projectile.h"
#include "GoldManager.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ATowerBase::ATowerBase()
{
    PrimaryActorTick.bCanEverTick = true;

    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
    RootComponent = TowerMesh;

    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(TowerMesh);

    TowerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorEnableCollision(false);

    // Always spawn tower actors at a scale of 45.
}

void ATowerBase::BeginPlay()
{
    Super::BeginPlay();

    // Find GoldManager by tag in the level
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("GoldManager"), Found);
    if (Found.Num() > 0)
    {
        GoldManager = Cast<AGoldManager>(Found[0]);
    }
}

void ATowerBase::SetPreview(bool bPreview)
{
    bIsPreview = bPreview;

    SetActorTickEnabled(!bIsPreview);
    SetActorEnableCollision(!bIsPreview);
}

void ATowerBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsPreview)
        return;

    TimeSinceLastShot += DeltaTime;

    // Drop target if it leaves range
    if (CurrentTarget)
    {
        float Dist = FVector::Dist(CurrentTarget->GetActorLocation(), GetActorLocation());
        if (Dist > Range)
        {
            CurrentTarget = nullptr;
        }
    }

    // Acquire new target if needed
    if (!IsValid(CurrentTarget) || CurrentTarget->IsActorBeingDestroyed())
    {
        CurrentTarget = FindTarget();
    }

    // Fire if valid target
    if (CurrentTarget && TimeSinceLastShot >= FireRate)
    {
        FireAtTarget();
        TimeSinceLastShot = 0.f;
    }
}

ANPCBase* ATowerBase::FindTarget()
{
    TArray<AActor*> FoundNPCs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPCBase::StaticClass(), FoundNPCs);

    ANPCBase* Closest = nullptr;
    float ClosestDist = Range;

    for (AActor* Actor : FoundNPCs)
    {
        const float Dist = FVector::Dist(Actor->GetActorLocation(), GetActorLocation());

        if (Dist <= Range && Dist < ClosestDist)
        {
            Closest = Cast<ANPCBase>(Actor);
            ClosestDist = Dist;
        }
    }

    return Closest;
}

void ATowerBase::FireAtTarget()
{
    if (!ProjectileClass || !CurrentTarget)
        return;

    const FVector SpawnLocation = MuzzlePoint
        ? MuzzlePoint->GetComponentLocation()
        : GetActorLocation();

    const FRotator SpawnRotation =
        (CurrentTarget->GetActorLocation() - SpawnLocation).Rotation();

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(
        ProjectileClass,
        SpawnLocation,
        SpawnRotation,
        Params
    );

    if (Proj)
    {
        Proj->SetOwner(this);
        Proj->Damage = TowerDamage;
    }
}

// --- Raw stat upgrade functions ---

void ATowerBase::UpgradeFireRate(float Amount)
{
    FireRate = FMath::Max(0.1f, FireRate - Amount);
}

void ATowerBase::UpgradeRange(float Amount)
{
    Range += Amount;
}

void ATowerBase::UpgradeDamage(float Amount)
{
    TowerDamage += Amount;
}

// --- Gold-checked wrappers ---

void ATowerBase::ApplyFireRateUpgrade()
{
    if (GoldManager && GoldManager->SpendGold(FireRateUpgradeCost))
    {
        UpgradeFireRate(FireRateUpgradeAmount);
    }
}

void ATowerBase::ApplyRangeUpgrade()
{
    if (GoldManager && GoldManager->SpendGold(RangeUpgradeCost))
    {
        UpgradeRange(RangeUpgradeAmount);
    }
}

void ATowerBase::ApplyDamageUpgrade()
{
    if (GoldManager && GoldManager->SpendGold(DamageUpgradeCost))
    {
        UpgradeDamage(DamageUpgradeAmount);
    }
}
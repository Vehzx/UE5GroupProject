#include "TowerBase.h"
#include "NPCBase.h"
#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ATowerBase::ATowerBase()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentTarget = nullptr;

    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
    RootComponent = TowerMesh;

    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(TowerMesh);
}

void ATowerBase::BeginPlay()
{
    Super::BeginPlay();

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
    UE_LOG(LogTemp, Warning, TEXT("Tower ticking"));
    Super::Tick(DeltaTime);

    if (bIsPreview) return;

    TimeSinceLastShot += DeltaTime;

    if (CurrentTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("CurrentTarget: %s"), *CurrentTarget->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CurrentTarget: NULL"));
    }

    UE_LOG(LogTemp, Warning, TEXT("TimeSinceLastShot: %f"), TimeSinceLastShot);

    if (!IsValid(CurrentTarget) || CurrentTarget->IsActorBeingDestroyed())
    {
        CurrentTarget = FindTarget();
    }

    if (CurrentTarget && TimeSinceLastShot >= FireRate)
    {
        UE_LOG(LogTemp, Warning, TEXT("FireAtTarget() SHOULD FIRE NOW"));
        FireAtTarget();
        TimeSinceLastShot = 0.f;
    }
}

ANPCBase* ATowerBase::FindTarget()
{
    TArray<AActor*> FoundNPCs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPCBase::StaticClass(), FoundNPCs);

    UE_LOG(LogTemp, Warning, TEXT("Tower found %d NPCs"), FoundNPCs.Num());

    ANPCBase* Closest = nullptr;
    float ClosestDist = Range;

    for (AActor* Actor : FoundNPCs)
    {
        float Dist = FVector::Dist(Actor->GetActorLocation(), GetActorLocation());
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
    if (ProjectileClass && CurrentTarget)
    {
        const FVector SpawnLocation = MuzzlePoint
            ? MuzzlePoint->GetComponentLocation()
            : GetActorLocation();

        const FRotator SpawnRotation =
            (CurrentTarget->GetActorLocation() - SpawnLocation).Rotation();

        AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(
            ProjectileClass,
            SpawnLocation,
            SpawnRotation
        );

        if (Proj)
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawned projectile (OnHit): %s"), *Proj->GetName());

            Proj->SetOwner(this);

            // Apply per-tower damage
            Proj->Damage = TowerDamage;

            UE_LOG(LogTemp, Warning, TEXT("Projectile damage set to: %f"), TowerDamage);
        }
    }
}

// --- Upgrade Functions ---
void ATowerBase::UpgradeFireRate(float Amount)
{
    FireRate = FMath::Max(0.1f, FireRate - Amount);
    UE_LOG(LogTemp, Warning, TEXT("Tower upgraded: FireRate now %f"), FireRate);
}

void ATowerBase::UpgradeRange(float Amount)
{
    Range += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Tower upgraded: Range now %f"), Range);
}

void ATowerBase::UpgradeDamage(float Amount)
{
    TowerDamage += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Tower upgraded: Damage now %f"), TowerDamage);
}

void ATowerBase::ApplyFireRateUpgrade()
{
    if (GoldManager && GoldManager->SpendGold(FireRateUpgradeCost))
    {
        UpgradeFireRate(FireRateUpgradeAmount);
        UE_LOG(LogTemp, Warning, TEXT("Fire rate upgraded!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough gold for fire rate upgrade."));
    }
}

void ATowerBase::ApplyRangeUpgrade()
{
    if (GoldManager && GoldManager->SpendGold(RangeUpgradeCost))
    {
        UpgradeRange(RangeUpgradeAmount);
        UE_LOG(LogTemp, Warning, TEXT("Range upgraded!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough gold for range upgrade."));
    }
}

void ATowerBase::ApplyDamageUpgrade()
{
    if (GoldManager && GoldManager->SpendGold(DamageUpgradeCost))
    {
        UpgradeDamage(DamageUpgradeAmount);
        UE_LOG(LogTemp, Warning, TEXT("Damage upgraded!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough gold for damage upgrade."));
    }
}
#include "TowerBase.h"
#include "NPCBase.h"
#include "Kismet/GameplayStatics.h"

ATowerBase::ATowerBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATowerBase::BeginPlay()
{
    Super::BeginPlay();
}

void ATowerBase::SetPreview(bool bPreview)
{
    bIsPreview = bPreview;

    // Preview towers should not shoot
    SetActorTickEnabled(!bIsPreview);

    SetActorEnableCollision(!bIsPreview);
}

void ATowerBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsPreview) return;

    TimeSinceLastShot += DeltaTime;

    if (!IsValid(CurrentTarget) || CurrentTarget->IsActorBeingDestroyed())
    {
        CurrentTarget = FindTarget();
    }

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
    if (CurrentTarget)
    {
        CurrentTarget->ApplyDamage(Damage);
    }
}

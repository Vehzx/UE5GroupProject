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
    UE_LOG(LogTemp, Warning, TEXT("Tower ticking"));
    Super::Tick(DeltaTime);

    if (bIsPreview) return;

    TimeSinceLastShot += DeltaTime;

    // Log current target status
    if (CurrentTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("CurrentTarget: %s"), *CurrentTarget->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CurrentTarget: NULL"));
    }

    // Log time since last shot
    UE_LOG(LogTemp, Warning, TEXT("TimeSinceLastShot: %f"), TimeSinceLastShot);

    // Acquire target if needed
    if (!IsValid(CurrentTarget) || CurrentTarget->IsActorBeingDestroyed())
    {
        CurrentTarget = FindTarget();
    }

    // Check firing conditions
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
        FVector SpawnLocation = GetActorLocation();
        FRotator SpawnRotation = (CurrentTarget->GetActorLocation() - SpawnLocation).Rotation();

        AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(
            ProjectileClass,
            SpawnLocation,
            SpawnRotation
        );

        if (Proj)
        {
            Proj->SetOwner(this);

            Proj->Collision->MoveIgnoreActors.Add(this);
        }
    }
}
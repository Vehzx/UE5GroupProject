#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "IDamageable.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;

    // Hit-based collision: block, generate hit events
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionObjectType(ECC_WorldDynamic);
    Collision->SetCollisionResponseToAllChannels(ECR_Block);
    Collision->SetNotifyRigidBodyCollision(true);          // enable hit events
    Collision->SetGenerateOverlapEvents(false);

    Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->InitialSpeed = Speed;
    Movement->MaxSpeed = Speed;
    Movement->bRotationFollowsVelocity = true;
    Movement->ProjectileGravityScale = 0.f;

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Projectile BeginPlay (OnHit mode)"));
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    UE_LOG(LogTemp, Warning, TEXT("Projectile OnHit: OtherActor=%s"),
        OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

    if (!OtherActor || OtherActor == GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit: Ignoring null or owner"));
        return;
    }

    if (IDamageable* Damageable = Cast<IDamageable>(OtherActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit: IDamageable, applying %f damage to %s"),
            Damage, *OtherActor->GetName());

        Damageable->ApplyDamage(Damage);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit: %s does NOT implement IDamageable"),
            *OtherActor->GetName());
    }

    Destroy();
}
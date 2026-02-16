#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "IDamageable.h"

AProjectile::AProjectile()
{
    // Collision (Root)
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;

    // Use QueryOnly so it never pushes NPCs
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionResponseToAllChannels(ECR_Overlap);

    // Still allow hit events
    Collision->SetNotifyRigidBodyCollision(true);
    Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

    // Movement
    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->InitialSpeed = Speed;
    Movement->MaxSpeed = Speed;
    Movement->bRotationFollowsVelocity = true;
    Movement->ProjectileGravityScale = 0.f;

    // Visual Mesh
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

    if (IDamageable* Damageable = Cast<IDamageable>(OtherActor))
    {
        Damageable->ApplyDamage(Damage);
    }

    Destroy();
}

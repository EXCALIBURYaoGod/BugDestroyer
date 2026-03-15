// Copyright @FpsLuping all reserved


#include "Weapons/Projectiles/ProjectileRocket.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


AProjectileRocket::AProjectileRocket()
{
	PrimaryActorTick.bCanEverTick = true;
	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileTrail"));
	TrailComponent->SetupAttachment(RootComponent);
	ProjectileFlyLoopComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ProjectileFlyLoopComponent"));
	ProjectileFlyLoopComponent->SetupAttachment(RootComponent);
	
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	AActor* Shooter = GetInstigator();
	if (Shooter && CollisionBox)
	{
		CollisionBox->IgnoreActorWhenMoving(Shooter, true);
	}
}




void AProjectileRocket::ApplyRadiaDamageFromImpactData(const FImpactEffectData* SelectedData)
{
	if (APawn* FiringPawn = GetInstigator())
	{
		if (AController* FiringController = FiringPawn->GetController())
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				SelectedData->ImpactDamage,
				MinimumDamage,
				GetActorLocation(),
				DamageInnerRadius,
				DamageOuterRadius,
				DamageFalloff,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				FiringController
			);
		}
	}
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	const FImpactEffectData* SelectedData = GetHitImpactDataByHitActorOwnTag(OtherActor);
	if (!bFakeProjectile) ApplyRadiaDamageFromImpactData(SelectedData);
	HandleFXDestruction();
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}


void AProjectileRocket::HandleFXDestruction()
{
	if (TrailComponent)
	{
		TrailComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		TrailComponent->Rename(nullptr, GetWorld());
		TrailComponent->Deactivate();
		TrailComponent->SetAutoDestroy(true);
	}
	if (ProjectileFlyLoopComponent)
	{
		ProjectileFlyLoopComponent->Stop();
	}
}

void AProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


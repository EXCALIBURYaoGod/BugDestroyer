// Copyright @FpsLuping all reserved


#include "Weapons/Projectiles/ProjectileGrenade.h"

#include "GameplayTagAssetInterface.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileGrenade::AProjectileGrenade()
{
	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileTrail"));
	TrailComponent->SetupAttachment(RootComponent);
	
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.6f;  
	ProjectileMovementComponent->Friction = 0.2f;   
	ProjectileMovementComponent->BounceVelocityStopSimulatingThreshold = 5.0f; 
	
}


void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();
	AActor* Shooter = GetInstigator();
	if (Shooter && CollisionBox)
	{
		CollisionBox->IgnoreActorWhenMoving(Shooter, true);
		ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnHandleBounce);
		if (AActor* MyOwner = GetOwner())
		{
			CollisionBox->IgnoreActorWhenMoving(MyOwner, true);
		}
	}
	SetLifeSpan(GrenadeTriggerTime);
}

void AProjectileGrenade::Destroyed()
{
	const FImpactEffectData* SelectedData = &DefaultImpactData;
	
	if (SelectedData->Particles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedData->Particles, GetActorLocation(), GetActorRotation());
	}
	if (SelectedData->Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SelectedData->Sound, GetActorLocation());
	}
	
	if (HasAuthority())
	{
		if (!bFakeProjectile)
		{
			TArray<AActor*> IgnoreActors;
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,                        // WorldContextObject
				BaseDamage,                  // 内圈全额伤害
				MinimumDamage,               // 边缘最小伤害
				GetActorLocation(),          // 爆炸中心点 (手雷当前位置)
				DamageInnerRadius,           // 内圈半径
				DamageOuterRadius,           // 外圈半径
				DamageFalloff,               // 衰减指数
				UDamageType::StaticClass(),  // 伤害类型 (可自定义，用于区分爆炸/火焰/毒气伤害)
				IgnoreActors,                // 忽略列表
				this,                        // 伤害制造者 (DamageCauser)
				GetInstigatorController(),   // 施加伤害的控制器 (InstigatorController，极重要！)
				ECC_Visibility               // 碰撞通道 (如果是 ECC_Visibility，躲在掩体后的怪物可以免伤)
			);
		}
	}
	
	Super::Destroyed();
}

void AProjectileGrenade::OnHandleBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound)
	{
		float Speed = ImpactVelocity.Size();
		if (Speed > 100.f)
		{
			UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
		}
	}
}


// Called every frame
void AProjectileGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


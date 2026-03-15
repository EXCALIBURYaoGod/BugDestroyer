// //Copyrights @FpsLuping all reserved


#include "Weapons/Projectiles/Projectile.h"

#include "GameplayTagAssetInterface.h"
#include "BugDestroyer/BugDestroyer.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SKM, ECR_Block);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	StaticMesh->SetupAttachment(CollisionBox);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (Tracer)
		{
			TracerComponent = UGameplayStatics::SpawnEmitterAttached(
					Tracer,
					CollisionBox,
					FName(),
					GetActorLocation(),
					GetActorRotation(),
					EAttachLocation::KeepWorldPosition
				);
		}
	}

	if (AActor* MyOwner = GetOwner())
	{
		CollisionBox->IgnoreActorWhenMoving(MyOwner, true);
	}

	CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	const FImpactEffectData* SelectedData = GetHitImpactDataByHitActorOwnTag(OtherActor);
	
	if (GetNetMode() != NM_DedicatedServer) 
	{
		bool bIsLocalShooter = false;
		if (APawn* InstigatorPawn = GetInstigator())
		{
			bIsLocalShooter = InstigatorPawn->IsLocallyControlled();
		}
		
		if (bFakeProjectile || (!bFakeProjectile && !bIsLocalShooter) || HasAuthority())
		{
			if (SelectedData->Particles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedData->Particles, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
    
			if (SelectedData->Sound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, SelectedData->Sound, Hit.ImpactPoint);
			}
		}
	}
	
	if (bFakeProjectile)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMesh->SetVisibility(false);
		SetLifeSpan(0.1f);
	}
	else if (HasAuthority()) 
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMesh->SetVisibility(false);
		SetLifeSpan(0.1f); 
	}
}

// 隐藏开枪者的服务器同步子弹
bool AProjectile::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	
	if (GetInstigator() && GetInstigator()->GetController() == RealViewer)
	{
		return false;
	}

	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

const FImpactEffectData* AProjectile::GetHitImpactDataByHitActorOwnTag(AActor* OtherActor)
{

	const FImpactEffectData* SelectedData = &DefaultImpactData;
	FGameplayTagContainer TargetTags;
	if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(OtherActor))
	{
		TagInterface->GetOwnedGameplayTags(TargetTags);
	
		for (const auto& Pair : TaggedImpactEffects)
		{
			if (TargetTags.HasTag(Pair.Key))
			{
				SelectedData = &Pair.Value;
				break;
			}
		}
	}
	return SelectedData;
	
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}





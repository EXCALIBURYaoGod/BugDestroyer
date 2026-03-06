// //Copyrights @FpsLuping all reserved


#include "Weapons/Projectile.h"

#include "DebugHelper.h"
#include "GameplayTagAssetInterface.h"
#include "BugDestroyer/BugDestroyer.h"
#include "Character/BugCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


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


	CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	
	
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
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
	
	if (SelectedData->Particles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedData->Particles, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}
    
	if (SelectedData->Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SelectedData->Sound, Hit.ImpactPoint);
	}
	
	if (HasAuthority())
	{
		//Destroy();
		SetLifeSpan(0.1f);
	}
	
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}





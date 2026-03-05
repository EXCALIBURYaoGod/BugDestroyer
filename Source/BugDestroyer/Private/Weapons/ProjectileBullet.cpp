// Copyright @FpsLuping all reserved


#include "Weapons/ProjectileBullet.h"

#include "DebugHelper.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectileBullet::AProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	/*if (HasAuthority())
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
        	AController* Controller = OwnerCharacter->GetController();
        	if (Controller)
        	{
        		FGameplayTag BulletDamageTag = FGameplayTag::RequestGameplayTag(FName("Gameplay.Damage.Bullet"));
        		UGameplayStatics::ApplyDamage(OtherActor, GetImpactDamageFromTag(BulletDamageTag), Controller, this, UDamageType::StaticClass());
        	}
        }
	}*/
	
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}


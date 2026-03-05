// Copyright @FpsLuping all reserved


#include "Weapons/Casing.h"

#include "Kismet/GameplayStatics.h"


ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("CasingMesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 10.f;
	InitialLifeSpan = 3.f;
	
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	StaticMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	StaticMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
	
}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		StaticMesh->SetNotifyRigidBodyCollision(false);
	}
	
}

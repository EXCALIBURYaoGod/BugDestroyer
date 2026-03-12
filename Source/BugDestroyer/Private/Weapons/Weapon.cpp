// //Copyrights @FpsLuping all reserved


#include "Weapons/Weapon.h"

#include "DebugHelper.h"
#include "BugDestroyer/BugDestroyer.h"
#include "Character/BugCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Weapons/Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
	PickupWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupWidget->SetGenerateOverlapEvents(false);
	
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnSphereEndOverlap);
		SetWeaponState(EWeaponState::EWS_Dropped);
	}
	CreateMIDs();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, CurrentAmmo);
	
}

void AWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if (NewOwner == nullptr)
	{
		CachedOwningBugCharacterForEquip = nullptr;
	}
	
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ABugCharacter* BugCharacter = Cast<ABugCharacter>(OtherActor);
        if (BugCharacter && PickupWidget)
        {
        	BugCharacter->SetOverlappingWeapon(this, true);
        }
	}

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABugCharacter* BugCharacter = Cast<ABugCharacter>(OtherActor);
	if (BugCharacter && PickupWidget && BugCharacter->IsLocallyControlled())
	{
		ShowPickupWidget(false);
	}
	if (HasAuthority())
	{
		if (BugCharacter && PickupWidget)
		{
			BugCharacter->SetOverlappingWeapon(this, false);
		}
	}
	
}

void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::OnRep_CurrentAmmo()
{
	if (CachedOwningBugCharacterForEquip)
	{
		int32 AmmoLeft = CachedOwningBugCharacterForEquip->GetAmmoLeft();
		OnAmmoChanged.Broadcast(CurrentAmmo, MagCapacity, AmmoLeft);
	}
}

void AWeapon::AttachToCharacter(const FName& SocketName)
{
	if (ABugCharacter* BugCharacter = Cast<ABugCharacter>(GetOwner()))
	{
		CachedOwningBugCharacterForEquip = BugCharacter;
		USkeletalMeshComponent* CharacterMesh = CachedOwningBugCharacterForEquip->GetMesh();
        
		if (CharacterMesh)
		{
			FAttachmentTransformRules AttachmentRules(
				EAttachmentRule::SnapToTarget, // Location
				EAttachmentRule::SnapToTarget, // Rotation
				EAttachmentRule::SnapToTarget, // Scale
				false                          // bWeldSimulatedBodies
			);
			this->AttachToComponent(CharacterMesh, AttachmentRules, SocketName);
		}
		CachedOwningBugCharacterForEquip->OnWeaponChanged.Broadcast(this);
	}
}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_EquippedPrimary:
		if (GetEquipSound())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				GetEquipSound(),
				GetActorLocation()
			);
		}
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttachToCharacter(TEXT("RightHandSocket"));
		if (WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
		break;
	case EWeaponState::EWS_EquippedSecondary:
		if (GetEquipSound())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				GetEquipSound(),
				GetActorLocation()
			);
		}
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->PutRigidBodyToSleep();
		AttachToCharacter(TEXT("BackpackSocket"));
		if (WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
		break;
	case EWeaponState::EWS_Dropped:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetGenerateOverlapEvents(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECC_SKM, ECR_Ignore);
		break;
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_MAX:
		break;
	}
}

void AWeapon::SpendRoundAmmo()
{
	if (CurrentAmmo <= 0) return;
	--CurrentAmmo;
	CurrentAmmo = FMath::Max(0, CurrentAmmo);
	if (CachedOwningBugCharacterForEquip)
	{
		int32 AmmoLeft = CachedOwningBugCharacterForEquip->GetAmmoLeft();
		OnAmmoChanged.Broadcast(CurrentAmmo, MagCapacity, AmmoLeft);
	}
}

void AWeapon::CreateMIDs()
{
	const int32 NumMaterials = GetWeaponMesh() ? GetWeaponMesh()->GetNumMaterials() : 0;
	DynamicMeshMID.Empty();
	for (int32 i = 0; i < NumMaterials; ++i)
	{
		UMaterialInstanceDynamic* MID = GetWeaponMesh()->CreateDynamicMaterialInstance(i);
		if (MID)
		{
			DynamicMeshMID.Add(MID);
		}
	}
}

void AWeapon::UpdateWeaponDither(float Alpha)
{
	for (UMaterialInstanceDynamic* MID : DynamicMeshMID)
	{
		MID->SetScalarParameterValue(FName("GhostAlpha"), Alpha);
	}
}

void AWeapon::PlayReloadAnimation()
{
	if (ReloadAnimation && WeaponMesh)
	{
		WeaponMesh->PlayAnimation(ReloadAnimation, false);
	}
}


void AWeapon::ShowPickupWidget(bool bShowWidget, ABugCharacter* BugCharacter)
{
	if (PickupWidget)
	{
		if (BugCharacter)
		{
			APlayerController* PC = Cast<APlayerController>(BugCharacter->GetController());
			if (PC && PC->PlayerCameraManager)
			{
				FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
				FVector WidgetLocation = PickupWidget->GetComponentLocation();
				FRotator LookAtRot = FRotationMatrix::MakeFromX(CameraLocation - WidgetLocation).Rotator();
				PickupWidget->SetWorldRotation(LookAtRot);
			}
		}
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation && WeaponMesh)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass)
	{
		const USkeletalMeshSocket* ShellEjectSocket = GetWeaponMesh()->GetSocketByName(FName("ShellEject"));
		FTransform SocketTransform = ShellEjectSocket->GetSocketTransform(GetWeaponMesh());
		FActorSpawnParameters SpawnParams;
		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator(),
					SpawnParams
				);
		}
	}
	SpendRoundAmmo();
	
}

void AWeapon::DropWeapon(const FVector& Direction)
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	
	if (CachedOwningBugCharacterForEquip)
	{
		CachedOwningBugCharacterForEquip->OnWeaponChanged.Broadcast(nullptr);
	}
	SetOwner(nullptr);
	CachedOwningBugCharacterForEquip = nullptr;
	
	if (WeaponMesh && WeaponMesh->IsSimulatingPhysics())
	{
		FVector Impulse = Direction * 300.f;
		WeaponMesh->AddImpulse(Impulse, NAME_None, true);
	}
	
}



// Copyrights @FpsLuping all reserved


#include "Components/CombatComponent.h"

#include "DebugHelper.h"
#include "Camera/CameraComponent.h"
#include "Character/BugCharacter.h"
#include "Controllers/GameCommonPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/BugHud.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Weapon.h"

#define TRACE_LENGTH 80000.f


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, HitTarget);
	DOREPLIFETIME(UCombatComponent, AmmoLeft);
	DOREPLIFETIME(UCombatComponent, CombatState);
	
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (BugCharacter)
	{
		UCameraComponent* CameraComponent = BugCharacter->GetFollowCamera();
		if (CameraComponent)
		{
			DefaultFOV = CameraComponent->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		BugCharacter->OnWeaponChanged.AddUniqueDynamic(this, &ThisClass::OnWeaponChangedCallBack);
	}
	InitializeCarriedAmmoMap();
}

void UCombatComponent::PushCrosshair(float DeltaTime)
{
	if (!EquippedWeapon)
	{
		if (BugHud) BugHud->SetbDrawCrosshair(false);
		return;
	}
	InitializePlayerController();
	InitializeHUD();
	if (PlayerController)
	{
		if (BugHud)
		{
			float VelocityRange = 0.f;
			if (BugCharacter)
			{
				FVector Velocity = BugCharacter->GetVelocity();
				Velocity.Z = 0.f;
				VelocityRange = Velocity.Size();
			}
			float VelocitySpread = FMath::GetMappedRangeValueClamped(
				FVector2D(0.f, 600.f), 
				FVector2D(0.f, 20.f), 
				VelocityRange
			);
			float TargetSpread = (bFiring ? EquippedWeapon->GetFireCrosshairSpread() : 0.0f) 
				+ (bAiming ? EquippedWeapon->GetAimCrosshairSpread() : 0.0f) + VelocitySpread;
			BugHud->SetCrosshairCurrentSpread(FMath::FInterpTo(BugHud->GetCrosshairCurrentSpread(), TargetSpread, DeltaTime, 10.f));
			BugHud->SetbDrawCrosshair(EquippedWeapon != nullptr);
		}
	}
	
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	InitializePlayerController();
	InitializeHUD();
	
	if (BugCharacter && BugCharacter->IsLocallyControlled())
	{
		PushCrosshair(DeltaTime);
		FHitResult HitResult;
        TraceUnderCrosshairs(HitResult);
        RPC_SetHitTarget(HitResult.ImpactPoint);
		
		InterpFOV(DeltaTime);
	}

}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	RPC_ServerSetAiming(bIsAiming);
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && BugCharacter)
	{
		BugCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		BugCharacter->bUseControllerRotationYaw = true;
		BugCharacter->PlayEquipMontage();
	}
}

void UCombatComponent::ExecuteFire(bool InIsFire)
{
	bFireButtonPressed = InIsFire;
	if (CanFire())
	{
		bFiring = true;
		Fire();
	}
	else
	{
		bFiring = false;
	}

}



void UCombatComponent::Fire()
{
	bCanFire = false;
	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);
	RPC_ServerStartFire(HitResult.ImpactPoint);
	StartFireTimer();
}

void UCombatComponent::StartFireTimer()
{
	bCanFire = true;
	if (!EquippedWeapon) return;
	bFiring = true;
	BugCharacter->GetWorldTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&UCombatComponent::FireTimerFinished,
			EquippedWeapon->GetFireDelay()
		);

}

void UCombatComponent::FireTimerFinished()
{
	bFiring = false;
	if (EquippedWeapon && EquippedWeapon->IsAutomatic() && CanFire())
	{
		Fire();
	}
}

void UCombatComponent::RPC_SetHitTarget_Implementation(const FVector_NetQuantize& ClientHitTarget)
{
	HitTarget = ClientHitTarget;
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& HitResult)
{

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairLocation,
			CrosshairWorldPosition,
			CrosshairWorldDirection
		);
	if (bScreenToWorld)
	{

		FVector Start = CrosshairWorldPosition;
		if (BugCharacter)
		{
			float DistanceToCharacter = (BugCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 95.f); // 解决准心锁到本地玩家自身问题
		}	
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);

		if (!HitResult.bBlockingHit)
		{
			HitResult.ImpactPoint = End;
		}
		else
		{
			if (HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
			{
				if (BugHud)
				{
					BugHud->SetCrosshairColor(FLinearColor::Red);
				}
				
			}
			else
			{
				if (BugHud)
				{
					BugHud->SetCrosshairColor(BugHud->GetDefaultConfig().Color);
				}
			}
		}
	}
	
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (!EquippedWeapon) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomedInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, EquippedWeapon->GetZoomedInterpSpeed());
	}
	if (BugCharacter && BugCharacter->GetFollowCamera())
	{
		BugCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
	
}

void UCombatComponent::InitializePlayerController()
{
	if (PlayerController) return;
	if (BugCharacter && BugCharacter->GetController())
	{
		PlayerController = Cast<AGameCommonPlayerController>(BugCharacter->GetController());
	}
}

void UCombatComponent::InitializeHUD()
{
	if (BugHud) return;
	if (PlayerController)
	{
		BugHud = Cast<ABugHud>(PlayerController->GetHUD());
	}
}

void UCombatComponent::OnWeaponChangedCallBack(class AWeapon* NewWeapon)
{
	EquippedWeapon = NewWeapon;
}

void UCombatComponent::OnRep_AmmoLeft()
{
	BugCharacter->OnAmmoLeftChanged.Broadcast(EquippedWeapon->GetCurrentAmmo(), EquippedWeapon->GetMagCapacity(), AmmoLeft);
}

void UCombatComponent::InitializeCarriedAmmoMap()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, 60);
}

bool UCombatComponent::CanFire()
{
	if (!bFireButtonPressed) return false;
	if (!EquippedWeapon) return false;
	if (EquippedWeapon->IsAmmoEmpty()) return false;
	if (!bCanFire) return false;
	if (!BugCharacter || !BugCharacter->IsUnoccupied()) return false;
	return true;
}

void UCombatComponent::RPC_ServerStartFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastRPC_StartFire(TraceHitTarget);
}

void UCombatComponent::MulticastRPC_StartFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (!EquippedWeapon) return;
	if (BugCharacter)
	{
		EquippedWeapon->Fire(TraceHitTarget);
		BugCharacter->PlayFireMontage();
	}
}

void UCombatComponent::RPC_ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

// only server calls
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (BugCharacter == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}
	if (EquippedWeapon)
	{
		EquippedWeapon->DropWeapon();
	}
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetOwner(BugCharacter);
	if (EquippedWeapon)
	{
		if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		{
			AmmoLeft = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
			BugCharacter->OnAmmoLeftChanged.Broadcast(EquippedWeapon->GetCurrentAmmo(), EquippedWeapon->GetMagCapacity(), AmmoLeft);
		}
	}
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	BugCharacter->PlayEquipMontage();
	BugCharacter->SetCombatState(ECombatState::ECS_Equipping);
	if (EquippedWeapon->GetEquipSound())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->GetEquipSound(),
			BugCharacter->GetActorLocation()
		);
	}
	BugCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	BugCharacter->bUseControllerRotationYaw = true;
	
	
}

void UCombatComponent::OnRep_CombatState()
{
	OnCombatStateChanged();
}

void UCombatComponent::OnCombatStateChanged()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	case ECombatState::ECS_Equipping:
		if (EquippedWeapon->GetEquipSound())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				EquippedWeapon->GetEquipSound(),
				BugCharacter->GetActorLocation()
			);
		}
		break;
	case ECombatState::ECS_Max:
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			ExecuteFire(true);
		}
		break;
	}
}

void UCombatComponent::Reload()
{
	if (EquippedWeapon && EquippedWeapon->GetCurrentAmmo() != EquippedWeapon->GetMagCapacity() && AmmoLeft > 0 && CombatState == ECombatState::ECS_Unoccupied)
	{
		RPC_Reload();
	}
}

void UCombatComponent::RPC_Reload_Implementation()
{
	if (EquippedWeapon && EquippedWeapon->GetCurrentAmmo() != EquippedWeapon->GetMagCapacity() && AmmoLeft > 0 && CombatState == ECombatState::ECS_Unoccupied)
	{
		CombatState = ECombatState::ECS_Reloading;
		MulticastRPC_ReloadStarted();
	}
}

void UCombatComponent::MulticastRPC_ReloadStarted_Implementation()
{
	if (!EquippedWeapon) return;
	if (BugCharacter)
	{
		BugCharacter->PlayReloadMontage();
		CombatState = ECombatState::ECS_Reloading;
		
		if (EquippedWeapon->GetReloadSound())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				EquippedWeapon->GetReloadSound(),
				BugCharacter->GetActorLocation()
			);
		}
	}
}

void UCombatComponent::MulticastRPC_ReloadCompleted_Implementation()
{
	if (BugCharacter)
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::OnReloadAnimationFinished()
{
	if (BugCharacter)
	{
		if (BugCharacter->HasAuthority() && EquippedWeapon)
		{
			// 服务器端直接处理Reload逻辑
			HandleReload();
			MulticastRPC_ReloadCompleted();
		}
		else if (!BugCharacter->HasAuthority())
		{
			// 客户端将动画结束通知发送到服务器
			RPC_ReloadAnimationFinished();
		}
	}
}

void UCombatComponent::RPC_ReloadAnimationFinished_Implementation()
{
	// 服务器端处理客户端发送的动画结束通知
	if (EquippedWeapon)
	{
		HandleReload();
		MulticastRPC_ReloadCompleted();
	}
}

void UCombatComponent::HandleReload()
{
	if (BugCharacter)
	{
		if (BugCharacter->HasAuthority() && EquippedWeapon)
		{
			int32 MagCapacity = EquippedWeapon->GetMagCapacity();
			int32 CurrentAmmo = EquippedWeapon->GetCurrentAmmo();
			int32 NeedToFill = MagCapacity - CurrentAmmo;
			if (NeedToFill <= 0) return;
			if (AmmoLeft >= NeedToFill)
			{
				EquippedWeapon->SetCurrentAmmo(MagCapacity);
				AmmoLeft -= NeedToFill;
			}
			else
			{
				EquippedWeapon->SetCurrentAmmo(CurrentAmmo + AmmoLeft);
				AmmoLeft = 0;
			}
			EquippedWeapon->OnAmmoChanged.Broadcast(EquippedWeapon->GetCurrentAmmo(), EquippedWeapon->GetMagCapacity(), AmmoLeft);
			BugCharacter->OnAmmoLeftChanged.Broadcast(EquippedWeapon->GetCurrentAmmo(), EquippedWeapon->GetMagCapacity(), AmmoLeft);
		}
	}
}



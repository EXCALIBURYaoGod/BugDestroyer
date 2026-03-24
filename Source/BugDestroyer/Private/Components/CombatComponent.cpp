// Copyrights @FpsLuping all reserved


#include "Components/CombatComponent.h"

#include "Camera/CameraComponent.h"
#include "Character/BugCharacter.h"
#include "Components/SphereComponent.h"
#include "Controllers/GameCommonPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/BugHud.h"
#include "Weapons/Projectiles/ProjectileGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Settings/BugGameUserSettings.h"
#include "Weapons/HitScanWeapon.h"
#include "Weapons/Weapon.h"

#define TRACE_LENGTH 80000.f


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, PrimaryWeapon);
	DOREPLIFETIME_CONDITION(UCombatComponent, bAiming, COND_SkipOwner);
	DOREPLIFETIME(UCombatComponent, HitTarget);
	DOREPLIFETIME(UCombatComponent, AmmoLeft);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, GrenadeAmount);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
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
	}
	InitializeCarriedAmmoMap();
}

void UCombatComponent::PushCrosshair(float DeltaTime)
{
	if (!PrimaryWeapon)
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
			if (PlayerController->IsLocalPlayerController())
			{
                float TargetSpread = (bFiring ? PrimaryWeapon->GetFireCrosshairSpread() : 0.0f) 
                	+ (bAiming || BugCharacter->bIsCrouched ? PrimaryWeapon->GetAimCrosshairSpread() : 0.0f) + VelocitySpread;
                BugHud->SetCrosshairCurrentSpread(FMath::FInterpTo(BugHud->GetCrosshairCurrentSpread(), TargetSpread, DeltaTime, 10.f));
                BugHud->SetbDrawCrosshair(PrimaryWeapon != nullptr);
			}

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
		LocalHitTarget = HitResult.ImpactPoint;
		RPC_SetHitTarget(HitResult.ImpactPoint);
		InterpFOV(DeltaTime);
		RecoverRecoil(DeltaTime);
	}

}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	
	// Adjust fire sphere radius for hit scan weapons when aiming
	if (AHitScanWeapon* HitScanWeapon = Cast<AHitScanWeapon>(PrimaryWeapon))
	{
		const float AdjustedRadius = FMath::Max(0.f, HitScanWeapon->GetDefaultFireSphereRadius() - (bIsAiming ? 20.f : 0.f));
		HitScanWeapon->SetFireSphereRadius(AdjustedRadius);
	}
	
	if (BugCharacter && BugCharacter->IsLocallyControlled() && PrimaryWeapon && 
		PrimaryWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		if (AGameCommonPlayerController* PC = Cast<AGameCommonPlayerController>(BugCharacter->GetController()))
		{
			PC->ShowSniperScopeWidget(bIsAiming);
		}
	}
	
	RPC_ServerSetAiming(bIsAiming);
}

void UCombatComponent::OnRep_PrimaryWeapon()
{
	if (PrimaryWeapon && BugCharacter)
	{
		BugCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		BugCharacter->bUseControllerRotationYaw = true;
		if (BugCharacter->IsLocallyControlled())
		{
			BugCharacter->OnAmmoLeftChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
		}
		if (BugCharacter && !BugCharacter->IsLocallyControlled())
		{
			BugCharacter->PlayEquipMontage(PrimaryWeapon);
		}
	}
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (SecondaryWeapon && BugCharacter)
	{
		BugCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		BugCharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::TossGrenade()
{
	if (GrenadeAmount <= 0)
	{
		return;
	}
	if (PrimaryWeapon)
	{
		RPC_TossGrenade();
	}

}

void UCombatComponent::LaunchGrenade()
{
	ShowAttachedGrenade(false);
	if (BugCharacter && BugCharacter->GetAttachedGrenade())
	{
		if (FakeGrenadeClass)
		{
			const FVector StartingLocation = BugCharacter->GetAttachedGrenade()->GetComponentLocation();
			FVector ToTarget = HitTarget - StartingLocation;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = BugCharacter;
			SpawnParams.Instigator = BugCharacter;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectileGrenade>(
					FakeGrenadeClass, 
					StartingLocation, 
					ToTarget.Rotation(),
					SpawnParams
					);
			}
		}
	}
	if (BugCharacter && BugCharacter->HasAuthority() && BugCharacter->GetAttachedGrenade())
	{
		if (GrenadeClass)
		{
			const FVector StartingLocation = BugCharacter->GetAttachedGrenade()->GetComponentLocation();
			FVector ToTarget = HitTarget - StartingLocation;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = BugCharacter;
			SpawnParams.Instigator = BugCharacter;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectileGrenade>(
					GrenadeClass, 
					StartingLocation, 
					ToTarget.Rotation(),
					SpawnParams
					);
			}
		}
		if (GrenadeAmount > 0)
		{
			GrenadeAmount --;
			BugCharacter->OnGrenadeAmountChanged.Broadcast(GrenadeAmount);
		}
	}
}

void UCombatComponent::ShowAttachedGrenade(bool bShowGrenade)
{
	if (BugCharacter && BugCharacter->GetAttachedGrenade())
	{
		BugCharacter->GetAttachedGrenade()->SetVisibility(bShowGrenade);
	}
}

void UCombatComponent::RPC_TossGrenade_Implementation()
{
	CombatState = ECombatState::ECS_TossingGrenade;
	if (BugCharacter)
	{
		BugCharacter->PlayGrenadeTossMontage();
	}
	ShowAttachedGrenade(true);
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
	if (!PrimaryWeapon) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, PrimaryWeapon->GetZoomedFOV(), DeltaTime, PrimaryWeapon->GetZoomedInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, PrimaryWeapon->GetZoomedInterpSpeed());
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

void UCombatComponent::OnRep_AmmoLeft()
{
	if (BugCharacter)
	{
		BugCharacter->OnAmmoLeftChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
	}
	
}

void UCombatComponent::InitializeCarriedAmmoMap()
{
	if (CarriedAmmoMap.IsEmpty())
	{
		CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, 60);
		CarriedAmmoMap.Emplace(EWeaponType::EWT_PlasmaPistol, 30);
		CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, 4);
		CarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, 80);
		CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, 24);
		CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, 10);
	}

}

void UCombatComponent::OnRep_GrenadeAmount()
{
	if (BugCharacter)
	{
		BugCharacter->OnGrenadeAmountChanged.Broadcast(GrenadeAmount);
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

void UCombatComponent::ApplyRecoil()
{
	if (!BugCharacter || !BugCharacter->IsLocallyControlled() || !PrimaryWeapon) return;
	if (PlayerController)
	{
		FWeaponRecoilData RecoilData = PrimaryWeapon->GetRecoilData();
		
		float PitchKick = FMath::RandRange(RecoilData.VerticalRecoilMin, RecoilData.VerticalRecoilMax);
		float YawKick = FMath::RandRange(RecoilData.HorizontalRecoilMin, RecoilData.HorizontalRecoilMax);

		PlayerController->AddPitchInput(-PitchKick);
		PlayerController->AddYawInput(YawKick);
		
		CurrentRecoilOffset.X += PitchKick;
		CurrentRecoilOffset.Y += YawKick;
		
		CurrentRecoveryRate = RecoilData.RecoilRecoveryRate;
	}
}

void UCombatComponent::RecoverRecoil(float DeltaTime)
{
	if (bFiring || CurrentRecoilOffset.IsNearlyZero(0.01f)) return;

	if (PlayerController)
	{
		float NewPitchOffset = FMath::FInterpTo(CurrentRecoilOffset.X, 0.0f, DeltaTime, CurrentRecoveryRate);
		float NewYawOffset = FMath::FInterpTo(CurrentRecoilOffset.Y, 0.0f, DeltaTime, CurrentRecoveryRate);

		float PitchRecoveryDelta = CurrentRecoilOffset.X - NewPitchOffset;
		float YawRecoveryDelta = CurrentRecoilOffset.Y - NewYawOffset;
		
		PlayerController->AddPitchInput(PitchRecoveryDelta);
		PlayerController->AddYawInput(-YawRecoveryDelta);

		CurrentRecoilOffset.X = NewPitchOffset;
		CurrentRecoilOffset.Y = NewYawOffset;
	}
}

void UCombatComponent::LocalPlayFireFX(const FVector& InHitTarget, int32 InRandomSeed)
{
	if (PrimaryWeapon && BugCharacter)
	{
		PrimaryWeapon->SimulateFireFX(InHitTarget, InRandomSeed);
		BugCharacter->PlayFireMontage();
		if (PrimaryWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
		{
			if (BoltActionDelay < PrimaryWeapon->GetFireDelay())
			{
				if (UWorld* World = GetWorld())
				{
					World->GetTimerManager().SetTimer(
						BoltTimerHandle,
						this,
						&ThisClass::PlayBoltAction,
						BoltActionDelay,
						false
					);
				}
			}
		}
	}
}

void UCombatComponent::Fire()
{
	bCanFire = false;
	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);
	RandomSeed = FMath::Rand();
	if (PrimaryWeapon)
	{
		PrimaryWeapon->ClientSpendRoundAmmo();
	}
	ApplyRecoil();
	LocalPlayFireFX(HitResult.ImpactPoint, RandomSeed); 
	RPC_ServerStartFire(HitResult.ImpactPoint, RandomSeed);
	StartFireTimer();
}

void UCombatComponent::StartFireTimer()
{
	if (!PrimaryWeapon) return;
	bFiring = true;
	BugCharacter->GetWorldTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&UCombatComponent::FireTimerFinished,
			PrimaryWeapon->GetFireDelay()
		);

}

void UCombatComponent::FireTimerFinished()
{
	bFiring = false;
	bCanFire = true;
	if (PrimaryWeapon && PrimaryWeapon->IsAutomatic() && CanFire())
	{
		Fire();
	}
}

bool UCombatComponent::CanFire()
{
	if (!bFireButtonPressed) return false;
	if (!PrimaryWeapon) return false;
	if (PrimaryWeapon->IsClientAmmoEmpty() || PrimaryWeapon->IsAmmoEmpty()) return false;
	if (!bCanFire) return false;
	if (!BugCharacter || !BugCharacter->IsUnoccupied()) return false;
	return true;
}

void UCombatComponent::RPC_ServerStartFire_Implementation(const FVector_NetQuantize& TraceHitTarget, int32 InRandomSeed)
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->ServerExecuteFireLogic(TraceHitTarget, InRandomSeed);
	}
    
	// 广播给除发起者之外的其他客户端播特效
	MulticastRPC_SimulateFireFX(TraceHitTarget, InRandomSeed);
}

void UCombatComponent::MulticastRPC_SimulateFireFX_Implementation(const FVector_NetQuantize& TraceHitTarget, int32 InRandomSeed)
{
	if (!PrimaryWeapon) return;
	if (BugCharacter && !BugCharacter->IsLocallyControlled())
	{
		LocalPlayFireFX(TraceHitTarget, InRandomSeed);
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
	if (PrimaryWeapon != nullptr && SecondaryWeapon == nullptr)
	{
		EquipSecondaryWeapon(WeaponToEquip);
	}
	else
	{
		EquipPrimaryWeapon(WeaponToEquip);
	}
	BugCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	BugCharacter->bUseControllerRotationYaw = true;
	
	
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (PrimaryWeapon != nullptr)
	{
		PrimaryWeapon->DropWeapon();
	}
	PrimaryWeapon = WeaponToEquip;
	PrimaryWeapon->SetOwner(BugCharacter);
	if (PrimaryWeapon)
	{
		if (CarriedAmmoMap.Contains(PrimaryWeapon->GetWeaponType()))
		{
			AmmoLeft = CarriedAmmoMap[PrimaryWeapon->GetWeaponType()];
			BugCharacter->OnAmmoLeftChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
		}
	}
	PrimaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedPrimary);
	BugCharacter->SetCombatState(ECombatState::ECS_Equipping);
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (SecondaryWeapon != nullptr)
	{
		SecondaryWeapon->DropWeapon();
	}
	
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetOwner(BugCharacter);
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary); 
	BugCharacter->SetCombatState(ECombatState::ECS_Equipping);
	
}

void UCombatComponent::SwapWeapons()
{
	if (SecondaryWeapon)
	{
		BugCharacter->PlayEquipMontage(SecondaryWeapon);
		RPC_SwapWeapons();
	}
	
}

void UCombatComponent::RPC_SwapWeapons_Implementation()
{
	if (PrimaryWeapon == nullptr || SecondaryWeapon == nullptr) return;
	AWeapon* TempWeapon = PrimaryWeapon;
	PrimaryWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;
	if (PrimaryWeapon)
	{
		PrimaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedPrimary);
	}
	if (SecondaryWeapon)
	{
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	}

	if (CarriedAmmoMap.Contains(PrimaryWeapon->GetWeaponType()))
	{
		AmmoLeft = CarriedAmmoMap[PrimaryWeapon->GetWeaponType()];
		if (BugCharacter && BugCharacter->IsLocallyControlled())
		{
			BugCharacter->OnAmmoLeftChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
		}
	}
	
	CombatState = ECombatState::ECS_Equipping;
	if (BugCharacter && !BugCharacter->IsLocallyControlled())
	{
		BugCharacter->PlayEquipMontage(PrimaryWeapon);
	}
	
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
		if (BugCharacter && !BugCharacter->IsLocallyControlled()) 
		{
			BugCharacter->PlayReloadMontage();
			if (PrimaryWeapon && PrimaryWeapon->GetReloadSound())
			{
				UGameplayStatics::PlaySoundAtLocation(
				   this,
				   PrimaryWeapon->GetReloadSound(),
				   BugCharacter->GetActorLocation()
				);
			}
			if (PrimaryWeapon)
			{
				PrimaryWeapon->PlayReloadAnimation();
			}
		}
		break;
	case ECombatState::ECS_Equipping:
		break;
	case ECombatState::ECS_Max:
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			ExecuteFire(true);
		}
		break;
	case ECombatState::ECS_TossingGrenade:
		if (BugCharacter)
		{
			BugCharacter->PlayGrenadeTossMontage();
		}
		break;
	}
}

void UCombatComponent::PlayBoltAction()
{
	BugCharacter->PlayBoltMontage();
}

void UCombatComponent::Reload()
{
	if (PrimaryWeapon && PrimaryWeapon->GetCurrentAmmo() != PrimaryWeapon->GetMagCapacity() && AmmoLeft > 0 && CombatState == ECombatState::ECS_Unoccupied)
	{
		if (BugCharacter && !BugCharacter->HasAuthority())
		{
			ReloadLocal();
		}
		RPC_Reload();
	}
}

void UCombatComponent::RPC_Reload_Implementation()
{
	if (PrimaryWeapon && PrimaryWeapon->GetCurrentAmmo() != PrimaryWeapon->GetMagCapacity() && AmmoLeft > 0 && CombatState == ECombatState::ECS_Unoccupied)
	{
		CombatState = ECombatState::ECS_Reloading;
		if (BugCharacter)
		{
			BugCharacter->PlayReloadMontage();
			if (PrimaryWeapon->GetReloadSound())
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					PrimaryWeapon->GetReloadSound(),
					BugCharacter->GetActorLocation()
				);
			}
			float ReloadTime = PrimaryWeapon->GetReloadTime();
       
			GetWorld()->GetTimerManager().SetTimer(
				ReloadTimerHandle, 
				this, 
				&UCombatComponent::ServerHandleReloadFinished,
				ReloadTime, 
				false
			);
			
			PrimaryWeapon->PlayReloadAnimation();
		}
	}
}


void UCombatComponent::ReloadLocal()
{
	CombatState = ECombatState::ECS_Reloading; 

	if (BugCharacter)
	{
		BugCharacter->PlayReloadMontage();
		if (PrimaryWeapon && PrimaryWeapon->GetReloadSound())
		{
			UGameplayStatics::PlaySoundAtLocation(
			   this,
			   PrimaryWeapon->GetReloadSound(),
			   BugCharacter->GetActorLocation()
			);
		}
		if (PrimaryWeapon)
		{
			PrimaryWeapon->PlayReloadAnimation();
		}
	}
}

void UCombatComponent::ServerHandleReloadFinished()
{
	if (PrimaryWeapon)
	{
		CombatState = ECombatState::ECS_Unoccupied;
		HandleReloadAmmo();
	}
}

void UCombatComponent::OnReloadAnimationFinished()
{
	if (BugCharacter->IsLocallyControlled())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		OnCombatStateChanged();
        
		if (PrimaryWeapon) 
		{
			PrimaryWeapon->ResetAmmoCounters();
			if (!BugCharacter->HasAuthority())
			{
				int32 MagCapacity = PrimaryWeapon->GetMagCapacity();
				int32 CurrentAmmo = PrimaryWeapon->GetCurrentAmmo();
				int32 NeedToFill = MagCapacity - CurrentAmmo;
             
				if (NeedToFill > 0 && AmmoLeft > 0)
				{
					int32 FillAmount = FMath::Min(NeedToFill, AmmoLeft);
				
					PrimaryWeapon->SetClientCurrentAmmo(CurrentAmmo + FillAmount);
					PrimaryWeapon->SetCurrentAmmo(CurrentAmmo + FillAmount);
					AmmoLeft -= FillAmount;
					CarriedAmmoMap.Add(PrimaryWeapon->GetWeaponType(), AmmoLeft);
					PrimaryWeapon->OnAmmoChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
					BugCharacter->OnAmmoLeftChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
				}
			}
		}
		
	}
}

void UCombatComponent::OnEquipAnimationFinished()
{
	if (BugCharacter)
	{
		if (BugCharacter->HasAuthority())
		{
			CombatState = ECombatState::ECS_Unoccupied;
			OnCombatStateChanged();
		}
		else
		{
			RPC_EquipCompleted();
		}
	}
}

void UCombatComponent::OnTossGrenadeAnimationFinished()
{
	if (BugCharacter)
	{
		if (BugCharacter->HasAuthority())
		{
			CombatState = ECombatState::ECS_Unoccupied;
			OnCombatStateChanged();
		}
		else
		{
			RPC_TossGrenadeCompleted();
		}
	}
}

void UCombatComponent::RPC_TossGrenadeCompleted_Implementation()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void UCombatComponent::RPC_EquipCompleted_Implementation()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void UCombatComponent::HandleReloadAmmo()
{
	if (BugCharacter)
	{
		if (PrimaryWeapon)
		{
			PrimaryWeapon->ResetAmmoCounters();
		}
		if (BugCharacter->HasAuthority() && PrimaryWeapon)
		{
			int32 MagCapacity = PrimaryWeapon->GetMagCapacity();
			int32 CurrentAmmo = PrimaryWeapon->GetCurrentAmmo();
			int32 NeedToFill = MagCapacity - CurrentAmmo;
			if (NeedToFill <= 0) return;
			if (AmmoLeft >= NeedToFill)
			{
				PrimaryWeapon->SetCurrentAmmo(MagCapacity);
				PrimaryWeapon->SetClientCurrentAmmo(MagCapacity);
				AmmoLeft -= NeedToFill;
			}
			else
			{
				PrimaryWeapon->SetCurrentAmmo(CurrentAmmo + AmmoLeft);
				PrimaryWeapon->SetClientCurrentAmmo(CurrentAmmo + AmmoLeft);
				AmmoLeft = 0;
			}
			CarriedAmmoMap.Add(PrimaryWeapon->GetWeaponType(), AmmoLeft);
			PrimaryWeapon->OnAmmoChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
			BugCharacter->OnAmmoLeftChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
		}
	}
}

void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] += AmmoAmount;
		if (PrimaryWeapon)
		{
			AmmoLeft = CarriedAmmoMap[PrimaryWeapon->GetWeaponType()];
			PrimaryWeapon->OnAmmoChanged.Broadcast(PrimaryWeapon->GetCurrentAmmo(), PrimaryWeapon->GetMagCapacity(), AmmoLeft);
			if (PrimaryWeapon->GetWeaponType() == WeaponType && PrimaryWeapon->IsAmmoEmpty())
			{
				Reload();
			}
		}
	}
}



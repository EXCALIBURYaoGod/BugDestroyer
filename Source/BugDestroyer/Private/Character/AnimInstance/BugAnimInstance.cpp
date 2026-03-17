// //Copyrights @FpsLuping all reserved


#include "Character/AnimInstance/BugAnimInstance.h"

#include "DebugHelper.h"
#include "Character/BugCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/Weapon.h"

void UBugAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Character = Cast<ABugCharacter>(TryGetPawnOwner());
	
}

void UBugAnimInstance::ShouldUseAimOffsetsForRun()
{
	switch (WeaponType)
	{
	case EWeaponType::EWT_AssaultRifle:
		bUseAimOffsetsAnim = true;
		break;
	case EWeaponType::EWT_RocketLauncher:
		bUseAimOffsetsAnim = true;
		break;
	case EWeaponType::EWT_Shotgun:
		bUseAimOffsetsAnim = true;
		break;
	case EWeaponType::EWT_PlasmaPistol:
		bUseAimOffsetsAnim = Speed > 360.f ? false : true;
		break;
	case EWeaponType::EWT_SubmachineGun:
		bUseAimOffsetsAnim = Speed > 360.f ? false : true;
		break;
	case EWeaponType::EWT_SniperRifle:
		bUseAimOffsetsAnim = true;
		break;
	case EWeaponType::EWT_Max:
		break;
	}
}

void UBugAnimInstance::RotateRightHandToAim(float DeltaSeconds, AWeapon* EquippedWeapon)
{
	FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), RTS_World);
	FRotator LookAtRot;
	if (Character->IsLocallyControlled())
	{
		FVector LocalHitTarget = Character->CalculateLocalHitTarget();
		LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			RightHandTransform.GetLocation(), 
			RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - LocalHitTarget)
		);
		RightHandRotation = LookAtRot; 
	}
	else
	{
		LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			RightHandTransform.GetLocation(), 
			RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - Character->GetHitTarget())
		);
		RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRot, DeltaSeconds, 30.f);
	}
}

void UBugAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (Character)
	{
		FVector Velocity = Character->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	
		bIsInAir = Character->GetCharacterMovement()->IsFalling();
		bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
		bWeaponEquipped = Character->IsWeaponEquipped();
		bIsCrouching = Character->bIsCrouched;
		bAiming = Character->IsAiming();
		bElimmed = Character->IsElimmed();
		bWantsToMove = Character->IsWantsToMove();
		bTossingGrenade = Character->GetCombatState() == ECombatState::ECS_TossingGrenade;
		
		// 计算移动时的Strafing和lean
		FRotator AimRotation = Character->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		float TargetYaw = DeltaRot.Yaw;
		YawOffset = FMath::FInterpTo(YawOffset, TargetYaw, DeltaSeconds, 6.f);
		CharacterRotationLastFrame = CharacterRotation;
		CharacterRotation = Character->GetActorRotation();
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
		const float Target = Delta.Yaw / DeltaSeconds;
		const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
		Lean = FMath::Clamp(Interp, -90.f, 90.f);
		
		if (bWeaponEquipped && !bWeaponEquippedLastFrame)
		{
			StartingAimRotation = Character->GetActorRotation();
		}
		bWeaponEquippedLastFrame = bWeaponEquipped;
		
		// 计算AimOffsets
		if (bWeaponEquipped)
		{
			WeaponType = Character->GetEquippedWeapon()->GetWeaponType();
			ShouldUseAimOffsetsForRun();
			FRotator CurrentRotation = Character->GetBaseAimRotation();
			if (!Character->IsLocallyControlled())
			{
				CurrentRotation.Yaw = Character->GetNetEstimatedAimYaw();
			}
			StartingAimRotation = Character->GetActorRotation();
			FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, StartingAimRotation);
			if (Speed <= 1.f && !bIsInAir) 
			{
				if (Character)
				{
					Character->GetCharacterMovement()->bOrientRotationToMovement = true;
					Character->bUseControllerRotationYaw = false;
				}
				AO_Yaw= DeltaAimRotation.Yaw;
				TurnInPlace(DeltaSeconds);
			}
			if (Speed > 1.f || bIsInAir) 
			{
				if (Character)
				{
					Character->GetCharacterMovement()->bOrientRotationToMovement = false;
					Character->bUseControllerRotationYaw = true;
				}
				AO_Yaw = 0.f;
				TurningInPlace = ETurningInPlace::ETIP_Center;
			}
			AO_Pitch = CurrentRotation.Pitch;
			if (AO_Pitch > 90.f && !Character->IsLocallyControlled())
			{
				// map pitch from [270, 360) to [-90, 0)
				FVector2D InRange(270.f, 360.f);
				FVector2D OutRange = FVector2D(-90.f, 0.f);
				AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
			}
		}
		
		// Fabric Ik
		if (bWeaponEquipped)
		{
			
			AWeapon* EquippedWeapon = Character->GetEquippedWeapon();
			if (EquippedWeapon && EquippedWeapon->GetWeaponMesh() && Character->GetMesh())
			{
				LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
				FVector OutPosition;
				FRotator OutRotation;
				
				Character->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), 
					FRotator::ZeroRotator, OutPosition, OutRotation);
				LeftHandTransform.SetLocation(OutPosition);
				LeftHandTransform.SetRotation(FQuat(OutRotation));
				
				// 旋转右手使武器朝向准心
				RotateRightHandToAim(DeltaSeconds, EquippedWeapon);
				// 旋转右手使武器朝向准心
				
			}
		}
		//Debug::Print(FString::Printf(TEXT("AO_Yaw: %f\n TurningInPlace: %s"), AO_Yaw, *UEnum::GetValueAsString(TurningInPlace)));
	}
	
}

void UBugAnimInstance::TurnInPlace(float DeltaTime)
{
	if (bWantsToMove)
	{
		TurningInPlace = ETurningInPlace::ETIP_Center;
		return;
	}
	if (AO_Yaw > 135.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_BackRight;
	}
	else if (AO_Yaw < -135.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_BackLeft;
	}
	else if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	
	if (TurningInPlace != ETurningInPlace::ETIP_Center)
	{
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_Center;
		}
	}
}

void UBugAnimInstance::AnimNotify_EquipAnimFinished()
{
	if (Character)
	{
		Character->OnEquipAnimationFinished();
	}
}

void UBugAnimInstance::AnimNotify_ReloadFinished()
{
	if (Character)
	{
		Character->OnReloadAnimationFinished();
	}
}

void UBugAnimInstance::AnimNotify_TossGrenadeFinished()
{
	if (Character)
	{
		Character->OnTossGrenadeFinished();
	}
}

void UBugAnimInstance::AnimNotify_LaunchGrenade()
{
	if (Character)
	{
		Character->LaunchGrenade();
	}
}

void UBugAnimInstance::ResetTurnInPlace()
{
	TurningInPlace = ETurningInPlace::ETIP_Center;
}
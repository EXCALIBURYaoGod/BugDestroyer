// //Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BugTypes/BugEnumTypes.h"
#include "BugAnimInstance.generated.h"

class ABugCharacter;

/**
 * 角色动画实例类
 */
UCLASS()
class BUGDESTROYER_API UBugAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	//begin UAnimInstance Interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	//end UAnimInstance Interface

	
protected:
	
	
private:
	UPROPERTY(BlueprintReadOnly, Category= "Character", meta=(AllowPrivateAccess=true))
	ABugCharacter* Character;
	UPROPERTY(BlueprintReadOnly, Category= "Character", meta=(AllowPrivateAccess=true))
	bool bWeaponEquipped;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	bool bAiming;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	bool bWantsToMove;
	UPROPERTY(BlueprintReadOnly, Category= "Character", meta=(AllowPrivateAccess=true))
	bool bElimmed = false;
	
	// bIsAccelerating 指是否有移动action按键输入
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	bool bIsAccelerating;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	bool bIsCrouching;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	float YawOffset;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	float Lean;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	float AO_Yaw;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
    float AO_Pitch;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	FTransform LeftHandTransform;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	ETurningInPlace TurningInPlace = ETurningInPlace::ETIP_Center;
	UPROPERTY(BlueprintReadOnly, Category= "CharacterMovement", meta=(AllowPrivateAccess=true))
	FRotator RightHandRotation;
	
	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator StartingAimRotation;
	
	bool bWeaponEquippedLastFrame;
	
	void TurnInPlace(float DeltaTime);
	UFUNCTION()
	void AnimNotify_EquipAnimFinished();
	UFUNCTION()
	void AnimNotify_ReloadFinished();
	
public:
	UFUNCTION(BlueprintCallable, Category = "AnimationNotifyCalls")
	void ResetTurnInPlace();
	
};

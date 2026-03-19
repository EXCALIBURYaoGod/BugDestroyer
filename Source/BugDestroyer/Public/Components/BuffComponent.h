// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BUGDESTROYER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffComponent();
	friend class ABugCharacter;
	
protected:
	virtual void BeginPlay() override;
	
	// Heal Buff
	void Heal(float HealAmount, float HealingTime);
	void HealRampUp(float DeltaTime);
	// Heal Buff
	
	// Shield Buff
	void ShieldReplenish(float ShieldAmount, float ShieldReplenishTime);
	void ShieldReplenishRampUp(float DeltaTime);
	// Shield Buff
	
	// Speed Buff
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffSpeedTime);
	void SetInitialSpeed(float BaseSpeed, float CrouchSpeed);
	// Speed Buff
	
	// Jump Buff
	void SetInitialJumpVelocity(float JumpVelocity);
	void BuffJump(float JumpBuffVelocity, float JumpBuffTime);
	// Jump Buff
	
private:
	UPROPERTY()
	ABugCharacter* BugCharacter;
	
	// Heal Buff
	bool bHealing;
	float HealingRate = 0; //每秒治疗量
	float AmountToHeal = 0.f; //总计要治疗的量
	// Heal Buff
	
	// Shield Buff
	bool bReplenishingShield = false;
	float ShieldReplenishRate = 0.f;
	float AmountToReplenish = 0.f;
	// Shield Buff
	
	// Speed Buff
	bool bSpeedBuffing = false;
	FTimerHandle SpeedBuffTimer;
	void ResetSpeed();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SpeedBuff(float BaseSpeed, float CrouchSpeed, bool bPlaySprintAnimation);
	// Speed Buff
	
	// Jump Buff
	bool bJumpBuffing = false;
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_JumpBuff(float JumpBuffVelocity);
	// Jump Buff
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	
};

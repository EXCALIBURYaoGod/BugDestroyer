// Copyright @FpsLuping all reserved


#include "Components/BuffComponent.h"

#include "Character/BugCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/Weapon.h"


UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	AmountToHeal += HealAmount;
	if (HealingTime > 0.f)
	{
		HealingRate = AmountToHeal / HealingTime;
	}
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || BugCharacter == nullptr || BugCharacter->IsElimmed()) return;
	const float HealThisFrame = HealingRate * DeltaTime;
	const float CurrentHealth = FMath::Clamp(HealThisFrame + BugCharacter->GetCurrentHealth(), 0.f, BugCharacter->GetMaxHealth());
	BugCharacter->SetCurrentHealth(CurrentHealth);
	BugCharacter->OnHealthChanged.Broadcast(CurrentHealth, BugCharacter->GetMaxHealth());
	AmountToHeal -= HealThisFrame;
	if (AmountToHeal <= 0.0f || BugCharacter->GetCurrentHealth() >= BugCharacter->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.0f;
	}
}

void UBuffComponent::ShieldReplenish(float ShieldAmount, float ShieldReplenishTime)
{
	bReplenishingShield= true;
	AmountToReplenish += ShieldAmount;
	if (ShieldReplenishTime > 0.f)
	{
		ShieldReplenishRate = AmountToReplenish / ShieldReplenishTime;
	}
}

void UBuffComponent::ShieldReplenishRampUp(float DeltaTime)
{
	if (!bReplenishingShield || BugCharacter == nullptr || BugCharacter->IsElimmed()) return;
	const float ShieldThisFrame = ShieldReplenishRate * DeltaTime;
	const float CurrentShield = FMath::Clamp(ShieldThisFrame + BugCharacter->GetCurrentShield(), 0.f, BugCharacter->GetMaxShield());
	BugCharacter->SetCurrentShield(CurrentShield);
	BugCharacter->OnShieldChanged.Broadcast(CurrentShield, BugCharacter->GetMaxShield());
	AmountToReplenish -= ShieldThisFrame;
	if (AmountToReplenish <= 0.0f || BugCharacter->GetCurrentShield() >= BugCharacter->GetMaxShield())
	{
		bReplenishingShield = false;
		AmountToReplenish = 0.0f;
	}
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffSpeedTime)
{
	if (BugCharacter == nullptr || bSpeedBuffing) return;
	if (BugCharacter->GetEquippedWeapon() && BugCharacter->GetEquippedWeapon()->GetWeaponType() == EWeaponType::EWT_RocketLauncher)
	{
		if (UCharacterMovementComponent* CharMovComp = BugCharacter->GetCharacterMovement())
		{
			CharMovComp->MaxWalkSpeed = BugCharacter->GetDefaultMaxWalkSpeed();
			CharMovComp->MaxWalkSpeedCrouched = BugCharacter->GetDefaultMaxWalkSpeedCrouched();
		}
		return;
	}
	bSpeedBuffing = true;
	if (UCharacterMovementComponent* CharMovComp = BugCharacter->GetCharacterMovement())
	{
		InitialBaseSpeed = CharMovComp->MaxWalkSpeed;
		InitialCrouchSpeed = CharMovComp->MaxWalkSpeedCrouched;
		CharMovComp->MaxWalkSpeed = BuffBaseSpeed;
		CharMovComp->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}
	
	BugCharacter->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&ThisClass::ResetSpeed,
		BuffSpeedTime
	);
	BugCharacter->SetPlaySprintAnimation(false);
	MulticastRPC_SpeedBuff(BuffBaseSpeed, BuffCrouchSpeed, false);
	
}

void UBuffComponent::SetInitialSpeed(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UBuffComponent::ResetSpeed()
{
	if (BugCharacter == nullptr || !bSpeedBuffing) return;
	bSpeedBuffing = false;
	float MaxWalkSpeedToReset = InitialBaseSpeed;
	if (UCharacterMovementComponent* CharMovComp = BugCharacter->GetCharacterMovement())
	{
		if (BugCharacter->IsSprintButtonPressed())
		{
			MaxWalkSpeedToReset = BugCharacter->GetSprintMaxWalkSpeed();
		}
		else
		{
			MaxWalkSpeedToReset = BugCharacter->GetDefaultMaxWalkSpeed();
		}
		CharMovComp->MaxWalkSpeed = MaxWalkSpeedToReset;
		CharMovComp->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	}
	BugCharacter->SetPlaySprintAnimation(true);
	MulticastRPC_SpeedBuff(MaxWalkSpeedToReset, InitialCrouchSpeed, true);
}

void UBuffComponent::MulticastRPC_SpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed, bool bPlaySprintAnimation)
{
	if (BugCharacter == nullptr) return;
	if (UCharacterMovementComponent* CharMovComp = BugCharacter->GetCharacterMovement())
	{
		CharMovComp->MaxWalkSpeed = BaseSpeed;
		CharMovComp->MaxWalkSpeedCrouched = CrouchSpeed;
	}
	BugCharacter->SetPlaySprintAnimation(bPlaySprintAnimation);
}

void UBuffComponent::SetInitialJumpVelocity(float JumpVelocity)
{
	InitialJumpVelocity = JumpVelocity;
}

void UBuffComponent::BuffJump(float JumpBuffVelocity, float JumpBuffTime)
{
	if (BugCharacter == nullptr || bSpeedBuffing) return;

	if (UCharacterMovementComponent* CharMovComp = BugCharacter->GetCharacterMovement())
	{
		CharMovComp->JumpZVelocity = JumpBuffVelocity;
	}
	
	BugCharacter->GetWorldTimerManager().SetTimer(
	SpeedBuffTimer,
	this,
	&ThisClass::ResetJump,
	JumpBuffTime
	);
	MulticastRPC_JumpBuff(JumpBuffVelocity);
}

void UBuffComponent::ResetJump()
{
	if (UCharacterMovementComponent* CharMovComp = BugCharacter->GetCharacterMovement())
	{
		CharMovComp->JumpZVelocity = InitialJumpVelocity;
	}
	MulticastRPC_JumpBuff(InitialJumpVelocity);
	
}


void UBuffComponent::MulticastRPC_JumpBuff_Implementation(float JumpBuffVelocity)
{
	if (BugCharacter == nullptr) return;
	if (UCharacterMovementComponent* CharMovComp = BugCharacter->GetCharacterMovement())
	{
		CharMovComp->JumpZVelocity = JumpBuffVelocity;
	}
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HealRampUp(DeltaTime);
	ShieldReplenishRampUp(DeltaTime);
}


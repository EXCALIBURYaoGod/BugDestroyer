//Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "InputActionValue.h"
#include "Components/BuffComponent.h"
#include "Components/CombatComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "BugCharacter.generated.h"

class ULagCompensationComponent;
class UBuffComponent;
class UCombatComponent;
class AWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChangedSignature, float, NewShield, float, MaxShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChangedSignature, AWeapon*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoLeftChangedSignature, int32, CurrentAmmo, int32, MagCapacity, int32, AmmoLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrenadeAmountChangedSignature, int32, CurrentGrenade);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetWarningSignature, bool, bIsWarning);

/**
 *	当前作为快速验证原型的角色类
 * TODO: 改造为角色容器，不包含业务逻辑，仅作为容器，将各个组件模块挂载到一起
 */
UCLASS()
class BUGDESTROYER_API ABugCharacter : public ACharacter, public IInteractWithCrosshairsInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ABugCharacter();
	
	// begin AActor Interface
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// end AActor Interface
	
	// begin Character Interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	// end Character Interface
	
	void GetHit(const FVector& HitPoint);
	void EliminateCharacter();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_EliminateCharacter();
	FORCEINLINE virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer = OwnedTags;
	}
	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	FVector CalculateLocalHitTarget();
	
protected:
	virtual void BeginPlay() override;
	
	// Input Actions //
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* GrenadeTossAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwapAction;
	bool bWantsToMove = false;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PlayEquipFX();
	void EquipButtonPressed();
	void StartCrouch();
	void StopCrouch();
	void StartAiming();
	void StopAiming();
	void StartSprint();
	void StopSprint();
	void StartFiring();
	void StopFiring();
	void ReloadButtonPressed();
	void ReleaseMoveButton();
	void GrenadeButtonPressed();
	void SwapButtonPressed();
	
	// begin Character Interface
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// end Character Interface
	
	// Input Actions //
	
	UPROPERTY(EditAnywhere, Category = "GameplayTags")
	FGameplayTagContainer OwnedTags;
	
	// == Hit boxes used for server-side rewind == //
	UPROPERTY(EditAnywhere, Category = "HitBox")
	class UBoxComponent* Head;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* Pelvis;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* Spine;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* UpperArm_l;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* UpperArm_r;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* LowerArm_l;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* LowerArm_r;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* Thigh_l;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* Thigh_r;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* Calf_l;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* Calf_r;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* Foot_l;
	UPROPERTY(EditAnywhere, Category = "HitBox")
	UBoxComponent* Foot_r;
	UPROPERTY(VisibleAnywhere, Category = "HitBox")
	TMap<FName, UBoxComponent*> HitBoxes;
	/** * 封装 HitBox 初始化的辅助模板函数 
	 * @param BoxComponent 引用类成员变量（如 Head, Pelvis）
	 * @param Name 组件的 Object Name
	 * @param BoneName 附加到的骨骼名称
	 */
	template<typename T>
	void InitHitBox(T*& BoxComponent, FName Name, FName BoneName)
	{
		BoxComponent = CreateDefaultSubobject<T>(Name);
		BoxComponent->SetupAttachment(GetMesh(), BoneName);
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitBoxes.Add(Name, BoxComponent);
	}
	// == Hit boxes used for server-side rewind == //
	
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, Category= "Grenade")
	UStaticMeshComponent* AttachedGrenadeMesh;
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;
	UPROPERTY(VisibleAnywhere, Category = "Equip")
	TArray<AWeapon*> OverlappingWeapons;
	AWeapon* GetNearestWeaponInArray();
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	
	// 用于将 AimRotation 同步给其他玩家
	UPROPERTY(Replicated)
	float NetEstimatedAimYaw;
	
	// Health
	UPROPERTY(EditAnywhere, Category = "Player States")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player States")
	float CurrentHealth = 100.f;
	UFUNCTION()
	void OnRep_Health();
	// Health
	
	// Shield
	UPROPERTY(EditAnywhere, Category = "Player States")
	float MaxShield = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, VisibleAnywhere, Category = "Player States")
	float CurrentShield = 100.f;
	UFUNCTION()
	void OnRep_Shield();
	// Shield
	
	// == Components == //
	UPROPERTY(VisibleAnywhere)
	UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere)
	UBuffComponent* BuffComponent;
	UPROPERTY(VisibleAnywhere)
	ULagCompensationComponent* LagCompensationComponent;
	// == Components == //
	
	
	UFUNCTION(Server, Reliable)
	void RPC_ServerEquipButtonPressed();
	UFUNCTION(Server, Reliable)
	void RPC_Sprint(bool bIsSprint);
	UPROPERTY(ReplicatedUsing = OnRep_MaxWalkSpeed)
	float ServerMaxWalkSpeed = 350.f;
	UPROPERTY(EditAnywhere, Category = "Speed")
	float DefaultMaxWalkSpeed = 350.f;
	UPROPERTY(EditAnywhere, Category = "Speed")
	float DefaultMaxWalkSpeedCrouched = 200.f;
	UPROPERTY(EditAnywhere, Category = "Speed")
	float SprintMaxWalkSpeed = 600.f;
	bool bSprintButtonPressed = false;
	UPROPERTY(EditAnywhere, Category= "Montages")
	UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, Category= "Montages")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category= "Montages")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, Category= "Montages")
	UAnimMontage* EquipMontage;
	UPROPERTY(EditAnywhere, Category= "Montages")
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, Category= "Montages")
	UAnimMontage* GrenadeTossMontage;
	
	UPROPERTY(ReplicatedUsing = OnRep_ProjectileImpactPoint, VisibleAnywhere, Category= "Combat")
	FVector_NetQuantize ProjectileImpactPoint;	
	
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	bool bElimmed;
	FTimerHandle ElimTimerHandle;
	void ElimTimerFinished();
	
	// Dissolve effect
	UPROPERTY(VisibleAnywhere, Category= "Combat")
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
	UFUNCTION()
	void OnDissolveTimelineFinished();
	UPROPERTY(EditAnywhere, Category = "Combat")
	UCurveFloat* DissolveCurve;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TMap<FName, UMaterialInterface*> SlotSpecificDissolveMaterials;
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TArray<UMaterialInstanceDynamic*> DynamicDissolveInstances;
	// Dissolve effect
	
	
	UFUNCTION()
	void OnRep_MaxWalkSpeed();
	UFUNCTION()
	void OnRep_ProjectileImpactPoint();
	
	// Only LocalControlled Calls
	void HideIfCameraClose(float DeltaTime);
	void CreateMIDs();
	float CameraThreshold = 150.f; // 当相机距离玩家小于此距离，将隐藏玩家
	float CurrentDitherAlpha =  1.f;
	UPROPERTY(VisibleAnywhere, Category = "Character")
	TArray<UMaterialInstanceDynamic*> DynamicMeshMID;

public:
	void SetOverlappingWeapon(AWeapon* Weapon, bool bIsOverlapping);
	void SetMaxWalkSpeed(float InMaxWalkSpeed);
	
	// Animation Notify Callbacks
	void OnReloadAnimationFinished();
	void OnEquipAnimationFinished();
	void OnTossGrenadeFinished();
	// Animation Notify Callbacks
	
	FORCEINLINE bool IsWeaponEquipped() const { return CombatComponent && CombatComponent->PrimaryWeapon; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE bool IsAiming() const { return CombatComponent && CombatComponent->bAiming; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensationComponent() const { return LagCompensationComponent; }
	FORCEINLINE float GetNetEstimatedAimYaw() const { return NetEstimatedAimYaw; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE int32 GetAmmoLeft() const { return CombatComponent->AmmoLeft; }
	FORCEINLINE bool IsUnoccupied() const { return CombatComponent->CombatState == ECombatState::ECS_Unoccupied; }
	FORCEINLINE void SetUnoccupied() const{ CombatComponent->CombatState = ECombatState::ECS_Unoccupied; CombatComponent->OnCombatStateChanged(); }
	FORCEINLINE void SetCombatState(ECombatState NewState) const { CombatComponent->CombatState = NewState; CombatComponent->OnCombatStateChanged(); }
	FORCEINLINE ECombatState GetCombatState() const { return CombatComponent->CombatState; }
	FORCEINLINE bool IsWantsToMove() const{ return bWantsToMove; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenadeMesh; }
	FORCEINLINE void LaunchGrenade() const{ CombatComponent->LaunchGrenade(); }
	FORCEINLINE int32 GetGrenadeAmount() const { return CombatComponent->GrenadeAmount; }
	FORCEINLINE void Healing (float HealAmount, float HealingTime) const{ if (BuffComponent) BuffComponent->Heal(HealAmount, HealingTime); }
	FORCEINLINE void ShieldReplenish (float ShieldAmount, float ShieldReplenishTime) const{ if (BuffComponent) BuffComponent->ShieldReplenish(ShieldAmount, ShieldReplenishTime); }
	FORCEINLINE void BuffingSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffSpeedTime) const{ if (BuffComponent) BuffComponent->BuffSpeed(BuffBaseSpeed, BuffCrouchSpeed, BuffSpeedTime); }
	FORCEINLINE bool IsSprintButtonPressed() const { return bSprintButtonPressed; }
	FORCEINLINE float GetDefaultMaxWalkSpeed() const { return DefaultMaxWalkSpeed; }
	FORCEINLINE float GetDefaultMaxWalkSpeedCrouched() const { return DefaultMaxWalkSpeedCrouched; }
	FORCEINLINE float GetSprintMaxWalkSpeed() const { return SprintMaxWalkSpeed; }
	FORCEINLINE void BuffingJump(float JumpVelocity, float JumpBuffTime) const { if (BuffComponent) BuffComponent->BuffJump(JumpVelocity, JumpBuffTime); }
	FORCEINLINE TMap<FName, UBoxComponent*> GetHitBoxes() const { return HitBoxes; }
	
	FVector GetHitTarget() const;
	AWeapon* GetEquippedWeapon() const;
	void PlayFireMontage();
	void PlayHitReactMontage(const FVector& HitPoint);
	void PlayDeathMontage(const FVector& HitPoint);
	void PlayEquipMontage(AWeapon* WeaponToEquip);
	void PlayReloadMontage();
	void PlayGrenadeTossMontage();
	// Health
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetMaxHealth(float NewMaxHealth) { MaxHealth = NewMaxHealth; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentHealth(float NewCurrentHealth) { CurrentHealth = NewCurrentHealth; }
	// Health
	
	// Shield
	UPROPERTY(BlueprintAssignable)
	FOnShieldChangedSignature OnShieldChanged;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetMaxShield(float NewMaxShield) { MaxShield = NewMaxShield; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentShield() const { return CurrentShield; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentShield(float NewCurrentShield) { CurrentShield = NewCurrentShield; }
	// Shield
	
	UPROPERTY(BlueprintAssignable)
	FOnWeaponChangedSignature OnWeaponChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAmmoLeftChangedSignature OnAmmoLeftChanged;
	UPROPERTY(BlueprintAssignable)
	FOnGrenadeAmountChangedSignature OnGrenadeAmountChanged;
	UPROPERTY(BlueprintAssignable)
	FOnNetWarningSignature OnNetWarning;
	
};

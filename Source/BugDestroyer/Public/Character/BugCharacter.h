//Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "InputActionValue.h"
#include "Components/CombatComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "BugCharacter.generated.h"

class UCombatComponent;
class AWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChangedSignature, AWeapon*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoLeftChangedSignature, int32, CurrentAmmo, int32, MagCapacity, int32, AmmoLeft);

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
	
	void EliminateCharacter();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_EliminateCharacter();

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		// 把你角色身上的标签塞进去
		TagContainer = OwnedTags;
	}
	
protected:
	virtual void BeginPlay() override;
	
	// Input Actions //
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
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
	bool bWantsToMove = false;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
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
	// begin Character Interface
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// end Character Interface
	// Input Actions //
	
	UPROPERTY(EditAnywhere, Category = "GameplayTags")
	FGameplayTagContainer OwnedTags;
	
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;
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
	
	UPROPERTY(VisibleAnywhere)
	UCombatComponent* CombatComponent;
	UFUNCTION(Server, Reliable)
	void RPC_ServerEquipButtonPressed();
	UFUNCTION(Server, Reliable)
	void RPC_Sprint(bool bIsSprint);
	UPROPERTY(ReplicatedUsing = OnRep_MaxWalkSpeed)
	float ServerMaxWalkSpeed;
	UPROPERTY(EditAnywhere, Category= "Combat")
	UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, Category= "Combat")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category= "Combat")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, Category= "Combat")
	UAnimMontage* EquipMontage;
	UPROPERTY(EditAnywhere, Category= "Combat")
	UAnimMontage* ReloadMontage;
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
	void GetHit(const FVector& HitPoint);
	// Only Server Calls
	void SetOverlappingWeapon(AWeapon* Weapon);
	void SetMaxWalkSpeed(float InMaxWalkSpeed);
	
	// Animation Notify Callbacks
	UFUNCTION()
	void OnReloadAnimationFinished();
	UFUNCTION()
	void OnEquipAnimationFinished();
	
	// Only Server Calls
	FORCEINLINE bool IsWeaponEquipped() const { return CombatComponent && CombatComponent->EquippedWeapon; }
	FORCEINLINE bool IsAiming() const { return CombatComponent && CombatComponent->bAiming; }
	FORCEINLINE float GetNetEstimatedAimYaw() const { return NetEstimatedAimYaw; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE int32 GetAmmoLeft() const { return CombatComponent->AmmoLeft; }
	FORCEINLINE bool IsUnoccupied() const { return CombatComponent->CombatState == ECombatState::ECS_Unoccupied; }
	FORCEINLINE void SetUnoccupied() const{ CombatComponent->CombatState = ECombatState::ECS_Unoccupied; CombatComponent->OnCombatStateChanged(); }
	FORCEINLINE void SetCombatState(ECombatState NewState) const { CombatComponent->CombatState = NewState; CombatComponent->OnCombatStateChanged(); }
	FORCEINLINE ECombatState GetCombatState() const { return CombatComponent->CombatState; }
	FORCEINLINE bool IsWantsToMove() const{ return bWantsToMove; }
	
	FVector GetHitTarget() const;
	AWeapon* GetEquippedWeapon() const;
	void PlayFireMontage();
	void PlayHitReactMontage(const FVector& HitPoint);
	void PlayDeathMontage(const FVector& HitPoint);
	void PlayEquipMontage();
	void PlayReloadMontage();
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
	UPROPERTY(BlueprintAssignable)
	FOnWeaponChangedSignature OnWeaponChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAmmoLeftChangedSignature OnAmmoLeftChanged;
	
};

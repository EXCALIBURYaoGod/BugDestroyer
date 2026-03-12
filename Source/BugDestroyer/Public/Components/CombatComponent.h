// Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_TossingGrenade UMETA(DisplayName = "TossingGrenade"),
	
	ECS_Max UMETA(DisplayName = "DefaultMAX")
};

enum class EWeaponType : uint8;
class AGameCommonPlayerController;
class ACommonPlayerController;
class ABugHud;
class AWeapon;

UCLASS()
class BUGDESTROYER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class ABugCharacter;
	
	// begin AActor Interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// end AActor Interface
	
	// only server calls
	void EquipWeapon(AWeapon* WeaponToEquip);
	void Reload();
	void HandleReloadAmmo();
	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	// only server calls

	
protected:
	virtual void BeginPlay() override;
	void PushCrosshair(float DeltaTime);
	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void RPC_ServerSetAiming(bool bIsAiming);
	UFUNCTION()
	void OnRep_PrimaryWeapon();
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	void Fire();
	void ExecuteFire(bool InIsFire);
	void TossGrenade();
	void LaunchGrenade();
	void ShowAttachedGrenade(bool bShowGrenade);
	UFUNCTION(Server, Reliable)
	void RPC_TossGrenade();
	UFUNCTION(Server, Reliable)
	void RPC_ServerStartFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_StartFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(Server, Reliable)
	void RPC_SetHitTarget(const FVector_NetQuantize& ClientHitTarget);
	UFUNCTION(Server, Reliable)
	void RPC_Reload();
	UFUNCTION(Server, Reliable)
	void RPC_ReloadAnimationFinished();
	
	void OnReloadAnimationFinished();	// Called when reload animation finishes
	void OnEquipAnimationFinished();
	void OnTossGrenadeAnimationFinished();
	UFUNCTION(Server, Reliable)
	void RPC_EquipCompleted();
	UFUNCTION(Server, Reliable)
	void RPC_TossGrenadeCompleted();
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	void SwapWeapons();
	UFUNCTION(Server, Reliable)
	void RPC_SwapWeapons();
	
private:
	UPROPERTY()
	ABugCharacter* BugCharacter;
	UPROPERTY()
	AGameCommonPlayerController* PlayerController;
	UPROPERTY()
	ABugHud* BugHud;
	UPROPERTY(ReplicatedUsing=OnRep_PrimaryWeapon, VisibleAnywhere, Category= "Equip")
	AWeapon* PrimaryWeapon;
	UPROPERTY(ReplicatedUsing=OnRep_SecondaryWeapon, VisibleAnywhere, Category= "Equip")
	AWeapon* SecondaryWeapon;
	UPROPERTY(Replicated)
	bool bAiming;
	bool bFiring;
	bool bFireButtonPressed;
	bool bEquipping;
	
	// Aiming and FOV
	float DefaultFOV;
	float CurrentFOV;
	void InterpFOV(float DeltaTime);
	// Aiming and FOV
	
	// Automatic fire
	FTimerHandle FireTimerHandle;
	bool bCanFire = true;
	void StartFireTimer();
	void FireTimerFinished();
	bool CanFire();
	// Automatic fire
	
	void TraceUnderCrosshairs(FHitResult& HitResult);
	void InitializePlayerController();
	void InitializeHUD();
	
	// Ammo
	UPROPERTY(ReplicatedUsing = OnRep_AmmoLeft, VisibleAnywhere, Category= "Combat | Ammo")
	int32 AmmoLeft = 90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Ammo", meta = (AllowPrivateAccess = "true"))
	TMap<EWeaponType, int32> CarriedAmmoMap;
	UFUNCTION()
	void OnRep_AmmoLeft();
	void InitializeCarriedAmmoMap();
	UPROPERTY(ReplicatedUsing = OnRep_GrenadeAmount, VisibleAnywhere, Category= "Combat | Ammo")
	int32 GrenadeAmount = 4;
	UFUNCTION()
	void OnRep_GrenadeAmount();
	// Ammo
	
	UPROPERTY(ReplicatedUsing=OnRep_CombatState, VisibleAnywhere, Category= "Combat")
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	UFUNCTION()
	void OnRep_CombatState();
	void OnCombatStateChanged();
	
	UPROPERTY(EditAnywhere, Category= "Grenade")
	TSubclassOf<class AProjectileGrenade> GrenadeClass;
	
public:
	UPROPERTY(Replicated)
	FVector_NetQuantize HitTarget;
	
};

// Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BugTypes/BugStructTypes.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class ABugCharacter;
class ACasing;
class UWidgetComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoChangedSignature, int32, CurrentAmmo, int32, MagCapacity, int32, AmmoLeft);

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_EquippedPrimary UMETA(DisplayName = "EquippedPrimary"),
	EWS_EquippedSecondary UMETA(DisplayName = "EquippedSecondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	
	EWS_MAX UMETA(DisplayName = "DefaultMax"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_PlasmaPistol UMETA(DisplayName = "Plasma Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SniperRifle UMETA(DisplayName = "SniperRifle"),
	
	EWT_Max UMETA(DisplayName = "DefaultMax")
};


/**
 * 武器类，目前是通用所有武器
 */
UCLASS()
class BUGDESTROYER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	
	// begin AActor Interface
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetOwner(AActor* NewOwner) override;
	// end AActor Interface

	virtual void SimulateFireFX(const FVector& HitTarget, int32 InRandomSeed);
	virtual void ServerExecuteFireLogic(const FVector& HitTarget, int32 RandomSeed);
	
	const FImpactEffectData* GetHitImpactDataByHitActorOwnTag(AActor* HitActor);
	void DropWeapon(const FVector& Direction = FVector::ZeroVector);
	void UpdateWeaponDither(float Alpha);
	void PlayReloadAnimation();
	void ShowPickupWidget(bool bShowWidget, ABugCharacter* BugCharacter = nullptr);
	void ClientSpendRoundAmmo();
	void ResetAmmoCounters();
	UPROPERTY(Replicated)
	int32 ServerAuthShots;
	int32 LocalFiredShots;

protected:
	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet();
	
	UPROPERTY()
	ABugCharacter* CachedOwningBugCharacterForEquip;
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	FImpactEffectData DefaultImpactData;
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	TMap<FGameplayTag, FImpactEffectData> TaggedImpactEffects;
	
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	
private:
	UPROPERTY(VisibleAnywhere, Category= "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category= "Weapon Properties")
	USphereComponent* AreaSphere;
	UPROPERTY(VisibleAnywhere, Category= "Weapon Properties")
	UWidgetComponent* PickupWidget;
	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	UAnimationAsset* ReloadAnimation;
	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere, Category= "Weapon Properties")
	EWeaponState WeaponState;
	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	TSubclassOf<ACasing> CasingClass;
	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere, Category= "Weapon Properties")
	float ZoomedInterpSpeed = 20.f;
	UPROPERTY(EditAnywhere, Category= "Weapon Properties", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float FireCrosshairSpread = 10.f;
	UPROPERTY(EditAnywhere, Category= "Weapon Properties", meta = (ClampMax = "0.0", UIMax = "0.0"))
	float AimCrosshairSpread = -10.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float FireDelay = .15f;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bAutomatic = true;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties", ReplicatedUsing = OnRep_CurrentAmmo )
	int32 CurrentAmmo = 30;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 MagCapacity = 30;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Mesh")
	TArray<UMaterialInstanceDynamic*> DynamicMeshMID;
	UPROPERTY(EditAnywhere)
	USoundBase* EquipSound;
	UPROPERTY(EditAnywhere)
	USoundBase* ReloadSound;
	
	UFUNCTION()
	void OnRep_WeaponState();
	UFUNCTION()
	void OnRep_CurrentAmmo();
	void AttachToCharacter(const FName& SocketName);
	void ServerSpendRoundAmmo();
	void CreateMIDs();
	
public:
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; OnWeaponStateSet(); }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomedInterpSpeed() const { return ZoomedInterpSpeed; }
	FORCEINLINE float GetFireCrosshairSpread() const { return FireCrosshairSpread; }
	FORCEINLINE float GetAimCrosshairSpread() const { return AimCrosshairSpread; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE void SetFireDelay(float InDelay) { FireDelay = FMath::Max(0.f, InDelay); }
	FORCEINLINE bool IsAutomatic() const { return bAutomatic; }
	FORCEINLINE void SetAutomatic(bool bInAutomatic) { bAutomatic = bInAutomatic; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }
	FORCEINLINE void SetCurrentAmmo(int32 InCurrentAmmo) { CurrentAmmo = InCurrentAmmo; }
	FORCEINLINE bool IsAmmoEmpty() const { return CurrentAmmo == 0; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE USoundBase* GetEquipSound() const { return EquipSound; }
	FORCEINLINE USoundBase* GetReloadSound() const { return ReloadSound; }
	FORCEINLINE UAnimationAsset* GetFireAnimation() const { return FireAnimation; }
	FORCEINLINE TSubclassOf<ACasing> GetCasingClass() const { return CasingClass; }
	
	UPROPERTY(BlueprintAssignable)
	FOnAmmoChangedSignature OnAmmoChanged;
	
};





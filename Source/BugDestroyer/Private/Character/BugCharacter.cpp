// Copyrights @FpsLuping all reserved


#include "Character/BugCharacter.h"

#include "DebugHelper.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "BugDestroyer/BugDestroyer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameMode/CommonGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Weapon.h"


ABugCharacter::ABugCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SKM);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 720.f);
	
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
	
}



void ABugCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
	CreateMIDs();
}


void ABugCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		// 获取当前的偏航角，并规范化到 -180~180
		NetEstimatedAimYaw = FRotator::NormalizeAxis(GetBaseAimRotation().Yaw);
	}
	
	HideIfCameraClose(DeltaTime);

	if (CombatComponent)
	{
		const UEnum* EnumPtr = StaticEnum<ECombatState>();
		FString StateString = EnumPtr ? EnumPtr->GetNameStringByValue((int64)CombatComponent->CombatState) : TEXT("Invalid");
		FString RolePrefix = CombatComponent->GetOwner()->HasAuthority() ? TEXT("[Server]") : TEXT("[Client]");
		FString FinalMessage = FString::Printf(TEXT("%s CombatState: %s"), *RolePrefix, *StateString);

	}

}

void ABugCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 将OverlappingWeapon设置为所有客户端都能复制，而不仅仅是角色拥有者
	DOREPLIFETIME(ABugCharacter, OverlappingWeapon);
	DOREPLIFETIME(ABugCharacter, CurrentHealth);
	DOREPLIFETIME(ABugCharacter, ProjectileImpactPoint);
}

void ABugCharacter::Move(const FInputActionValue& Value)
{
	bWantsToMove = true;
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void ABugCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABugCharacter::EquipButtonPressed()
{
	if (CombatComponent && OverlappingWeapon)
	{

		if (HasAuthority())
		{
			CombatComponent->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			RPC_ServerEquipButtonPressed();
		}
	}
}

void ABugCharacter::StartCrouch()
{
	if (GetCharacterMovement()->IsFalling()) return;
	if (!bIsCrouched)
	{
		Crouch();
	}
}

void ABugCharacter::StopCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void ABugCharacter::StartAiming()
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(true);
	}
}

void ABugCharacter::StopAiming()
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
	}
}

void ABugCharacter::StartSprint()
{
	if (HasAuthority())
	{
		SetMaxWalkSpeed(600.f);
	}
	else
	{
		RPC_Sprint(true);
	}
}

void ABugCharacter::StopSprint()
{
	if (HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = 350.f;
	}
	else
	{
		RPC_Sprint(false);
	}
}

void ABugCharacter::StartFiring()
{
	if (CombatComponent)
	{
		CombatComponent->ExecuteFire(true);
	}
}

void ABugCharacter::StopFiring()
{
	if (CombatComponent)
	{
		CombatComponent->ExecuteFire(false);
	}
}

void ABugCharacter::ReloadButtonPressed()
{
	if (CombatComponent)
	{
		CombatComponent->Reload();
	}
}

void ABugCharacter::ReleaseMoveButton()
{
	bWantsToMove = false;
}

void ABugCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
		Super::Jump();
	}
	else
	{
		Super::Jump();
	}
}

float ABugCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	
	if (HasAuthority())
	{	// ListenServer本地需手动调用伤害处理
		CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth); 
		if (CurrentHealth == 0.f)
		{
			ACommonGameMode* GameMode = GetWorld()->GetAuthGameMode<ACommonGameMode>();
			if (GameMode)
			{
				if (GetController())
				{
					GameMode->PlayerEliminated(this, GetController(), EventInstigator);
				}
			}
		}
	}
	
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABugCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true, this);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false, this);
	}
}

void ABugCharacter::OnRep_Health()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}


void ABugCharacter::RPC_ServerEquipButtonPressed_Implementation()
{
	if (CombatComponent && OverlappingWeapon)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
}

void ABugCharacter::RPC_Sprint_Implementation(bool bIsSprint)
{
	if (bIsSprint)
	{
		SetMaxWalkSpeed(600.f);
	}
	else
	{
		SetMaxWalkSpeed(300.f);
	}
	
}


void ABugCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	float DynamicGlow = FMath::Lerp(100.f, 200.f, DissolveValue);
	for (UMaterialInstanceDynamic* MID : DynamicDissolveInstances)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
			MID->SetScalarParameterValue(TEXT("Glow"), DynamicGlow);
		}
	}
}

void ABugCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABugCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		if (DeathMontage && DissolveTimeline)
		{
			// FromFront3指定的动画时长最短，为了同步消失选取最短
			int32 SectionIndex = DeathMontage->GetSectionIndex(FName(TEXT("FromFront3")));
			if (SectionIndex != INDEX_NONE)
			{
				float SectionLength = DeathMontage->GetSectionLength(SectionIndex);
				float CurveMin, CurveMax;
				DissolveCurve->GetTimeRange(CurveMin, CurveMax);
				float CurveDuration = CurveMax - CurveMin;

				float NewPlayRate = CurveDuration / SectionLength;
				DissolveTimeline->SetPlayRate(NewPlayRate);

			}
		}
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
		
	}
}

void ABugCharacter::CreateMIDs()
{
	const int32 NumMaterials = GetMesh() ? GetMesh()->GetNumMaterials() : 0;
	DynamicMeshMID.Empty();
	for (int32 i = 0; i < NumMaterials; ++i)
	{
		UMaterialInstanceDynamic* MID = GetMesh()->CreateDynamicMaterialInstance(i);
		if (MID)
		{
			DynamicMeshMID.Add(MID);
		}
	}
}

void ABugCharacter::OnRep_MaxWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = ServerMaxWalkSpeed;
}

void ABugCharacter::OnRep_ProjectileImpactPoint()
{
	PlayHitReactMontage(ProjectileImpactPoint);
}


void ABugCharacter::HideIfCameraClose(float DeltaTime)
{
	if (!IsLocallyControlled()) return;
	
	const float Distance = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size();
	const float TargetAlpha = (Distance < CameraThreshold) ? 0.2f : 1.0f;
	
	CurrentDitherAlpha = FMath::FInterpTo(CurrentDitherAlpha, TargetAlpha, DeltaTime, 10.f);
	
	for (UMaterialInstanceDynamic* MID : DynamicMeshMID)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(FName("GhostAlpha"), CurrentDitherAlpha);
		}
	}
	
	if (CombatComponent && CombatComponent->EquippedWeapon)
	{
		CombatComponent->EquippedWeapon->UpdateWeaponDither(CurrentDitherAlpha);
	}
}



void ABugCharacter::GetHit(const FVector& HitPoint)
{
	ProjectileImpactPoint = HitPoint;
	if (HasAuthority())
	{
		PlayHitReactMontage(ProjectileImpactPoint); // 服务端本地调用
	}
	
}

// only Server calls
void ABugCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsLocallyControlled())
	{
		if (Weapon == nullptr)
        {
        	if (OverlappingWeapon)
        	{
        		OverlappingWeapon->ShowPickupWidget(false, this);
        	}
        }
	}


	OverlappingWeapon = Weapon;
	
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true, this);
		}
	}
}

void ABugCharacter::SetMaxWalkSpeed(float InMaxWalkSpeed)
{
	if (HasAuthority())
	{
		ServerMaxWalkSpeed = InMaxWalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed = InMaxWalkSpeed;
	}
}

FVector ABugCharacter::GetHitTarget() const
{
	if (!CombatComponent) return FVector::ZeroVector;
	return CombatComponent->HitTarget;
}

AWeapon* ABugCharacter::GetEquippedWeapon() const
{
	if (CombatComponent && CombatComponent->EquippedWeapon)
	{
		return CombatComponent->EquippedWeapon;
	}
	return nullptr;
}


void ABugCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABugCharacter::Move);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ABugCharacter::ReleaseMoveButton);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABugCharacter::ReleaseMoveButton);
		}
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABugCharacter::Look);
		}
		if (EquipAction)
		{
			EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ABugCharacter::EquipButtonPressed);
		}
		if (CrouchAction)
		{
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABugCharacter::StartCrouch);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Canceled, this, &ABugCharacter::StopCrouch);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABugCharacter::StopCrouch);
		}
		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABugCharacter::StartAiming);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &ABugCharacter::StopAiming);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABugCharacter::StopAiming);
		}
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABugCharacter::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &ABugCharacter::StopSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABugCharacter::StopSprint);
		}
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABugCharacter::StartFiring);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &ABugCharacter::StopFiring);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABugCharacter::StopFiring);
		}
		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ABugCharacter::ReloadButtonPressed);
		}
	}
}

void ABugCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComponent)
	{
		CombatComponent->BugCharacter = this;
	}
}

void ABugCharacter::EliminateCharacter()
{
	if (CombatComponent && CombatComponent->EquippedWeapon)
	{
		CombatComponent->EquippedWeapon->DropWeapon();
		CombatComponent->EquippedWeapon = nullptr;
	}
	/*// 清理UI
	UBugUISubsystem* UISubsystem = UBugUISubsystem::Get(GetController());
	if (UISubsystem)
	{
		Debug::Print(TEXT("111"));
		UWidget_PrimaryLayout* Layout = UISubsystem->GetCreatedPrimaryLayout();
		if (Layout)
		{
			Debug::Print(TEXT("222"));
			Layout->RemoveFromParent();
			UISubsystem->UnRegisterCreatedPrimaryLayoutWidget(Layout);
		}
	}*/
	MulticastRPC_EliminateCharacter();
	GetWorldTimerManager().SetTimer(
		ElimTimerHandle,
		this,
		&ABugCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void ABugCharacter::MulticastRPC_EliminateCharacter_Implementation()
{
	bElimmed = true;
	// Disable Input
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
	GetCharacterMovement()->StopMovementImmediately();
	
	// Disable Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PlayDeathMontage(ProjectileImpactPoint);

	int32 NumMaterials = GetMesh()->GetNumMaterials();
	DynamicDissolveInstances.Empty();
	USkinnedAsset* SkinnedAsset = GetMesh()->GetSkinnedAsset();
	
	for (int32 i = 0; i < NumMaterials; ++i)
	{
		FName SlotName = SkinnedAsset->GetMaterials()[i].MaterialSlotName;
		UMaterialInterface** FoundMat = SlotSpecificDissolveMaterials.Find(SlotName);
        
		if (FoundMat && *FoundMat)
		{
			UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(*FoundMat, this);
			GetMesh()->SetMaterial(i, MID);
			DynamicDissolveInstances.Add(MID);
			MID->SetScalarParameterValue(TEXT("Dissolve"), -0.5f);
			MID->SetScalarParameterValue(TEXT("Glow"), 100.f);
		}
	}
	StartDissolve();
	
	
}


void ABugCharacter::ElimTimerFinished()
{
	ACommonGameMode* GameMode = GetWorld()->GetAuthGameMode<ACommonGameMode>();
	if (GameMode)
	{
		if (GetController())
		{
			GameMode->RequestRespawn(this, GetController());
		}
	}
}

void ABugCharacter::PlayFireMontage()
{
	if (!CombatComponent || CombatComponent->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RifleFireMontage)
	{
		AnimInstance->Montage_Play(RifleFireMontage);
	}
	
}

void ABugCharacter::PlayHitReactMontage(const FVector& HitPoint)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		FVector ToHit = HitPoint - GetActorLocation();
		ToHit.Z = 0.f;
		ToHit.Normalize();
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();
		float CosTheta = FVector::DotProduct(Forward, ToHit);
		float CosPhi = FVector::DotProduct(Right, ToHit);
		FName SectionName = FName("FromFront1"); 
		if (CosTheta >= 0.5f) 
		{
			int32 RandomIndex = FMath::RandRange(1, 7); 
			SectionName = FName(*FString::Printf(TEXT("FromFront%d"), RandomIndex));
		}
		else if (CosTheta <= -0.5f)
		{
			SectionName = FName("FromBack");
		}
		else if (CosPhi >= 0.5f)
		{
			SectionName = FName("FromRight");
		}
		else
		{
			SectionName = FName("FromLeft");
		}

		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
		
	}
	
}

void ABugCharacter::PlayDeathMontage(const FVector& HitPoint)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		FVector ToHit = HitPoint - GetActorLocation();
		ToHit.Z = 0.f;
		ToHit.Normalize();
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();
		float CosTheta = FVector::DotProduct(Forward, ToHit);
		float CosPhi = FVector::DotProduct(Right, ToHit);
		FName SectionName = FName("FromFront1"); 
		if (CosTheta >= 0.5f) 
		{
			int32 RandomIndex = FMath::RandRange(1, 3); 
			SectionName = FName(*FString::Printf(TEXT("FromFront%d"), RandomIndex));
		}
		else if (CosTheta <= -0.5f)
		{
			SectionName = FName("FromBack");
		}
		else if (CosPhi >= 0.5f)
		{
			SectionName = FName("FromRight");
		}
		else
		{
			SectionName = FName("FromLeft");
		}
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
		
	}
}

void ABugCharacter::PlayEquipMontage()
{
	if (!CombatComponent || CombatComponent->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RifleEquipMontage)
	{
		FName SectionName;
		switch (CombatComponent->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Max:
			break;
		}
		AnimInstance->Montage_Play(RifleEquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, RifleEquipMontage);
	}
}

void ABugCharacter::PlayReloadMontage()
{
	if (!CombatComponent || CombatComponent->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RifleReloadMontage)
	{
		FName SectionName;
		switch (CombatComponent->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Max:
			break;
		}
		AnimInstance->Montage_Play(RifleReloadMontage);
		AnimInstance->Montage_JumpToSection(SectionName, RifleReloadMontage);
	}
}

void ABugCharacter::OnReloadAnimationFinished()
{
	if (CombatComponent)
	{
		CombatComponent->OnReloadAnimationFinished();
	}
}


// Copyrights @FpsLuping all reserved


#include "Character/BugCharacter.h"

#include "DebugHelper.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "BugDestroyer/BugDestroyer.h"
#include "Camera/CameraComponent.h"
#include "Components/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameMode/CommonGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Weapon.h"
#include "Engine/NetConnection.h"


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
	
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	BuffComponent->SetIsReplicated(true);
	
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
	
	AttachedGrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedGrenade"));
	AttachedGrenadeMesh->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}


void ABugCharacter::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (CombatComponent)
	{
		CombatComponent->PickupAmmo(WeaponType, AmmoAmount);
	}
}

void ABugCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	
	CreateMIDs();
	if (AttachedGrenadeMesh)
	{
		AttachedGrenadeMesh->SetVisibility(false);
	}
	if (IsLocallyControlled())
	{
		GetWorldTimerManager().SetTimer(NetStatTimerHandle, this, &ThisClass::UpdateNetworkStats, 1.0f, true);
	}
	
}


void ABugCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Update net estimated aim yaw on server for client prediction
	if (HasAuthority())
	{
		NetEstimatedAimYaw = FRotator::NormalizeAxis(GetBaseAimRotation().Yaw);
	}
	
	// Hide mesh if camera is too close
	HideIfCameraClose(DeltaTime);
	
	// Debug code - can be removed in final build
	// if (CombatComponent)
	// {
	//     const UEnum* EnumPtr = StaticEnum<ECombatState>();
	//     FString StateString = EnumPtr ? EnumPtr->GetNameStringByValue((int64)CombatComponent->CombatState) : TEXT("Invalid");
	//     FString RolePrefix = CombatComponent->GetOwner()->HasAuthority() ? TEXT("[Server]") : TEXT("[Client]");
	//     FString FinalMessage = FString::Printf(TEXT("%s CombatState: %s"), *RolePrefix, *StateString);
	//     UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalMessage);
	// }

}

void ABugCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ABugCharacter, OverlappingWeapon, COND_OwnerOnly);
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
			if (OverlappingWeapon->GetEquipSound())
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					OverlappingWeapon->GetEquipSound(),
					GetActorLocation()
				);
			}
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
	bSprintButtonPressed = true;
	if (HasAuthority())
	{
		SetMaxWalkSpeed(SprintMaxWalkSpeed);
	}
	else
	{
		RPC_Sprint(true);
	}
}

void ABugCharacter::StopSprint()
{
	bSprintButtonPressed = false;
	if (BuffComponent)
	{
		if (BuffComponent->bSpeedBuffing) return;
	}
	if (HasAuthority())
	{
		SetMaxWalkSpeed(DefaultMaxWalkSpeed);
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

void ABugCharacter::GrenadeButtonPressed()
{
	if (CombatComponent)
	{
		CombatComponent->TossGrenade();
	}
}

void ABugCharacter::SwapButtonPressed()
{
	if (CombatComponent)
	{
		CombatComponent->SwapWeapons();
	}
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

float ABugCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bElimmed) return 0.0f;
	
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	float DamageToHealth = ActualDamage;
	if (GetCurrentShield() > 0.f)
	{
		if (GetCurrentShield() >= ActualDamage)
		{
			SetCurrentShield(FMath::Clamp(GetCurrentShield() - ActualDamage, 0.f, GetMaxShield()));
			DamageToHealth = 0.f;
			OnShieldChanged.Broadcast(GetCurrentShield(), GetMaxShield());
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - GetCurrentShield(), 0.f, ActualDamage);
			SetCurrentShield(0.f);
			OnShieldChanged.Broadcast(GetCurrentShield(), GetMaxShield());
		}
	}
	
	if (HasAuthority())
	{
		if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
		{
			const FRadialDamageEvent* RadialEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
			GetHit(RadialEvent->Origin);
		}
		CurrentHealth = FMath::Clamp(CurrentHealth - DamageToHealth, 0.f, MaxHealth);
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
		if (CurrentHealth <= 0.f)
		{
			ACommonGameMode* GameMode = GetWorld()->GetAuthGameMode<ACommonGameMode>();
			if (GameMode && GetController())
			{
				GameMode->PlayerEliminated(this, GetController(), EventInstigator);
			}
		}
	}

	return ActualDamage;
}

AWeapon* ABugCharacter::GetNearestWeaponInArray()
{

	if (OverlappingWeapons.Num() == 0) return nullptr;

	AWeapon* BestMatch = nullptr;
	float MinDistSq = MAX_FLT;
	FVector Loc = GetActorLocation();

	for (AWeapon* W : OverlappingWeapons)
	{
		if (W)
		{
			float DistSq = FVector::DistSquared(Loc, W->GetActorLocation());
			if (DistSq < MinDistSq)
			{
				MinDistSq = DistSq;
				BestMatch = W;
			}
		}
	}
	return BestMatch;
	
}

void ABugCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (LastWeapon) LastWeapon->ShowPickupWidget(false);
	if (OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true, this);
}


void ABugCharacter::OnRep_Health()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void ABugCharacter::OnRep_Shield()
{
	OnShieldChanged.Broadcast(CurrentShield, MaxShield);
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
	bSprintButtonPressed = bIsSprint;

	if (bIsSprint)
	{
		SetMaxWalkSpeed(SprintMaxWalkSpeed);
	}
	else
	{
		SetMaxWalkSpeed(DefaultMaxWalkSpeed);
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
		if (DeathMontage)
		{
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

		FOnTimelineEvent TimelineFinishedEvent;
		TimelineFinishedEvent.BindDynamic(this, &ABugCharacter::OnDissolveTimelineFinished);
		DissolveTimeline->SetTimelineFinishedFunc(TimelineFinishedEvent);

		DissolveTimeline->Play();
	}
}

void ABugCharacter::OnDissolveTimelineFinished()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	
	if (CombatComponent && CombatComponent->PrimaryWeapon)
	{
		CombatComponent->PrimaryWeapon->UpdateWeaponDither(CurrentDitherAlpha);
	}
}


void ABugCharacter::UpdateNetworkStats()
{
	if (APlayerState* PS = GetPlayerState())
	{
		CurrentPing = PS->GetPingInMilliseconds();
	}
	
	if (UNetDriver* NetDriver = GetWorld()->GetNetDriver())
	{
		UNetConnection* NetConn = NetDriver->ServerConnection; 
		
		if (NetConn)
		{
			float InLoss = NetConn->GetInLossPercentage().GetAvgLossPercentage();
			float OutLoss = NetConn->GetOutLossPercentage().GetAvgLossPercentage();
    
			PacketLossPercentage = FMath::Max(InLoss, OutLoss);
		}
	}
	
	bool bShouldWarn = (CurrentPing > 200.f || PacketLossPercentage > 5.f);
	
	if (bLastNetWarning != bShouldWarn)
	{
		bLastNetWarning = bShouldWarn;
		OnNetWarning.Broadcast(bShouldWarn);
	}
	
}

void ABugCharacter::GetHit(const FVector& HitPoint)
{
	ProjectileImpactPoint = HitPoint;
	PlayHitReactMontage(ProjectileImpactPoint);
	
}


void ABugCharacter::SetOverlappingWeapon(AWeapon* Weapon, bool bIsOverlapping)
{
	// 1. 更新维护本地数组
	if (Weapon)
	{
		if (bIsOverlapping)
		{
			OverlappingWeapons.AddUnique(Weapon);
		}
		else
		{
			// 离开范围时，确保关掉该武器的 UI（预防性）
			if (IsLocallyControlled()) Weapon->ShowPickupWidget(false);
			OverlappingWeapons.Remove(Weapon);
		}
	}

	// 2. 找到当前最新的最近武器
	AWeapon* NewNearest = GetNearestWeaponInArray();

	// 3. UI 切换逻辑 (仅本地客户端执行)
	if (IsLocallyControlled())
	{
		if (NewNearest != OverlappingWeapon)
		{
			if (OverlappingWeapon) 
			{
				OverlappingWeapon->ShowPickupWidget(false);
			}

			if (NewNearest)
			{
				NewNearest->ShowPickupWidget(true, this);
			}
		}
	}
	
	OverlappingWeapon = NewNearest;
}

void ABugCharacter::SetMaxWalkSpeed(float InMaxWalkSpeed)
{
	if (BuffComponent)
	{
		if (BuffComponent->bSpeedBuffing) return;
	}
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
	if (CombatComponent && CombatComponent->PrimaryWeapon)
	{
		return CombatComponent->PrimaryWeapon;
	}
	return nullptr;
}


void ABugCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
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
			EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ABugCharacter::EquipButtonPressed);
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
		if (GrenadeTossAction)
		{
			EnhancedInputComponent->BindAction(GrenadeTossAction, ETriggerEvent::Started, this, &ABugCharacter::GrenadeButtonPressed);
		}
		if (SwapAction)
		{
			EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Started, this, &ABugCharacter::SwapButtonPressed);
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
	if (BuffComponent)
	{
		BuffComponent->BugCharacter = this;
		BuffComponent->SetInitialSpeed(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		BuffComponent->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
}


void ABugCharacter::EliminateCharacter()
{
	if (CombatComponent)
	{
		if (CombatComponent->PrimaryWeapon)
		{
			CombatComponent->PrimaryWeapon->DropWeapon();
			CombatComponent->PrimaryWeapon = nullptr;
		}
		if (CombatComponent->SecondaryWeapon)
		{
			CombatComponent->SecondaryWeapon->DropWeapon();
			CombatComponent->SecondaryWeapon = nullptr;
		}
		
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
	if (GetMesh())
	{
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); 
		GetCharacterMovement()->DisableMovement();
	}
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
	if (!CombatComponent || CombatComponent->PrimaryWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireMontage)
	{
		FName SectionName;
		switch (CombatComponent->PrimaryWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_PlasmaPistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("SMG");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_Max:
			break;
		}
		AnimInstance->Montage_Play(FireMontage);
		AnimInstance->Montage_JumpToSection(SectionName, FireMontage);
	}
	
}

void ABugCharacter::PlayHitReactMontage(const FVector& HitPoint)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage && !bElimmed)
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
		Debug::Print(FString::Printf(TEXT("SectionName: %s"), *SectionName.ToString()));
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
		
	}
}

void ABugCharacter::PlayEquipMontage()
{
	if (!CombatComponent || CombatComponent->PrimaryWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		FName SectionName;
		switch (CombatComponent->PrimaryWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_PlasmaPistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");// 暂时用Pistol替代
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Rifle"); // 暂时用rifle替代
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("Rifle"); // 暂时用rifle替代
			break;
		case EWeaponType::EWT_Max:
			break;
		}
		
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ABugCharacter::PlayReloadMontage()
{
	if (!CombatComponent || CombatComponent->PrimaryWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		FName SectionName;
		switch (CombatComponent->PrimaryWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_PlasmaPistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol"); // 暂时用Pistol替代
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Rifle"); // 暂时用rifle替代
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("Rifle"); // 暂时用rifle替代
			break;
		case EWeaponType::EWT_Max:
			break;
		}
		AnimInstance->Montage_Play(ReloadMontage);
		AnimInstance->Montage_JumpToSection(SectionName, ReloadMontage);
	}
}

void ABugCharacter::PlayGrenadeTossMontage()
{
	if (!CombatComponent || CombatComponent->PrimaryWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GrenadeTossMontage)
	{
		AnimInstance->Montage_Play(GrenadeTossMontage);
	}
}

void ABugCharacter::OnReloadAnimationFinished()
{
	if (CombatComponent)
	{
		CombatComponent->OnReloadAnimationFinished();
	}
}

void ABugCharacter::OnEquipAnimationFinished()
{
	if (CombatComponent)
	{
		CombatComponent->OnEquipAnimationFinished();
	}
}

void ABugCharacter::OnTossGrenadeFinished()
{
	if (CombatComponent)
	{
		CombatComponent->OnTossGrenadeAnimationFinished();
	}
}


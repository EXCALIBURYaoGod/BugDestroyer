// Copyright @FpsLuping all reserved


#include "Components/LagCompensationComponent.h"

#include "BugDestroyer/BugDestroyer.h"
#include "Components/BoxComponent.h"
#include "Character/BugCharacter.h"
#include "Controllers/GameCommonPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULagCompensationComponent::SaveFramePackageHitBoxes(FFramePackage& Package)
{
	if (BugCharacter)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		for (auto& BoxPair : BugCharacter->GetHitBoxes() )
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}

void ULagCompensationComponent::ShowFramePackage(FFramePackage& Package, const FColor& Color)
{
	for (auto& BoxInfo : Package.HitBoxInfo )
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			false,
			4.f
		);
	}
}

void ULagCompensationComponent::CacheBoxPositions(ABugCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->GetHitBoxes() )
	{
		if (HitBoxPair.Value != nullptr)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = HitBoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = HitBoxPair.Value->GetScaledBoxExtent();
			OutFramePackage.HitBoxInfo.Add(HitBoxPair.Key, BoxInformation);
		}
	}
}

void ULagCompensationComponent::MoveBoxes(ABugCharacter* HitCharacter, const FFramePackage& InMovePackage)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->GetHitBoxes() )
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(InMovePackage.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(InMovePackage.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(InMovePackage.HitBoxInfo[HitBoxPair.Key].BoxExtent);
		}
	}
}

void ULagCompensationComponent::SaveFramePackage()
{
	if (BugCharacter == nullptr || !BugCharacter->HasAuthority()) return;
	if (FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackageHitBoxes(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		FFramePackage ThisFrame;
		SaveFramePackageHitBoxes(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
		
	//	ShowFramePackage(ThisFrame, FColor::Red);
	}
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SaveFramePackage();
	
}

FFramePackage ULagCompensationComponent::GetFramePackageByTime(ABugCharacter* HitCharacter, float HitTime)
{

	FFramePackage FrameToCheck;

	if (AGameCommonPlayerController* PC = Cast<AGameCommonPlayerController>(HitCharacter->GetController()))
	{
		HitTime = HitTime + PC->GetClientServerDelta();
		bool bShouldInterpolate = true;
		const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensationComponent()->FrameHistory;
		const float OldestHistoryTime = History.GetTail()->GetValue().Time;
		const float NewestHistoryTime = History.GetHead()->GetValue().Time;
		if (OldestHistoryTime > HitTime)
		{
			FServerSideRewindResult();
			return FrameToCheck;
		}
		if (OldestHistoryTime == HitTime)
		{
			FrameToCheck = History.GetTail()->GetValue();
		}
		if (NewestHistoryTime <= HitTime)
		{
			FrameToCheck = History.GetHead()->GetValue();
		}
		TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
		TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;
	 
		while (Older->GetValue().Time > HitTime) // OlderTime <= HitTime < YoungerTime
		{
			if (Older->GetNextNode() == nullptr) break;
			Older = Older->GetNextNode();
			if (Older->GetValue().Time > HitTime)
			{
				Younger = Older;
			}
		}
		if (Older->GetValue().Time == HitTime)
		{
			FrameToCheck = Older->GetValue();
			bShouldInterpolate = false;
		}
		if (bShouldInterpolate)
		{
			FrameToCheck = InterpBetweenTwoFrames(Older->GetValue(), Younger->GetValue(), HitTime);
		}
	}
	return FrameToCheck;
}

FServerSideRewindResult ULagCompensationComponent::ConfirmLineTraceHit(const FFramePackage& FramePackageToCheck,
	ABugCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if (HitCharacter == nullptr) return FServerSideRewindResult();
	
	FFramePackage CurrentFramePackage;
	CacheBoxPositions(HitCharacter, CurrentFramePackage);
	MoveBoxes(HitCharacter, FramePackageToCheck);
	
	for (auto& BoxPair : HitCharacter->GetHitBoxes())
	{
		if (BoxPair.Value)
		{
			BoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			BoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
		}
	}
	
	FHitResult ConfirmHitResult;
	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	UWorld* World = GetWorld();
    
	FServerSideRewindResult Result;

	if (World)
	{
		World->LineTraceSingleByChannel(
		   ConfirmHitResult,
		   TraceStart,
		   TraceEnd,
		   ECC_HitBox
		);

		if (ConfirmHitResult.bBlockingHit)
		{
			Result.HitResult = ConfirmHitResult;
		}
	}
	
	MoveBoxes(HitCharacter, CurrentFramePackage);

	return Result;
}

FServerSideRewindResult ULagCompensationComponent::ConfirmProjectileHit(const FFramePackage& FramePackageToCheck,
	ABugCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& InitialVelocity)
{
	if (HitCharacter == nullptr) return FServerSideRewindResult();
	
	FFramePackage CurrentFramePackage;
	CacheBoxPositions(HitCharacter, CurrentFramePackage);
	MoveBoxes(HitCharacter, FramePackageToCheck);
	
	for (auto& BoxPair : HitCharacter->GetHitBoxes())
	{
		if (BoxPair.Value)
		{
			BoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			BoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
		}
	}
	
	// Predict ProjectilePath
	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = TraceStart;
	PredictParams.LaunchVelocity = InitialVelocity; 
	PredictParams.ProjectileRadius = 5.0f;
	PredictParams.bTraceWithCollision = true;
	PredictParams.MaxSimTime = 3.0f;
	PredictParams.TraceChannel = ECC_HitBox;
	FPredictProjectilePathResult PredictResult;
	bool bHitHitBox = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
	// Predict ProjectilePath
	FServerSideRewindResult Result;
	if (bHitHitBox)
	{
		Result.HitResult = PredictResult.HitResult;
	}
	MoveBoxes(HitCharacter, CurrentFramePackage);

	return Result;
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind_LineTrace(ABugCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
                                                                              const FVector_NetQuantize& HitLocation, float HitTime)
{
	bool bReturn = 
	HitCharacter == nullptr 
	|| HitCharacter->GetLagCompensationComponent() == nullptr
	|| HitCharacter->GetLagCompensationComponent()->FrameHistory.GetHead() == nullptr
	|| HitCharacter->GetLagCompensationComponent()->FrameHistory.GetTail() == nullptr;
	if (bReturn)
	{
		return FServerSideRewindResult();
	}
	FFramePackage FrameToCheck = GetFramePackageByTime(HitCharacter, HitTime);
	return ConfirmLineTraceHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
	
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind_Projectile(ABugCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& InitialVelocity, float HitTime)
{
	bool bReturn = 
		HitCharacter == nullptr 
		|| HitCharacter->GetLagCompensationComponent() == nullptr
		|| HitCharacter->GetLagCompensationComponent()->FrameHistory.GetHead() == nullptr
		|| HitCharacter->GetLagCompensationComponent()->FrameHistory.GetTail() == nullptr;
	if (bReturn)
	{
		return FServerSideRewindResult();
	}
	FFramePackage FrameToCheck = GetFramePackageByTime(HitCharacter, HitTime);
	return ConfirmProjectileHit(FrameToCheck, HitCharacter, TraceStart, InitialVelocity);
}


FFramePackage ULagCompensationComponent::InterpBetweenTwoFrames(const FFramePackage& OlderFrame,
                                                                const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFraction = FMath::Clamp((HitTime - OlderFrame.Time) / Distance, 0.0f, 1.0f);
    
	FFramePackage InterpFrame;
	InterpFrame.Time = HitTime;

	for (auto& YoungerBoxPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerBoxPair.Key;

		if (!OlderFrame.HitBoxInfo.Contains(BoxInfoName)) continue;
		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YoungerBoxPair.Value;
		FBoxInformation InterpBoxInfo;
		InterpBoxInfo.Location = FMath::Lerp(OlderBox.Location, YoungerBox.Location, InterpFraction);
		FQuat OlderQuat = OlderBox.Rotation.Quaternion();
		FQuat YoungerQuat = YoungerBox.Rotation.Quaternion();
		InterpBoxInfo.Rotation = FQuat::Slerp(OlderQuat, YoungerQuat, InterpFraction).Rotator();
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;
       
		InterpFrame.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}
	return InterpFrame;
}

// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "BugTypes/BugStructTypes.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector Location = FVector::ZeroVector;
	UPROPERTY()
	FRotator Rotation = FRotator::ZeroRotator;
	UPROPERTY()
	FVector BoxExtent = FVector::ZeroVector;
	
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()
	
	UPROPERTY()
	float Time = 0.f;
	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;
	
};


/**
 * 延迟补偿组件
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BUGDESTROYER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULagCompensationComponent();
	friend class ABugCharacter;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FFramePackage GetFramePackageByTime(ABugCharacter* HitCharacter, float HitTime);
	FServerSideRewindResult ServerSideRewind_LineTrace(
		ABugCharacter* HitCharacter, 
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime
		);
	FServerSideRewindResult ServerSideRewind_Projectile(ABugCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize& InitialVelocity,
		float HitTime
		);
	
protected:
	virtual void BeginPlay() override;
	
	//== ServerSide Rewind ==//
	void SaveFramePackageHitBoxes(FFramePackage& Package);
	void ShowFramePackage(FFramePackage& Package, const FColor& Color);
	void CacheBoxPositions(ABugCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(ABugCharacter* HitCharacter, const FFramePackage& InMovePackage);
	FServerSideRewindResult ConfirmLineTraceHit(const FFramePackage& FramePackageToCheck, 
		ABugCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart, 
		const FVector_NetQuantize& HitLocation);
	FServerSideRewindResult ConfirmProjectileHit(const FFramePackage& FramePackageToCheck, 
	ABugCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, 
	const FVector_NetQuantize& InitialVelocity);
	void SaveFramePackage();
	FFramePackage InterpBetweenTwoFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	//== ServerSide Rewind ==//
	
private:
	UPROPERTY()
	ABugCharacter* BugCharacter;
	
	//== ServerSide Rewind ==//
	TDoubleLinkedList<FFramePackage> FrameHistory;
	UPROPERTY(EditAnywhere, Category="Server Rewind")
	float MaxRecordTime = 4.f;
	//== ServerSide Rewind ==//
	
public:

};

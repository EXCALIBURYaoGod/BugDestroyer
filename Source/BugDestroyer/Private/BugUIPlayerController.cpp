// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/BugUIPlayerController.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

void ABugUIPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this,
		ACameraActor::StaticClass(), FName("Default"), FoundCameras);
	if (!FoundCameras.IsEmpty())
	{
		SetViewTarget(FoundCameras[0]);
	}
}

void ABugUIPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	if (IsLocalPlayerController())
	{
		// 触发蓝图事件
		K2_OnAcknowledgePossession(P);
	}
}

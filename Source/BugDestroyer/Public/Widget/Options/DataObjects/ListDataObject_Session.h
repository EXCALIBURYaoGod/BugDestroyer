// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Base.h"
#include "Data/SessionInfo.h"
#include "ListDataObject_Session.generated.h"

/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UListDataObject_Session : public UListDataObject_Base
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	FSessionInfo SessionInfo;
	
protected:
	//begin UListDataObject_Base Interface
	virtual void OnDataObjectInitialized() override;
	//end UListDataObject_Base Interface
	
private:
	
};

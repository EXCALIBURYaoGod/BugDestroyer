// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Base.h"
#include "ListDataObjcet_Collection.generated.h"

/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UListDataObject_Collection : public UListDataObject_Base
{
	GENERATED_BODY()
	
public:
	void AddChildListData(UListDataObject_Base* InChildListData);
	
	// begin UListDataObject_Base Interface
	virtual TArray<UListDataObject_Base*> GetChildListDataObjects() const override;
	virtual bool HasAnyChildListDataObjects() const override;
	//end UListDataObject_Base Interface
	
private:
	
	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ChildListDataArray;
	
};

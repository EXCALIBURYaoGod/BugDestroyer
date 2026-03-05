// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"
#include "Widget_ListEntry_Scalar.generated.h"

class UListDataObject_Scalar;
class UAnalogSlider;
class UCommonNumericTextBlock;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_ListEntry_Scalar : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	//begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//end UUserWidget Interface
	
	// begin UWidget_ListEntry_Base interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningListDataObject, 
		EOptionsListDataModifyReason ModifyReason) override;
	// end UWidget_ListEntry_Base interface
	
private:
	UFUNCTION()
	void OnSliderValueChanged(float InValue);
	
	UFUNCTION()
	void OnSliderMouseCaptureBegin();
	
	//*** bind widgets ***//
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonNumericTextBlock* CommonNumeric_SettingValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnalogSlider* AnalogSlider_SettingSlider;
	//*** bind widgets ***//
	
	UPROPERTY(Transient)
	UListDataObject_Scalar* CachedOwningScalarDataObject;
	
};

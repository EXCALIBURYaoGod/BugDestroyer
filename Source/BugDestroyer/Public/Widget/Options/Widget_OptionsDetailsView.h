// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widget_OptionsDetailsView.generated.h"

class UListDataObject_Base;
class UCommonRichTextBlock;
class UCommonLazyImage;
class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_OptionsDetailsView : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateDetailsViewInfo(UListDataObject_Base* InDataObject, 
		const FString& InEntryWidgetClassName = FString());
	void ClearDetailsViewInfo();
	
protected:
	//begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//end UUserWidget Interface
	
private:
	//*** bound widgets ***//
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_Title;
	UPROPERTY(meta = (BindWidget))
	UCommonLazyImage* CommonLazyImage_DescriptionImage;
	UPROPERTY(meta = (BindWidget))
	UCommonRichTextBlock* CommonRichTextBlock_Description;
	UPROPERTY(meta = (BindWidget))
	UCommonRichTextBlock* CommonRichTextBlock_DynamicDetails;
	UPROPERTY(meta = (BindWidget))
	UCommonRichTextBlock* CommonRichTextBlock_DisabledReason;
	//*** bound widgets ***//
	
};

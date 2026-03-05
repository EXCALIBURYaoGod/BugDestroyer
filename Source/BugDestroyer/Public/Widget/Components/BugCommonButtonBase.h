// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BugCommonButtonBase.generated.h"

class UCommonLazyImage;
class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UBugCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText InButtonText);
	
	UFUNCTION(BlueprintCallable)
	FText GetButtonDisplayText();
	
	UFUNCTION(BlueprintCallable)
	void SetButtonDisplayImage(const FSlateBrush& InButtonImage);
	
private:
	
	// begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	// end UUserWidget Interface
	
	// begin UCommonButtonBase Interface
	virtual void NativeOnCurrentTextStyleChanged() override;
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;
	// end UCommonButtonBase Interface
	
	// bound widgets
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonTextBlock* CommonTextBlock_ButtonText;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UCommonLazyImage* CommonLazyImage_ButtonImage;
	// bound widgets
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bug Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDisplayText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bug Button", meta = (AllowPrivateAccess = "true"))
	bool bUseUpperCaseForButtonText = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bug Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDescriptionText;
	
};

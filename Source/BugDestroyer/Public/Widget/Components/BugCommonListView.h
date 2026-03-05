// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "BugCommonListView.generated.h"

class UDataAsset_DataListEntryMapping;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UBugCommonListView : public UCommonListView
{
	GENERATED_BODY()
	
protected:
	// begin UCommonListView interface
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;
	virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;
	// end UCommonListView interface

	
private:
	// begin UWidget Interface
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	// End UWidget Interface
	
	UPROPERTY(EditAnywhere, Category= "Bug ListView Settings")
	UDataAsset_DataListEntryMapping* DataListEntryMapping;
	
	
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Components/BugTabListWidgetBase.h"
#include "Widget/Components/BugCommonButtonBase.h"
#include "Editor/WidgetCompilerLog.h"


void UBugTabListWidgetBase::RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName)
{
	RegisterTab(InTabID, TabButtonEntryWidgetClass, nullptr);
	
	 if (UBugCommonButtonBase* FoundButton = Cast<UBugCommonButtonBase>(GetTabButtonBaseByID(InTabID)))
	 {
		 FoundButton->SetButtonText(InTabDisplayName);
	 }
}

#if WITH_EDITOR
void UBugTabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(TEXT("TabButtonEntryWidgetClass has no valid entry specified.") + 
			GetClass()->GetName() + 
			TEXT("Needs a valid entry widget class to function properly")
		));
	}
}
#endif
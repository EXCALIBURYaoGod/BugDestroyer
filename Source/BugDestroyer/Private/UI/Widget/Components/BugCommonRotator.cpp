// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Components/BugCommonRotator.h"

#include "CommonTextBlock.h"

void UBugCommonRotator::SetSelectedOptionByText(const FText& SelectedOptionText)
{
	const int32 FoundIndex = TextLabels.IndexOfByPredicate(
		[SelectedOptionText](const FText& TextItem)->bool
		{
			return TextItem.EqualTo(SelectedOptionText);
		}
	);

	if (FoundIndex != INDEX_NONE)
	{
		SetSelectedItem(FoundIndex);
	}
	else
	{
		MyText->SetText(SelectedOptionText);
	}
}

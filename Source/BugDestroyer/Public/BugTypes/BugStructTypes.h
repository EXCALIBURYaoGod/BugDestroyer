#pragma once

#include "BugStructTypes.generated.h"

USTRUCT()
struct FOptionsDataEditConditionDescriptor
{
	GENERATED_BODY()
	
public:
	void SetEditConditionFunc(TFunction<bool()> InEditConditionFunc)
	{
		EditConditionFunction = InEditConditionFunc;
	}
	
	bool CheckEditConditionFuncIsValid() const
	{
		return EditConditionFunction != nullptr;
	}
	
	
	bool IsEditConditionMet() const
	{
		if (CheckEditConditionFuncIsValid())
		{
			return EditConditionFunction();
		}
		
		return true;
	}
	
	FString GetDisabledRichReason() const
	{
		return DisabledRichReason;
	}
	
	void SetDisabledRichReason(const FString& InDisabledRichReason)
	{
		DisabledRichReason = InDisabledRichReason;
	}
	
	bool HasForcedStringValue() const
	{
		return DisabledForcedStringValue.IsSet();
	}
	
	FString GetDisabledForcedStringValue() const
	{
		return DisabledForcedStringValue.GetValue();
	}
	
	void SetDisabledForcedStringValue(const FString& InDisabledForcedStringValue)
	{
		DisabledForcedStringValue = InDisabledForcedStringValue;
	}
	
private:
	TFunction<bool()> EditConditionFunction;
	FString DisabledRichReason;
	TOptional<FString> DisabledForcedStringValue;
	
};

USTRUCT(BlueprintType)
struct FCrosshairSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); // Orange

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Thickness = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Size = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gap = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTextureMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairTexture = nullptr;
};
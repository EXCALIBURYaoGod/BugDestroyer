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

USTRUCT(BlueprintType)
struct FImpactEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UParticleSystem* Particles;

	UPROPERTY(EditAnywhere)
	USoundBase* Sound;
	
	UPROPERTY(EditAnywhere)
	float ImpactDamage;
	
	FImpactEffectData()
		: Particles(nullptr)
		, Sound(nullptr)
		, ImpactDamage(0.0f)
	{
	}
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FHitResult HitResult;
	
};

class UTexture2D;

/**
 * 武器 UI 数据表结构
 * 必须继承自 FTableRowBase
 */
USTRUCT(BlueprintType)
struct FWeaponUIData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 击杀提示右上角的图标 (强推软引用，配合 CommonLazyImage 异步加载)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon UI")
	TSoftObjectPtr<UTexture2D> KillFeedIcon;

	// 💡 扩展思路：未来你还可以把武器的本地化名字、准星样式也加在这里
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon UI")
	// FText WeaponDisplayName;
    
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon UI")
	// TSoftClassPtr<UUserWidget> CrosshairWidgetClass;
};
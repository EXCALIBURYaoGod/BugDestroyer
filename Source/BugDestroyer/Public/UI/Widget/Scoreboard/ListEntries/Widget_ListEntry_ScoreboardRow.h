// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widget_ListEntry_ScoreboardRow.generated.h"

class UCommonTextBlock;
class UListDataObject_ScoreboardEntry;
class UTextBlock;
class UBorder;

/**
 * 计分板单行 UI
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_ListEntry_ScoreboardRow : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	// 开放给蓝图或者内部调用的刷新函数
	UFUNCTION(BlueprintCallable, Category = "Scoreboard")
	void RefreshRowUI();
	
protected:
	// ==========================================
	// 实现 IUserObjectListEntry 接口：当 ListView 给这行分配数据时触发
	// ==========================================
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	

	// 缓存的数据对象
	UPROPERTY(Transient)
	UListDataObject_ScoreboardEntry* CachedScoreData;

	// ==========================================
	// 绑定的 UI 控件
	// ==========================================
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Border_SelfHighlight;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_PlayerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_Kills;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_Deaths;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* CommonTextBlock_Ping;
	
};
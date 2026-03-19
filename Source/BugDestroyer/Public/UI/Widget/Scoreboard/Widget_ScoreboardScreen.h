// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget/Widget_ActivatableBase.h" // 使用你已有的 CommonUI 基类
#include "Widget_ScoreboardScreen.generated.h"

class UCommonListView;
class UListDataObject_ScoreboardEntry;

UCLASS(Abstract, BlueprintType)
class BUGDESTROYER_API UWidget_ScoreboardScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
public:

	
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	// 每隔一段时间刷新一次数据 (比如 0.5秒)
	void RefreshScoreboard();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonListView* ListView_Scoreboard;

private:
	FTimerHandle RefreshTimerHandle;

	// 缓存当前已创建的数据对象，防止每帧都在 NewObject
	UPROPERTY()
	TMap<APlayerState*, UListDataObject_ScoreboardEntry*> ScoreboardDataMap;
	
};

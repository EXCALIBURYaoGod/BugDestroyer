// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget/Widget_ActivatableBase.h"
#include "Widget_GameMenuScreen.generated.h"

class UCommonButtonBase;

/**
 * 游戏内按 Esc 呼出的菜单界面 (Screen层级)
 */
UCLASS()
class BUGDESTROYER_API UWidget_GameMenuScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	// begin UCommonActivatableWidget Interface
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
    
	// 配置 Common UI 的输入路由，拦截游戏输入并显示鼠标
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	// end UCommonActivatableWidget Interface

	// 绑定蓝图中的 CommonButton
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonButtonBase* Button_Resume;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonButtonBase* Button_Options;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonButtonBase* Button_LeaveMatch;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonButtonBase* Button_Quit;

	// 按钮回调
	UFUNCTION()
	void OnResumeClicked();
	UFUNCTION()
	void OnOptionsClicked();
	UFUNCTION()
	void OnLeaveMatchClicked();
	UFUNCTION()
	void OnQuitClicked();
	
};
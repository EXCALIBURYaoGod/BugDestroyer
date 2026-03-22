// //Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "Widget_LobbyScreen.generated.h"

class ALobbyGameState;
class UBugCommonButtonBase;
class UCommonTextBlock;
/**
 * 大厅UI界面，直接在关卡中通过Lobbyplayercontroller创建
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_LobbyScreen : public UWidget_ActivatableBase
{
	
	GENERATED_BODY()

protected:
	// begin UCommonActivatableWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnHandleBackAction() override;
	// end UCommonActivatableWidget Interface
	
	// 响应人数变化
	UFUNCTION()
	void UpdatePlayerCount(int32 NewCount);

	// 响应按钮点击
	UFUNCTION(BlueprintCallable)
	void OnStartGameClicked();
	
private:
	// === UI 组件绑定 (BindWidget) ===
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_PlayerCount;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UBugCommonButtonBase* CommonButton_StartGame;
	// === UI 组件绑定 (BindWidget) ===
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"), Category= "BackAction")
	FDataTableRowHandle CustomBackDataTableRowHandle;
	
	// 缓存 GameState 指针
	TWeakObjectPtr<ALobbyGameState> LobbyGameState;
	FTimerHandle WaitGameStateTimer;
	void TryBindGameState();
	
};

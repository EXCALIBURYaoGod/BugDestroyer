// Copyright @FpsLuping all reserved


#include "Widget/Widget_CharacterScreen.h"

#include "DebugHelper.h"
#include "Character/BugCharacter.h"
#include "GameState/CommonGamePlayerState.h"
#include "GameState/CommonGameState.h"
#include "Subsystems/BugUISubsystem.h"
#include "Weapons/Weapon.h"

void UWidget_CharacterScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	APlayerController* PC = GetOwningPlayerController();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

}

void UWidget_CharacterScreen::SetupGameStateBindings(ACommonGameState* GS)
{
	GS->OnMatchTimeUpdated.AddUniqueDynamic(this, &UWidget_CharacterScreen::OnMatchTimeChangedCallback);
	int32 M, S;
	int32 MatchTime = GS->GetMatchTime();
	M = MatchTime / 60;
	S = MatchTime % 60;
	OnMatchTimeChangedCallback(M, S);
}

void UWidget_CharacterScreen::NativeConstruct()
{
	Super::NativeConstruct();
	if (UBugUISubsystem* UISubsystem = UBugUISubsystem::Get(GetOwningPlayer()))
	{
		UISubsystem->OnPawnResubscribedDelegate.AddUniqueDynamic(this, &ThisClass::OnHandlePawnResubscribed);
	}
	if (ACommonGamePlayerState* CommonGamePlayerState = GetOwningPlayerState<ACommonGamePlayerState>())
	{
		CommonGamePlayerState->OnScoreChanged.AddUniqueDynamic(this, &ThisClass::OnScoreChangedCallback);
		OnScoreChangedCallback(CommonGamePlayerState->GetScore());
		CommonGamePlayerState->OnDefeatsChanged.AddUniqueDynamic(this, &ThisClass::OnDefeatsChangedCallback);
		OnDefeatsChangedCallback(CommonGamePlayerState->GetDefeats());
	}
	if (ACommonGameState* GS = GetWorld()->GetGameState<ACommonGameState>())
	{
		SetupGameStateBindings(GS);
	}
	else
	{
		GetWorld()->GameStateSetEvent.AddUObject(this, &ThisClass::OnGameStateSet);
	}
}

void UWidget_CharacterScreen::OnHandlePawnResubscribed(APawn* NewPawn)
{
	if (ABugCharacter* BugChar = Cast<ABugCharacter>(NewPawn))
	{
		BugChar->OnHealthChanged.AddUniqueDynamic(this, &ThisClass::OnHealthChangedCallback);
		OnHealthChangedCallback(BugChar->GetCurrentHealth(), BugChar->GetMaxHealth());
		BugChar->OnWeaponChanged.AddUniqueDynamic(this, &ThisClass::OnWeaponChangedCallback);
		BP_SetAmmoHUDVisibility(false);
	}
	
}

void UWidget_CharacterScreen::OnHealthChangedCallback(float NewHealth, float MaxHealth)
{
	BP_OnHealthUpdated(NewHealth, MaxHealth);
}

void UWidget_CharacterScreen::OnScoreChangedCallback(float InNewScore)
{
	BP_OnScoreUpdated(InNewScore);
}

void UWidget_CharacterScreen::OnDefeatsChangedCallback(int32 InNewDefeats)
{
	BP_OnDefeatsUpdated(InNewDefeats);
}

void UWidget_CharacterScreen::OnAmmoChangedCallback(int32 InCurrentAmmo, int32 InMagCapacity, int32 InAmmoLeft)
{
	BP_OnAmmoUpdated(InCurrentAmmo, InMagCapacity, InAmmoLeft);
}

void UWidget_CharacterScreen::OnWeaponChangedCallback(class AWeapon* NewWeapon)
{
	if (NewWeapon)
	{
		if (CachedEquippedWeapon)
		{
			CachedEquippedWeapon->OnAmmoChanged.RemoveDynamic(this, &ThisClass::OnAmmoChangedCallback);
		}
		CachedEquippedWeapon = NewWeapon;
		CachedEquippedWeapon->OnAmmoChanged.AddUniqueDynamic(this, &ThisClass::OnAmmoChangedCallback);
		ABugCharacter* BugChar = GetOwningCharacter();
		if (BugChar)
		{
			BugChar->OnAmmoLeftChanged.AddUniqueDynamic(this, &ThisClass::OnAmmoChangedCallback);
		}
		int32 AmmoLeft = GetOwningCharacter()->GetAmmoLeft();
		OnAmmoChangedCallback(CachedEquippedWeapon->GetCurrentAmmo(), CachedEquippedWeapon->GetMagCapacity(), AmmoLeft);
        BP_SetAmmoHUDVisibility(true);
	}
	else
	{
		CachedEquippedWeapon->OnAmmoChanged.Clear();
		CachedEquippedWeapon = nullptr;
		BP_SetAmmoHUDVisibility(false);
	}
}

void UWidget_CharacterScreen::OnAmmoLeftChangedCallback(int32 InCurrentAmmo, int32 InMagCapacity, int32 InAmmoLeft)
{
	BP_OnAmmoLeftUpdated(InCurrentAmmo, InMagCapacity, InAmmoLeft);
}

void UWidget_CharacterScreen::OnMatchTimeChangedCallback(int32 InNewMinutes, int32 InNewSeconds)
{
	BP_OnMatchTimeUpdated(InNewMinutes, InNewSeconds);
}


void UWidget_CharacterScreen::OnGameStateSet(AGameStateBase* GameStateBase)
{
	GetWorld()->GameStateSetEvent.RemoveAll(this);
	if (ACommonGameState* GS = Cast<ACommonGameState>(GameStateBase))
	{
		SetupGameStateBindings(GS);
	}
}

ABugCharacter* UWidget_CharacterScreen::GetOwningCharacter()
{
	APlayerController* PC = GetOwningPlayerController();
	if (PC)
	{
		if (ABugCharacter* BugCharacter = Cast<ABugCharacter>(PC->GetPawn()))
		{
			return BugCharacter;
		}
	}
	return nullptr;
}


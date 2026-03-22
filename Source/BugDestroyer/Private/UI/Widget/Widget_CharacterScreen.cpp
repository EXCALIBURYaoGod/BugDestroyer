// Copyright @FpsLuping all reserved


#include "Widget/Widget_CharacterScreen.h"

#include "Character/BugCharacter.h"
#include "GameState/CommonGamePlayerState.h"
#include "GameState/CommonGameState.h"
#include "UI/Subsystems/BugUISubsystem.h"
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
	GS->OnKillMessageBroadcast.AddUniqueDynamic(this, &UWidget_CharacterScreen::OnKillMessageReceivedCallback);
	GS->OnTeamScoreUpdated.AddUniqueDynamic(this, &UWidget_CharacterScreen::OnTeamScoreChangedCallback);
	CachedPlayerState = Cast<ACommonGamePlayerState>(GetOwningPlayerState());
	int32 OurTeamScore = 0;
	int32 EnemyTeamScore = 0;
	if (CachedPlayerState)
	{
		if (CachedPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			OurTeamScore = GS->RedTeamScore;
			EnemyTeamScore = GS->BlueTeamScore;
		}
		else if (CachedPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			OurTeamScore = GS->BlueTeamScore;
			EnemyTeamScore = GS->RedTeamScore;
		}
	}
	OnTeamScoreChangedCallback(OurTeamScore, EnemyTeamScore);
	int32 M, S;
	int32 MatchTime = GS->GetMatchTime();
	M = MatchTime / 60;
	S = MatchTime % 60;
	OnMatchTimeChangedCallback(M, S);
}

void UWidget_CharacterScreen::OnKillMessageReceivedCallback(const FString& KillerName, const FString& VictimName,
	const FString& AttackWeaponName, bool bIsHeadshot)
{
	TSoftObjectPtr<UTexture2D> FoundWeaponIcon = nullptr;
	bool bGeneric = (AttackWeaponName == TEXT("Generic"));
	if (WeaponUIDataTable)
	{
		static const FString ContextString(TEXT("KillFeed Weapon Icon Lookup"));
		
		FWeaponUIData* Row = WeaponUIDataTable->FindRow<FWeaponUIData>(FName(*AttackWeaponName), ContextString, true);
		if (Row)
		{
			FoundWeaponIcon = Row->KillFeedIcon;
		}
		else
		{
			FWeaponUIData* GenericRow = WeaponUIDataTable->FindRow<FWeaponUIData>(FName("Generic"), ContextString, true);
			if (GenericRow)
			{
				FoundWeaponIcon = GenericRow->KillFeedIcon;
				bGeneric = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("KillFeed: Failed to find 'Generic' fallback row in WeaponUIDataTable!"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("KillFeed: WeaponUIDataTable is NULL! Please assign it in WBP_CharacterScreen."));
	}
	BP_OnKillMessageReceived(KillerName, VictimName, FoundWeaponIcon, bIsHeadshot, bGeneric);
}

void UWidget_CharacterScreen::NativeConstruct()
{
	Super::NativeConstruct();
	if (UBugUISubsystem* UISubsystem = UBugUISubsystem::Get(GetOwningPlayer()))
	{
		UISubsystem->OnPawnResubscribedDelegate.AddUniqueDynamic(this, &ThisClass::OnHandlePawnResubscribed);
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
		
		BugChar->OnShieldChanged.AddUniqueDynamic(this, &ThisClass::OnShieldChangedCallback);
		OnShieldChangedCallback(BugChar->GetCurrentShield(), BugChar->GetMaxShield());
		BugChar->OnWeaponChanged.AddUniqueDynamic(this, &ThisClass::OnWeaponChangedCallback);
		BP_SetAmmoHUDVisibility(false);
		
		BugChar->OnGrenadeAmountChanged.AddUniqueDynamic(this, &ThisClass::OnGrenadeAmountChangedCallback);
		OnGrenadeAmountChangedCallback(BugChar->GetGrenadeAmount());
		BP_SetGrenadeHUDVisibility(false);
		BugChar->OnNetWarning.AddUniqueDynamic(this, &ThisClass::OnNetWarningCallback);
		OnNetWarningCallback(false);
	}
	
}

void UWidget_CharacterScreen::OnHealthChangedCallback(float NewHealth, float MaxHealth)
{
	BP_OnHealthUpdated(NewHealth, MaxHealth);
}

void UWidget_CharacterScreen::OnShieldChangedCallback(float NewShield, float MaxShield)
{
	BP_OnShieldUpdated(NewShield, MaxShield);
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
		BP_SetGrenadeHUDVisibility(true);
	}
	else
	{
		if (CachedEquippedWeapon)
		{
			CachedEquippedWeapon->OnAmmoChanged.Clear();
            CachedEquippedWeapon = nullptr;
		}
		BP_SetAmmoHUDVisibility(false);
		BP_SetGrenadeHUDVisibility(false);
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

void UWidget_CharacterScreen::OnTeamScoreChangedCallback(int32 InRedTeamScore, int32 InBlueTeamScore)
{
	int32 OurTeamScore = 0;
	int32 EnemyTeamScore = 0;
	if (CachedPlayerState)
	{
		if (CachedPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			OurTeamScore = InRedTeamScore;
			EnemyTeamScore = InBlueTeamScore;
		}
		else if (CachedPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			OurTeamScore = InBlueTeamScore;
			EnemyTeamScore = InRedTeamScore;
		}
	}
	BP_OnTeamScoreUpdated(OurTeamScore, EnemyTeamScore);
}

void UWidget_CharacterScreen::OnGrenadeAmountChangedCallback(int32 InCurrentGrenadeAmount)
{
	BP_OnGrenadeAmountUpdated(InCurrentGrenadeAmount);
}


void UWidget_CharacterScreen::OnNetWarningCallback(bool bIsVisible)
{
	BP_SetNetWarningHUDVisibility(bIsVisible);
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


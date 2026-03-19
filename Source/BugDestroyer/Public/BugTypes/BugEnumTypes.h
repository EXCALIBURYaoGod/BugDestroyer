#pragma once

UENUM(Blueprintable)
enum class EConfirmScreenType : uint8
{
	Ok,
	YesNo,
	OkCancel,
	Unknown UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EConfirmScreenButtonType : uint8
{
	Cancelled,
	Closed,
	Unknown UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EOptionsListDataModifyReason : uint8
{
	DirectlyModified,
	DependencyModified,
	ResetToDefault
};

UENUM(Blueprintable)
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(DisplayName = "Turning Left"),
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_BackLeft UMETA(DisplayName = "Turning Back Left"),   
	ETIP_BackRight UMETA(DisplayName = "Turning Back Right"), 
	ETIP_Center UMETA(DisplayName = "Center"),
	
	ETIP_MAX UMETA(DisplayName = "Max"),
};

UENUM(Blueprintable)
enum class ETeam : uint8
{
	ET_RedTeam UMETA(DisplayName = "Red Team"),
	ET_BlueTeam UMETA(DisplayName = "Blue Team"),
	ET_NoneTeam UMETA(DisplayName = "None Team"),
	
	ET_Max UMETA(DisplayName = "Max")
};
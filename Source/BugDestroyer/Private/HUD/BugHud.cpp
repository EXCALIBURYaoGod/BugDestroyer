// Copyright @FpsLuping all reserved


#include "HUD/BugHud.h"

#include "BugTypes/BugStructTypes.h"
#include "Engine/Canvas.h"


void ABugHud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ABugHud::DrawCrosshair()
{
	if (!Canvas) return;
	if (bDrawCrosshair)
	{
		const FVector2D Center(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);
		const float ActiveGap = FMath::Max(0.f, Config.Gap + CurrentSpread);
		if (Config.bIsTextureMode && Config.CrosshairTexture)
		{
			float TextureWidth = Config.CrosshairTexture->GetSizeX();

			DrawTexture(
				Config.CrosshairTexture,             
		Center.X - (Config.Size * 0.5f),     
		Center.Y - (Config.Size * 0.5f),   
		Config.CrosshairTexture->GetSizeX(),                         
		Config.CrosshairTexture->GetSizeY(),                        
		0.f, 0.f,                            
		1.f, 1.f,                         
				Config.Color
			);
		}
		else
		{
			float HalfThickness = Config.Thickness * 0.5f;
			DrawRect(Config.Color, Center.X - HalfThickness, Center.Y - ActiveGap - Config.Size, Config.Thickness, Config.Size);
			DrawRect(Config.Color, Center.X - HalfThickness, Center.Y + ActiveGap, Config.Thickness, Config.Size);
			DrawRect(Config.Color, Center.X - ActiveGap - Config.Size, Center.Y - HalfThickness, Config.Size, Config.Thickness);
			DrawRect(Config.Color, Center.X + ActiveGap, Center.Y - HalfThickness, Config.Size, Config.Thickness);
		}
		
	}

}

void ABugHud::DrawHUD()
{
	Super::DrawHUD();
	DrawCrosshair();
	
}


void ABugHud::BeginPlay()
{
	Super::BeginPlay();
	DefaultConfig = Config;
}


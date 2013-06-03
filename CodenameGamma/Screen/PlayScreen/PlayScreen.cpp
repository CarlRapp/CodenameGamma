#include "PlayScreen.h"
#include "Weapons/Weapon.h"
#include "Weapons/WeaponList.h"

PlayScreen::PlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);
	gScreenData	=	Setup;

	gPauseScreenEntry.push_back( "Resume" );
	gPauseScreenEntry.push_back( "Quit" );
	gPauseScreenIndex	=	0;
	
	isPaused	=	false;
}

#pragma region Load / Unload
bool PlayScreen::Load()
{
	IFW1Factory				*pFW1Factory = 0;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(gDevice, L"Apocalypse 1", &gTextInstance);
	pFW1Factory->CreateFontWrapper(gDevice, L"Visitor TT1 BRK", &gWaveTextWrapper);
	pFW1Factory->Release();

	string	tPath;
	for( int n = 0; n < 10; ++n)
	{
		tPath	=	"DATA/GUI/Health/Health_" + to_string( (long double) n + 1 ) + ".png";
		D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, tPath.c_str(), 0, 0, &gHealthBar[n], 0 );
	}
	for( int n = 0; n <= 5; ++n)
	{
		tPath	=	"DATA/GUI/Hunger/Hunger_" + to_string( (long double) n ) + ".png";
		D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, tPath.c_str(), 0, 0, &gHungerBar[n], 0 );

		tPath	=	"DATA/GUI/Thirst/Thirst_" + to_string( (long double) n ) + ".png";
		D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, tPath.c_str(), 0, 0, &gThirstBar[n], 0 );
	}
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/GUI/BlackTrans.png", 0, 0, &gBackground, 0 );
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/GUI/BlackTrans20.png", 0, 0, &gSpectatingBackground, 0 );
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/GUI/Shell.png", 0, 0, &gBulletGUI, 0 );
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/GUI/WaveBackground.png", 0, 0, &gWaveBackground, 0 );

	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/GUI/Weapons/Glock.png", 0, 0, &gWeapons[0], 0 );
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/GUI/Weapons/Shotgun.png", 0, 0, &gWeapons[1], 0 );
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/GUI/Weapons/AutomaticRifle.png", 0, 0, &gWeapons[2], 0 );
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/GUI/Weapons/SniperRifle.png", 0, 0, &gWeapons[3], 0 );
	D3D11_VIEWPORT	tVP	=	D3D11_VIEWPORT();
	tVP.MinDepth	=	0.0f;
	tVP.MaxDepth	=	1.0f;

	tVP.Width	=	54;
	tVP.Height	=	35;
	gWeaponsVP[0]	=	tVP;

	tVP.Width	=	272;
	tVP.Height	=	49;
	gWeaponsVP[1]	=	tVP;

	tVP.Width	=	272;
	tVP.Height	=	57;
	gWeaponsVP[2]	=	tVP;

	tVP.Width	=	272;
	tVP.Height	=	59;
	gWeaponsVP[3]	=	tVP;

	gLevel	=	0;

	SoundManager::GetInstance()->Load("Pistol_Fire", "DATA/Sounds/Weapons/Pistol_Fire.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("Pistol_Reload", "DATA/Sounds/Weapons/Pistol_Reload.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("Pistol_Empty", "DATA/Sounds/Weapons/Pistol_Empty.wav", FMOD_SOFTWARE | FMOD_2D);

	SoundManager::GetInstance()->Load("Shotgun_Fire", "DATA/Sounds/Weapons/Shotgun_Fire.wav", FMOD_SOFTWARE | FMOD_2D);

	SoundManager::GetInstance()->Load("Rifle_Fire", "DATA/Sounds/Weapons/Rifle_Fire.wav", FMOD_SOFTWARE | FMOD_2D);

	SoundManager::GetInstance()->Load("Rat_Death", "DATA/Sounds/Units/Rat_Death.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("Rat_Attack", "DATA/Sounds/Units/Rat_Attack.wav", FMOD_SOFTWARE | FMOD_2D);

	Reset();

	return true;
}

bool PlayScreen::Unload()
{
	if ( gLevel )
		delete	gLevel;

	for( int n = 0; n < 10; ++n)
	{
		gHealthBar[n]->Release();

		if( n <= 5 )
		{
			gHungerBar[n]->Release();
			gThirstBar[n]->Release();
		}

		if( n <= 3 )
			gWeapons[n]->Release();
	}
	SAFE_RELEASE( gSpectatingBackground );
	SAFE_RELEASE( gWaveTextWrapper );
	SAFE_RELEASE( gTextInstance );
	SAFE_RELEASE( gBackground );
	SAFE_RELEASE( gBulletGUI );
	SAFE_RELEASE( gWaveBackground );
		
	return true;
}
#pragma endregion
void PlayScreen::Update(float DeltaTime)
{
	Controller*	tC	=	InputManager::GetInstance()->GetController(0);
	Keyboard*	tK	=	InputManager::GetInstance()->GetKeyboard();

	bool	UP		=	tC->GetButtonState( D_UP ) == PRESSED		|| tK->GetKeyState( VK_UP ) == PRESSED		|| tK->GetKeyState( 'W' ) == PRESSED;
	bool	DOWN	=	tC->GetButtonState( D_DOWN ) == PRESSED		|| tK->GetKeyState( VK_DOWN ) == PRESSED	|| tK->GetKeyState( 'S' ) == PRESSED;
	bool	CONFIRM	=	tC->GetButtonState( A ) == PRESSED			|| tK->GetKeyState( VK_RETURN ) == PRESSED;
	bool	PAUSE	=	tC->GetButtonState( START ) == PRESSED		|| tK->GetKeyState( VK_ESCAPE ) == PRESSED;

	if(	PAUSE )
		isPaused	=	!isPaused;

	if( isPaused )
	{
		if( DOWN )
		{
			++gPauseScreenIndex;
			gPauseScreenIndex	=	( gPauseScreenIndex >= gPauseScreenEntry.size() ) ? 0 : gPauseScreenIndex;
		}
		if( UP )
		{
			--gPauseScreenIndex;
			gPauseScreenIndex	=	( gPauseScreenIndex < 0 ) ? gPauseScreenEntry.size() - 1 : gPauseScreenIndex;
		}
		
		

		if( CONFIRM )
		{
			if( gPauseScreenIndex == 0 )
			{
				isPaused	=	false;
				return;
			}

			if( gPauseScreenIndex == gPauseScreenEntry.size() - 1 )
				gGotoNextFrame	=	MAIN_MENU_SCREEN;
		}

		return;
	}

	gLevel->Update(DeltaTime);

	if( gLevel->IsGameOver() )
	{
		for each( Player* p in gLevel->GetPlayers() )
			gScreenData->PLAYER_SCORE_LIST.push_back( *p->GetPlayerScore() );

		gGotoNextFrame	=	POST_PLAY_SCREEN;
	}
}

void PlayScreen::Render()
{
	gLevel->Render();

	RenderGUISplitter();
	for each( Player* p in gLevel->GetPlayers() )
		RenderGUI( p );

	//	Wave info
	D3D11_VIEWPORT	tVP;
	tVP.MinDepth	=	0.0f;
	tVP.MaxDepth	=	1.0f;
	tVP.Width	=	150;
	tVP.Height	=	100;
	tVP.TopLeftX=	gScreenWidth * 0.5f - tVP.Width * 0.5f;
	tVP.TopLeftY=	0;

	RenderGUISprite( tVP, gWaveBackground );
	gDeviceContext->RSSetViewports( 1, &gFullscreenVP );

	XMFLOAT2	tWavePos;
	Wave::WaveGUIInfo	tWave	=	gLevel->GetWaveInfo();
	float	tWaveSize	=	26;
	tWavePos	=	XMFLOAT2( gScreenWidth * 0.5f, gScreenHeight * 0.02f );
	RenderGUIText( gWaveTextWrapper, tWavePos, "Wave:" + to_string( (long double)tWave.Wave ), tWaveSize, White );
	tWavePos.y	+=	tWaveSize;
	RenderGUIText( gWaveTextWrapper, tWavePos, to_string( (long double)ceil( tWave.Timer.first ) ), tWaveSize, White );
	tWavePos.y	+=	tWaveSize;

	if( tWave.Units.first == tWave.Units.second )
		RenderGUIText( gWaveTextWrapper, tWavePos, to_string( (long double)tWave.Units.first ) + "/" + to_string( (long double)tWave.Units.second ), tWaveSize, Red );
	else
		RenderGUIText( gWaveTextWrapper, tWavePos, to_string( (long double)tWave.Units.first ) + "/" + to_string( (long double)tWave.Units.second ), tWaveSize, White );

	
	if( isPaused )
		RenderPauseScreen();

}

ScreenType PlayScreen::GetScreenType()
{
	return PLAY_SCREEN;
}

void PlayScreen::Reset()
{
	if( gLevel != 0 )
		delete gLevel;

	SystemData	tData			=	SystemData();
	tData.DEVICE				=	gDevice;
	tData.DEVICE_CONTEXT		=	gDeviceContext;
	tData.RENDER_TARGET_VIEW	=	gRenderTargetView;
	tData.SCREEN_WIDTH			=	gScreenWidth;
	tData.SCREEN_HEIGHT			=	gScreenHeight;

	gLevel	=	new Level( tData, gGraphicsManager );
	gLevel->LoadLevel( gScreenData->LEVEL_NAME );
	gScreenData->PLAYER_SCORE_LIST.clear();

	SetNumberOfPlayers(gScreenData->NUMBER_OF_PLAYERS);

	isPaused	=	false;
	SoundManager::GetInstance()->Stop("Theme");
}

void PlayScreen::RenderGUI( Player* P )
{
	if( !P->GetUnit() )
		return;
	if( !P->GetUnit()->IsAlive() )
		return;

	XMFLOAT2	tHealthPos, tHungerPos, tThirstPos, tClipPos;
	D3D11_VIEWPORT		pVP		=	P->GetCamera()->GetViewPort();

	bool	fixVP	=	( ( P->GetIndex() == 1 ||  P->GetIndex() == 3 ) && gLevel->GetPlayers().size() == 4 )
					||	( P->GetIndex() == 1 && gLevel->GetPlayers().size() == 2 )
					||	( P->GetIndex() == 2 && gLevel->GetPlayers().size() == 3 );
	XMFLOAT2	EdgeOffset	=	XMFLOAT2( gFullscreenVP.Width * 0.01f, gFullscreenVP.Height * 0.01f );

	D3D11_VIEWPORT	tVP;
	tVP.TopLeftX	=	pVP.TopLeftX + pVP.Height * 0.014f + EdgeOffset.x;
	tVP.TopLeftY	=	pVP.TopLeftY + pVP.Height * 0.014f + EdgeOffset.y;
	tVP.MinDepth	=	0.0f;
	tVP.MaxDepth	=	1.0f;
	tVP.Width		=	pVP.Height * 0.11f;
	tVP.Height		=	pVP.Height * 0.09f;

	float	tTextSize	=	( pVP.Height ) / ( 72 - 10 * gLevel->GetPlayers().size() );

	if( fixVP )
		tVP.TopLeftX	=	pVP.TopLeftX + pVP.Width - pVP.Height * 0.014f - tVP.Width - EdgeOffset.x;

	//	The stats
	UnitHealth			uHealth	=	P->GetUnit()->GetHealth();
	UnitHunger			uHunger	=	P->GetUnit()->GetHungerMeter();
	UnitThirst			uThirst	=	P->GetUnit()->GetThirstMeter();
	int	tPercent;
	int	tIndex;
	
	//	Health
	tPercent	=	(int)ceil( 100.f * ( uHealth.first / uHealth.second ) );
	tPercent	=	MathHelper::Clamp(tPercent, 0, 100);
	tIndex		=	(int)( (float)0.1f * (float)tPercent );
	tIndex		=	MathHelper::Clamp(tIndex, 0, 9);
	RenderGUISprite( tVP, gHealthBar[tIndex] );
	tHealthPos.x	=	tVP.TopLeftX + tVP.Width * 0.5f;
	tHealthPos.y	=	tVP.TopLeftY + tVP.Height * 0.5f;
	
	//	Move the viewport	
	tVP.TopLeftX	=	pVP.TopLeftX + pVP.Height * 0.02f + EdgeOffset.x;
	tVP.TopLeftY	=	pVP.TopLeftY + pVP.Height * 0.111f + EdgeOffset.y;
	tVP.Width		=	pVP.Height * 0.035f;
	tVP.Height		=	pVP.Height * 0.0695f;

	if( fixVP )
		tVP.TopLeftX	=	pVP.TopLeftX + pVP.Width - pVP.Height * 0.02f - tVP.Width - EdgeOffset.x;


	//	Hunger
	tPercent	=	(int)ceil( 100.f * ( uHunger.first / uHunger.second ) );
	tPercent	=	MathHelper::Clamp(tPercent, 0, 100);
	tIndex		=	(int)( 0.05f * tPercent );
	tIndex		=	MathHelper::Clamp(tIndex, 0, 5);
	RenderGUISprite( tVP, gHungerBar[tIndex] );
	tHungerPos.x	=	tVP.TopLeftX + tVP.Width * 0.5f;
	tHungerPos.y	=	tVP.TopLeftY + tVP.Height * 0.5f;

	//	Move the viewport
	tVP.TopLeftX	=	pVP.TopLeftX + pVP.Height * 0.083f + EdgeOffset.x;
	tVP.TopLeftY	=	pVP.TopLeftY + pVP.Height * 0.111f + EdgeOffset.y;
	tVP.Width		=	pVP.Height * 0.035f;
	tVP.Height		=	pVP.Height * 0.0695f;

	if( fixVP )
		tVP.TopLeftX	=	pVP.TopLeftX + pVP.Width - pVP.Height * 0.083f - tVP.Width - EdgeOffset.x;

	//	Thirst
	tPercent	=	(int)ceil( 100.f * ( uThirst.first / uThirst.second ) );
	tPercent	=	MathHelper::Clamp(tPercent, 0, 100);
	tIndex		=	(int)( 0.05f * tPercent );
	tIndex		=	MathHelper::Clamp(tIndex, 0, 5);
	RenderGUISprite( tVP, gThirstBar[tIndex] );
	tThirstPos.x	=	tVP.TopLeftX + tVP.Width * 0.5f;
	tThirstPos.y	=	tVP.TopLeftY + tVP.Height * 0.5f;


	//	Weapon bullet printing
	if( P->GetUnit()->GetWeapon() )
	{
		Weapon::WeaponInfo	tInfo	=	P->GetUnit()->GetWeapon()->GetInfo();

		tVP.TopLeftX	=	pVP.TopLeftX + pVP.Height * 0.014f + EdgeOffset.x + pVP.Height * 0.11f;
		tVP.TopLeftY	=	pVP.TopLeftY + pVP.Height * 0.014f + EdgeOffset.y;
		tVP.Width		=	pVP.Height * 0.02f;
		tVP.Height		=	pVP.Height * 0.02f;

		float	tWidth	=	tVP.Width + tVP.Width * 0.05f;
		float	tHeight	=	tVP.Height + tVP.Height * 0.05f;

		XMFLOAT2	ClipZero;
		ClipZero.x	=	pVP.TopLeftX + pVP.Height * 0.014f + EdgeOffset.x + pVP.Height * 0.11f;
		ClipZero.y	=	pVP.TopLeftY + pVP.Height * 0.014f + EdgeOffset.y;

		if( fixVP )
		{
			tVP.TopLeftX	=	pVP.TopLeftX + pVP.Width - pVP.Height * 0.014f - EdgeOffset.x - pVP.Height * 0.11f - tWidth;
			ClipZero.x		=	pVP.TopLeftX + pVP.Width - pVP.Height * 0.014f - EdgeOffset.x - pVP.Height * 0.11f;
		}

		int	tCurrentAmmo	=	tInfo.Magazine.first;
		int	i	=	0;
		int	dir	=	fixVP ? -1 : 1;
		while(true)
		{
			bool	isEven	=	i % 2 == 0;
			int	tN;
			if( isEven )
				tN	=	( tCurrentAmmo - 9 >= 0 ) ? 9 : tCurrentAmmo;
			else
				tN	=	( tCurrentAmmo - 8 >= 0 ) ? 8 : tCurrentAmmo;

			if( fixVP )
				tVP.TopLeftX	=	isEven ? ClipZero.x - tWidth : ( ClipZero.x + dir * 0.5f * tWidth ) - tWidth;
			else
				tVP.TopLeftX	=	isEven ? ClipZero.x : ( ClipZero.x + dir * 0.5f * tWidth );
			tVP.TopLeftY	=	ClipZero.y + i * tHeight;

			for( int n = 0; n < tN; ++n )
			{
				tVP.TopLeftX	+=	dir * tWidth;
				RenderGUISprite( tVP, gBulletGUI );
			}

			++i;
			tCurrentAmmo	-=	tN;
			if( tCurrentAmmo <= 0 || i == 5 )
				break;
		}
		Weapon*	tWeapon	=	P->GetUnit()->GetWeapon();
		float	tWeaponWidth, tWeaponHeight;
		int		tWeaponIndex	=	0;

		if( IsOfType<Pistol>( tWeapon ) )
		{
			tWeaponWidth	=	gWeaponsVP[0].Width;
			tWeaponHeight	=	gWeaponsVP[0].Height;
		}
		else if( IsOfType<Shotgun>( tWeapon ) )
		{
			tWeaponWidth	=	gWeaponsVP[1].Width;
			tWeaponHeight	=	gWeaponsVP[1].Height;
			tWeaponIndex	=	1;
		}
		else if( IsOfType<AutomaticRifle>( tWeapon ) )
		{
			tWeaponWidth	=	gWeaponsVP[2].Width;
			tWeaponHeight	=	gWeaponsVP[2].Height;
			tWeaponIndex	=	2;
		}
		else
		{
			tWeaponWidth	=	gWeaponsVP[3].Width;
			tWeaponHeight	=	gWeaponsVP[3].Height;
			tWeaponIndex	=	3;
		}

		tVP.Width		=	tWeaponWidth * pVP.Height * 0.001f;
		tVP.Height		=	tWeaponHeight * pVP.Height * 0.001f;
		if( fixVP )
			tVP.TopLeftX	=	ClipZero.x + dir * 11 * tWidth - tVP.Width;
		else
			tVP.TopLeftX	=	ClipZero.x + dir * 11 * tWidth;
		tVP.TopLeftY	=	ClipZero.y;

		RenderGUISprite( tVP, gWeapons[tWeaponIndex] );

		tClipPos.x	=	ClipZero.x + (float)dir * 5.5f * tWidth;
		tClipPos.y	=	ClipZero.y + tHeight * ( 1.5f + ceil( tInfo.Magazine.second * 0.1f ) );

		//	Render the text
		gDeviceContext->RSSetViewports( 1, &gFullscreenVP );
		if( tInfo.Ammo != -1 )
			RenderGUIText( gWaveTextWrapper, tClipPos, to_string( (long double)( tInfo.Ammo / tInfo.Magazine.second ) ) + " clips left.", tTextSize, White );
		else
			RenderGUIText( gWaveTextWrapper, tClipPos, "infinite clips.", tTextSize, White );
	}

	//	Render the text
	gDeviceContext->RSSetViewports( 1, &gFullscreenVP );
	RenderGUIText( gWaveTextWrapper, tHealthPos, to_string( (long double)( (int)(100.0f * ( uHealth.first / uHealth.second ) ) ) ), tTextSize, White );

	if( !gLevel->IsGameOver() )
		if( P->IsSpectating() )
		{
			RenderGUISprite( P->GetCamera()->GetViewPort(), gSpectatingBackground );
			gDeviceContext->RSSetViewports( 1, &gFullscreenVP );

			tVP	=	P->GetCamera()->GetViewPort();
			XMFLOAT2	tPos	=	XMFLOAT2( tVP.TopLeftX + tVP.Width * 0.5f, tVP.TopLeftY + tVP.Height * 0.2f );
			RenderGUIText( gWaveTextWrapper, tPos, "Spectating", tTextSize, White );
		}
}

void PlayScreen::RenderGUISprite( D3D11_VIEWPORT VP, ID3D11ShaderResourceView* Sprite )
{
	gGraphicsManager->RenderQuad( VP, Sprite, Effects::CombineFinalFX->AlphaTransparencyColorTech );
}

void PlayScreen::RenderGUIText( XMFLOAT2 Position, string Text, float TextSize, TextColor Color )
{
	RenderGUIText( gTextInstance, Position, Text, TextSize, Color );
}
void PlayScreen::RenderGUIText( IFW1FontWrapper* FontWrapper, XMFLOAT2 Position, string Text, float TextSize, TextColor Color )
{
	DrawString(
		*FontWrapper,
		Text,
		Position.x,
		Position.y,
		TextSize,
		Color,
		BlackTrans,
		1.0f,
		FW1_CENTER | FW1_VCENTER
	);
}

void PlayScreen::RenderPauseScreen()
{
	gGraphicsManager->RenderQuad( gFullscreenVP, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );

	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.10f);
	RenderGUIText( tPos, "Paused", 72, White );
	tPos.y	+=	72 * 1.5f;

	int	i	=	0;
	for each( string tText in gPauseScreenEntry )
	{
		if( i == gPauseScreenIndex )
			DrawString(*gTextInstance, tText, tPos.x, tPos.y + 1.5f * i * 38, 38, Green, Black, 2, FW1_CENTER);
		else
			DrawString(*gTextInstance, tText, tPos.x, tPos.y + 1.5f * i * 38, 38, WhiteTrans, BlackTrans, 2, FW1_CENTER);

		++i;
	}

}

void PlayScreen::RenderGUISplitter()
{
	D3D11_VIEWPORT	tFrame;
	tFrame.TopLeftX	=	0;
	tFrame.TopLeftY	=	0;
	tFrame.MinDepth	=	0.0f;
	tFrame.MaxDepth	=	1.0f;
	tFrame.Width	=	3;
	tFrame.Height	=	0;
	switch( gLevel->GetPlayers().size() )
	{
	case 2:
		tFrame.TopLeftX	=	gScreenWidth * 0.5f;
		tFrame.TopLeftY	=	0;
		tFrame.Height	=	gScreenHeight;
		gGraphicsManager->RenderQuad( tFrame, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );
		break;

	case 3:
		tFrame.TopLeftX	=	0;
		tFrame.TopLeftY	=	gScreenHeight * 0.5f;
		tFrame.Height	=	3;
		tFrame.Width	=	gScreenWidth;
		gGraphicsManager->RenderQuad( tFrame, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );

		tFrame.TopLeftX	=	gScreenWidth * 0.5f;
		tFrame.TopLeftY	=	gScreenHeight * 0.5f;
		tFrame.Height	=	gScreenHeight * 0.5f;
		tFrame.Width	=	3;
		gGraphicsManager->RenderQuad( tFrame, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );
		break;

	case 4:
		tFrame.TopLeftX	=	0;
		tFrame.TopLeftY	=	gScreenHeight * 0.5f;
		tFrame.Height	=	3;
		tFrame.Width	=	gScreenWidth;
		gGraphicsManager->RenderQuad( tFrame, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );

		tFrame.TopLeftX	=	gScreenWidth * 0.5f;
		tFrame.TopLeftY	=	0;
		tFrame.Height	=	gScreenHeight;
		tFrame.Width	=	3;
		gGraphicsManager->RenderQuad( tFrame, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );
		break;
	}

	gDeviceContext->RSSetViewports( 1, &gFullscreenVP );
}
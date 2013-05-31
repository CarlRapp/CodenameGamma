#include "PlayScreen.h"
#include "Weapons/Weapon.h"

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
	gLevel	=	0;

	SoundManager::GetInstance()->Load("Pistol_Fire", "DATA/Sounds/Weapons/Pistol_Fire.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("Pistol_Reload", "DATA/Sounds/Weapons/Pistol_Reload.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("Pistol_Empty", "DATA/Sounds/Weapons/Pistol_Empty.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("Shotgun_Fire", "DATA/Sounds/Weapons/Shotgun_Fire.wav", FMOD_SOFTWARE | FMOD_2D);

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
	}
	SAFE_RELEASE( gTextInstance );
	SAFE_RELEASE( gBackground );
		
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
	bool	PAUSE	=	tC->GetButtonState( START ) == PRESSED			|| tK->GetKeyState( VK_ESCAPE ) == PRESSED;

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


	for each( Player* p in gLevel->GetPlayers() )
		RenderGUI( p );

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

	XMFLOAT2	tHealthPos, tHungerPos, tThirstPos, tClipPos, tWavePos;
	D3D11_VIEWPORT		pVP		=	P->GetCamera()->GetViewPort();

	bool	fixVP	=	( ( P->GetIndex() == 1 ||  P->GetIndex() == 3 ) && gLevel->GetPlayers().size() == 4 );
	XMFLOAT2	EdgeOffset	=	XMFLOAT2( gFullscreenVP.Width * 0.01f, gFullscreenVP.Height * 0.01f );

	D3D11_VIEWPORT	tVP;
	tVP.TopLeftX	=	pVP.TopLeftX + pVP.Height * 0.014f + EdgeOffset.x;
	tVP.TopLeftY	=	pVP.TopLeftY + pVP.Height * 0.014f + EdgeOffset.y;
	tVP.MinDepth	=	0.0f;
	tVP.MaxDepth	=	1.0f;
	tVP.Width		=	pVP.Height * 0.11f;
	tVP.Height		=	pVP.Height * 0.09f;

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
	tPercent	=	MathHelper::Clamp(0, tPercent, 100);
	tIndex		=	(int)( (float)0.1f * (float)tPercent );
	tIndex		=	MathHelper::Clamp(0, tIndex, 9);
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
	tPercent	=	MathHelper::Clamp(0, tPercent, 100);
	tIndex		=	(int)( 0.05f * tPercent );
	tIndex		=	MathHelper::Clamp(0, tIndex, 5);
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
	tPercent	=	MathHelper::Clamp(0, tPercent, 100);
	tIndex		=	(int)( 0.05f * tPercent );
	tIndex		=	MathHelper::Clamp(0, tIndex, 5);
	RenderGUISprite( tVP, gThirstBar[tIndex] );
	tThirstPos.x	=	tVP.TopLeftX + tVP.Width * 0.5f;
	tThirstPos.y	=	tVP.TopLeftY + tVP.Height * 0.5f;


	//	Render the text
	gDeviceContext->RSSetViewports( 1, &gFullscreenVP );
	RenderGUIText( tHealthPos, to_string( (long double)( (int)(100.0f * ( uHealth.first / uHealth.second ) ) ) ), 18, White );


	//	Weapon info
	if( P->GetUnit()->GetWeapon() )
	{
		Weapon::WeaponInfo	tInfo	=	P->GetUnit()->GetWeapon()->GetInfo();
		string	tClipInfo;
		tClipInfo	+=	to_string( (long double)tInfo.Magazine.first ) + "/";
		if( tInfo.Ammo == -1 )
			tClipInfo	+=	"#";
		else
			tClipInfo	+=	to_string( (long double)tInfo.Ammo );
		tClipPos.x	=	tHealthPos.x;
		tClipPos.y	=	tVP.TopLeftY + tVP.Height * 1.10f;
		RenderGUIText( tClipPos, tClipInfo, 18, White );
	}

	//	Wave info
	Wave::WaveGUIInfo	tWave	=	gLevel->GetWaveInfo();
	float	tWaveSize	=	26;
	tWavePos	=	XMFLOAT2( gScreenWidth * 0.5f, gScreenHeight * 0.025f );
	RenderGUIText( gWaveTextWrapper, tWavePos, "Wave: " + to_string( (long double)tWave.Wave ), tWaveSize, White );
	tWavePos.y	+=	tWaveSize;
	RenderGUIText( gWaveTextWrapper, tWavePos, to_string( (long double)ceil( tWave.Timer.first ) ), tWaveSize, White );
	tWavePos.y	+=	tWaveSize;

	if( tWave.Units.first == tWave.Units.second )
		RenderGUIText( gWaveTextWrapper, tWavePos, to_string( (long double)tWave.Units.first ) + "/" + to_string( (long double)tWave.Units.second ), tWaveSize, Red );
	else
		RenderGUIText( gWaveTextWrapper, tWavePos, to_string( (long double)tWave.Units.first ) + "/" + to_string( (long double)tWave.Units.second ), tWaveSize, White );
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
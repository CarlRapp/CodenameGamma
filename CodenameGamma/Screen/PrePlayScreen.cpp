#include "PrePlayScreen.h"

PrePlayScreen::PrePlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

	gScreenData	=	Setup;
	gScreenData->NUMBER_OF_PLAYERS	=	1;


	gMenuEntries[0]	=	MenuEntry("One",		PLAY_SCREEN);
	gMenuEntries[1]	=	MenuEntry("Two",		PLAY_SCREEN);
	gMenuEntries[2]	=	MenuEntry("Three",		PLAY_SCREEN);
	gMenuEntries[3]	=	MenuEntry("Four",		PLAY_SCREEN);
	gTextSize		=	32.0f;
}
bool PrePlayScreen::Load()
{
	IFW1Factory				*pFW1Factory = 0;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(gDevice, L"Apocalypse 1", &gTextInstance);
	pFW1Factory->Release();

	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/MAIN_MENU.png", 0, 0, &gBackground, 0 );

	Reset();
	return true;
}
bool PrePlayScreen::Unload()
{
	SAFE_RELEASE( gBackground );
	SAFE_RELEASE( gTextInstance );

	return false;
}
void PrePlayScreen::Reset()
{
	gConfirmNext	=	false;
}

void PrePlayScreen::Update(float DeltaTime)
{
	if( gConfirmNext )
	{
		MenuEntry	tMenuEntry	=	gMenuEntries[gCurrentIndex];

		gGotoNextFrame	=	tMenuEntry.second;
		return;
	}


	gTextSizeActiveTicker	+=	DeltaTime;

	Controller*	tC	=	InputManager::GetInstance()->GetController(0);
	Keyboard*	tK	=	InputManager::GetInstance()->GetKeyboard();

	bool	UP		=	tC->GetButtonState( D_UP ) == PRESSED		|| tK->GetKeyState( VK_UP ) == PRESSED		|| tK->GetKeyState( 'W' ) == PRESSED;
	bool	DOWN	=	tC->GetButtonState( D_DOWN ) == PRESSED		|| tK->GetKeyState( VK_DOWN ) == PRESSED	|| tK->GetKeyState( 'S' ) == PRESSED;
	bool	CONFIRM	=	tC->GetButtonState( A ) == PRESSED			|| tK->GetKeyState( VK_RETURN ) == PRESSED;
	bool	BACK	=	tC->GetButtonState( B ) == PRESSED			|| tK->GetKeyState( VK_ESCAPE ) == PRESSED;



	if( DOWN )
	{
		gTextSizeActiveTicker	=	0;

		gCurrentIndex	=	(gCurrentIndex + 1 >= (int)gMenuEntries.size()) ? 0 : gCurrentIndex + 1;
		SoundManager::GetInstance()->Play("MenuChange", SFX);
	}
	if( UP )
	{
		gTextSizeActiveTicker	=	0;

		gCurrentIndex	=	(gCurrentIndex - 1 < 0) ? gMenuEntries.size() - 1 : gCurrentIndex - 1;
		SoundManager::GetInstance()->Play("MenuChange", SFX);
	}
	if( CONFIRM )
	{
		gScreenData->NUMBER_OF_PLAYERS	=	gCurrentIndex + 1;
		SoundManager::GetInstance()->Play("MenuPick", SFX);
		gConfirmNext	=	true;
	}

	if( BACK )
		gGotoNextFrame	=	LEVEL_SELECT_SCREEN;
	

	gTextSizeActive			=	(gTextSize	+	cos(10 * gTextSizeActiveTicker));
}


void PrePlayScreen::Render()
{
	gGraphicsManager->RenderQuad( gFullscreenVP, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );

	if( !gConfirmNext )
	{
		XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.05f, gScreenHeight * 0.05f);
	
		DrawString(*gTextInstance, "Select number of players", tPos.x, tPos.y, 72, White, WhiteTrans, 2, FW1_LEFT);
		tPos.y	+=	1.5f * 72;

		int	i	=	0;
		for ( gMenuIterator = gMenuEntries.begin(); gMenuIterator != gMenuEntries.end(); gMenuIterator++ )
		{
			MenuEntry	tMenuEntry	=	gMenuIterator->second;
			bool		isActive	=	(i == gCurrentIndex);
			TextColor	tTColor		=	isActive ? Orange : WhiteTrans;
			TextColor	tBColor		=	isActive ? OrangeTrans : BlackTrans;
			string		tText		=	isActive ? ((char)235) + tMenuEntry.first + ((char)233) : tMenuEntry.first;

			float		tTextSize	=	(i == gCurrentIndex) ? gTextSizeActive : gTextSize;

			DrawString(*gTextInstance, tText, tPos.x, tPos.y + i * 1.5f * gTextSize , tTextSize, tTColor, tBColor, 2, FW1_LEFT);
			i++;
		}

		return;
	}

	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.45f);
	
	DrawString(*gTextInstance, "Loading...", tPos.x, tPos.y, 72, White, WhiteTrans, 2, FW1_CENTER | FW1_VCENTER);
	tPos.y	+=	1.0f * 72;
	DrawString(*gTextInstance, "It can take a while, so don't worry.", tPos.x, tPos.y, 18, White, WhiteTrans, 2, FW1_CENTER | FW1_VCENTER);
}

ScreenType PrePlayScreen::GetScreenType()
{
	return PRE_PLAY_SCREEN;
}
#include "MainMenuScreen.h"
#include "PlayScreen/Graphics/GraphicsManager.h"
#include "PlayScreen/Graphics/RenderStates.h"

MainMenuScreen::MainMenuScreen(void) 
{ 
	gTextSize				=	62;
	gTextSizeActive			=	0;
	gTextSizeActiveTicker	=	0;
	gCurrentIndex			=	0;
}
MainMenuScreen::MainMenuScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);
	gScreenData	=	Setup;

	
	gMenuEntries[0]	=	MenuEntry("Play Game",		LEVEL_SELECT_SCREEN);
	gMenuEntries[1]	=	MenuEntry("Options",		OPTIONS_SCREEN);
	gMenuEntries[2]	=	MenuEntry("Credits",		CREDITS_SCREEN);
	gMenuEntries[3]	=	MenuEntry("Quit",			EXIT_SCREEN); 

	gTextSize				=	62;
	gTextSizeActive			=	0;
	gTextSizeActiveTicker	=	0;
	gCurrentIndex			=	0;
}

bool MainMenuScreen::Load()
{
	SoundManager::GetInstance()->Load("MenuChange", "DATA/Sounds/MenuChange.mp3", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("MenuPick", "DATA/Sounds/MenuPick.mp3", FMOD_SOFTWARE | FMOD_2D);

	SoundManager::GetInstance()->Load("Theme", "DATA/Sounds/Theme.mp3", FMOD_SOFTWARE | FMOD_2D);

	SoundManager::GetInstance()->Play("Theme", Song, true);

	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/MAIN_MENU.png", 0, 0, &gBackground, 0 );

	IFW1Factory				*pFW1Factory = 0;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(gDevice, L"Apocalypse 1", &gTextInstance);
	pFW1Factory->Release();

	return true;
}

bool MainMenuScreen::Unload()
{
	SAFE_RELEASE( gBackground );
	SAFE_RELEASE( gTextInstance );
	return true;
}
void MainMenuScreen::Update(float DeltaTime)
{
	gTextSizeActiveTicker	+=	DeltaTime;

	Controller*	tC	=	InputManager::GetInstance()->GetController(0);
	Keyboard*	tK	=	InputManager::GetInstance()->GetKeyboard();

	bool	UP		=	tC->GetButtonState( D_UP )	== PRESSED		|| tK->GetKeyState( VK_UP ) == PRESSED		|| tK->GetKeyState( 'W' ) == PRESSED;
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
		MenuEntry	tMenuEntry	=	gMenuEntries[gCurrentIndex];

		gGotoNextFrame	=	tMenuEntry.second;
		SoundManager::GetInstance()->Play("MenuPick", SFX);
	}

	if( BACK )
	{
		if( gCurrentIndex != gMenuEntries.size() - 1 )
			gCurrentIndex	=	gMenuEntries.size() - 1;
		else
			gGotoNextFrame	=	EXIT_SCREEN;
	}
	

	gTextSizeActive			=	(gTextSize	+	cos(10 * gTextSizeActiveTicker));
}


void MainMenuScreen::Render()
{
	gGraphicsManager->RenderQuad( gFullscreenVP, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );
	
	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.1f);
	DrawString(*gTextInstance, "Codename: Gamma", tPos.x, tPos.y, 72, Red, RedTrans, 2, FW1_CENTER);
	
	int	i	=	0;
	for ( gMenuIterator = gMenuEntries.begin(); gMenuIterator != gMenuEntries.end(); gMenuIterator++ )
	{
		MenuEntry	tMenuEntry	=	gMenuIterator->second;
		bool		isActive	=	(i == gCurrentIndex);
		TextColor	tTColor		=	isActive ? Orange : WhiteTrans;
		TextColor	tBColor		=	isActive ? OrangeTrans : BlackTrans;
		string		tText		=	isActive ? ((char)235) + tMenuEntry.first + ((char)233) : tMenuEntry.first;

		XMFLOAT2	tPosition	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.3f + i * (gTextSize + gTextSize * 0.1f));

		float		tTextSize	=	isActive ? gTextSizeActive : gTextSize;

		DrawString(*gTextInstance, tText, tPosition.x, tPosition.y, tTextSize, tTColor, tBColor, 2, FW1_CENTER);
		i++;
	}
}

ScreenType MainMenuScreen::GetScreenType()
{
	return MAIN_MENU_SCREEN;
}

void MainMenuScreen::Reset()
{
	if (!SoundManager::GetInstance()->IsPlaying("Theme"))
		SoundManager::GetInstance()->Play("Theme", Song, true);
}
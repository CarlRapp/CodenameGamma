#include "MainMenuScreen.h"
DebugData*	FPS_COUNTER;
MainMenuScreen::MainMenuScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

	
	gMenuEntries[0]	=	MenuEntry("Play Game",		PLAY_SCREEN);
	gMenuEntries[1]	=	MenuEntry("Options",		UNDEFINED_SCREEN);
	gMenuEntries[2]	=	MenuEntry("Credits",		UNDEFINED_SCREEN);
	gMenuEntries[3]	=	MenuEntry("Quit",			EXIT_SCREEN);

	gTextSize				=	62;
	gTextSizeActive			=	0;
	gTextSizeActiveTicker	=	0;
	gCurrentIndex			=	0;

	SoundManager::GetInstance()->Load("TEST", "DATA/Sounds/Heart.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("TEST2", "DATA/Sounds/Save.wav", FMOD_SOFTWARE | FMOD_2D);

	SoundManager::GetInstance()->Load("Theme", "DATA/Sounds/Theme.mp3", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("MenuPick", "DATA/Sounds/MenuPick.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Play("Theme", true);
	SoundManager::GetInstance()->Play("Derpaderp");

	FPS_COUNTER	=	DebugScreen::GetInstance()->AddDebugData("");
	FPS_COUNTER->Value	=	" FPS";
	FPS_COUNTER->ValueColor	=	Green;
	FPS_COUNTER->TitleColor	=	White;
}

bool MainMenuScreen::Load()
{
	return true;
}

bool MainMenuScreen::Unload()
{
	return true;
}
void MainMenuScreen::Update(float DeltaTime)
{
	gTextSizeActiveTicker	+=	DeltaTime;

	Controller*	gController	=	InputManager::GetInstance()->GetController(0);
	if(gController->GetButtonState(D_DOWN) == PRESSED)
	{
		gTextSizeActiveTicker	=	0;

		gCurrentIndex	=	(gCurrentIndex + 1 >= gMenuEntries.size()) ? 0 : gCurrentIndex + 1;
		SoundManager::GetInstance()->Play("MenuPick");
	}
	if(gController->GetButtonState(D_UP) == PRESSED)
	{
		gTextSizeActiveTicker	=	0;

		gCurrentIndex	=	(gCurrentIndex - 1 < 0) ? gMenuEntries.size() - 1 : gCurrentIndex - 1;
		SoundManager::GetInstance()->Play("TEST");
	}
	if(gController->GetButtonState(A) == PRESSED)
	{
		MenuEntry	tMenuEntry	=	gMenuEntries[gCurrentIndex];

		gGotoNextFrame	=	tMenuEntry.second;
		SoundManager::GetInstance()->Play("MenuPick");
	}
	

	gTextSizeActive			=	(gTextSize	+	cos(10 * gTextSizeActiveTicker));

	FPS_COUNTER->Title	=	to_string((long double)((int)(1/DeltaTime)));
}


void MainMenuScreen::Render()
{
	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.1f);
	DrawString(*gTextInstance, "Codename: Gamma", tPos.x, tPos.y, 72, Red, Black, 2, FW1_CENTER);
	
	int	i	=	0;
	for ( gMenuIterator = gMenuEntries.begin(); gMenuIterator != gMenuEntries.end(); gMenuIterator++ )
	{
		MenuEntry	tMenuEntry	=	gMenuIterator->second;
		TextColor	tColor		=	(i == gCurrentIndex) ? Orange : White;
		string		tText		=	(i == gCurrentIndex) ? ".: " + tMenuEntry.first + " :." : tMenuEntry.first;

		XMFLOAT2	tPosition	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.3f + i * (gTextSize + gTextSize * 0.1f));

		float		tTextSize	=	(i == gCurrentIndex) ? gTextSizeActive : gTextSize;

		DrawString(*gTextInstance, tText, tPosition.x, tPosition.y, tTextSize, tColor, Black, 2, FW1_CENTER);
		i++;
	}
}

ScreenType MainMenuScreen::GetScreenType()
{
	return MAIN_MENU_SCREEN;
}
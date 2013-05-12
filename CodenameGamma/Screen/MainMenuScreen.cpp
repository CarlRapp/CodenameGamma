#include "MainMenuScreen.h"
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

	
	gMenuEntries[0]	=	MenuEntry("Play Game",		PRE_PLAY_SCREEN);
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

	SoundManager::GetInstance()->Load("Pistol", "DATA/Sounds/Pistol.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("Reload", "DATA/Sounds/Reload.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("EmptyClip", "DATA/Sounds/EmptyClip.wav", FMOD_SOFTWARE | FMOD_2D);
	SoundManager::GetInstance()->Load("Shotgun_Fire", "DATA/Sounds/Shotgun_Fire.mp3", FMOD_SOFTWARE | FMOD_2D);

	//SoundManager::GetInstance()->Play("Theme", true);
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

	Controller*	controller	=	InputManager::GetInstance()->GetController(0);
	Keyboard*	keyboard	=	InputManager::GetInstance()->GetKeyboard();
	if(controller->GetButtonState(D_DOWN) == PRESSED 
		|| keyboard->GetKeyState('S') == PRESSED || 
		keyboard->GetKeyState(VK_DOWN) == PRESSED)
	{
		gTextSizeActiveTicker	=	0;

		gCurrentIndex	=	(gCurrentIndex + 1 >= (int)gMenuEntries.size()) ? 0 : gCurrentIndex + 1;
		SoundManager::GetInstance()->Play("MenuPick");
	}
	if(controller->GetButtonState(D_UP) == PRESSED || 
		keyboard->GetKeyState('W') == PRESSED || 
		keyboard->GetKeyState(VK_UP) == PRESSED)
	{
		gTextSizeActiveTicker	=	0;

		gCurrentIndex	=	(gCurrentIndex - 1 < 0) ? gMenuEntries.size() - 1 : gCurrentIndex - 1;
		SoundManager::GetInstance()->Play("TEST");
	}
	if(controller->GetButtonState(A) == PRESSED || keyboard->GetKeyState(VK_RETURN) == PRESSED)
	{
		MenuEntry	tMenuEntry	=	gMenuEntries[gCurrentIndex];

		gGotoNextFrame	=	tMenuEntry.second;
		SoundManager::GetInstance()->Play("MenuPick");
		SoundManager::GetInstance()->Stop("Theme");
	}
	

	gTextSizeActive			=	(gTextSize	+	cos(10 * gTextSizeActiveTicker));
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
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
}

void PrePlayScreen::Update(float DeltaTime)
{
	gTextSizeActiveTicker	+=	DeltaTime;

	Controller*	gController	=	InputManager::GetInstance()->GetController(0);
	if(gController->GetButtonState(D_DOWN) == PRESSED)
	{
		gTextSizeActiveTicker	=	0;

		gCurrentIndex	=	(gCurrentIndex + 1 >= (int)gMenuEntries.size()) ? 0 : gCurrentIndex + 1;
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
		//for( int i = 0; i < gCurrentIndex + 1; ++i )
		//{
			//Player*	p	=	new Player(i);
			
			//gScreenData->PLAYER_LIST->push_back(p);
		//}

		gScreenData->NUMBER_OF_PLAYERS	=	gCurrentIndex + 1;

		MenuEntry	tMenuEntry	=	gMenuEntries[gCurrentIndex];

		gGotoNextFrame	=	tMenuEntry.second;
		SoundManager::GetInstance()->Play("MenuPick");
		SoundManager::GetInstance()->Stop("Theme");
	}
	

	gTextSizeActive			=	(gTextSize	+	cos(10 * gTextSizeActiveTicker));
}


void PrePlayScreen::Render()
{
	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.1f);
	DrawString(*gTextInstance, "How many players?", tPos.x, tPos.y, 72, Red, Black, 2, FW1_CENTER);
	
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

ScreenType PrePlayScreen::GetScreenType()
{
	return PRE_PLAY_SCREEN;
}
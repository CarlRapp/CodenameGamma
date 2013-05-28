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
bool PrePlayScreen::Load()
{
	return true;
}
bool PrePlayScreen::Unload()
{
	return false;
}
void PrePlayScreen::Reset()
{
}

void PrePlayScreen::Update(float DeltaTime)
{
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
		SoundManager::GetInstance()->Play("MenuPick", SFX);
	}
	if( UP )
	{
		gTextSizeActiveTicker	=	0;

		gCurrentIndex	=	(gCurrentIndex - 1 < 0) ? gMenuEntries.size() - 1 : gCurrentIndex - 1;
		SoundManager::GetInstance()->Play("TEST", SFX);
	}
	if( CONFIRM )
	{
		gScreenData->NUMBER_OF_PLAYERS	=	gCurrentIndex + 1;

		MenuEntry	tMenuEntry	=	gMenuEntries[gCurrentIndex];

		gGotoNextFrame	=	tMenuEntry.second;
		SoundManager::GetInstance()->Play("MenuPick", SFX);
		SoundManager::GetInstance()->Stop("Theme");
	}

	if( BACK )
		gGotoNextFrame	=	LEVEL_SELECT_SCREEN;
	

	gTextSizeActive			=	(gTextSize	+	cos(10 * gTextSizeActiveTicker));
}


void PrePlayScreen::Render()
{
	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.1f);
	DrawString(*gTextInstance, "How many players?", tPos.x, tPos.y, 72, Red, RedTrans, 2, FW1_CENTER);
	
	int	i	=	0;
	for ( gMenuIterator = gMenuEntries.begin(); gMenuIterator != gMenuEntries.end(); gMenuIterator++ )
	{
		MenuEntry	tMenuEntry	=	gMenuIterator->second;
		TextColor	tTColor		=	(i == gCurrentIndex) ? Orange : WhiteTrans;
		TextColor	tBColor		=	(i == gCurrentIndex) ? OrangeTrans : BlackTrans;
		string		tText		=	(i == gCurrentIndex) ? ((char)235) + tMenuEntry.first + ((char)233) : tMenuEntry.first;

		XMFLOAT2	tPosition	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.3f + i * (gTextSize + gTextSize * 0.1f));

		float		tTextSize	=	(i == gCurrentIndex) ? gTextSizeActive : gTextSize;

		DrawString(*gTextInstance, tText, tPosition.x, tPosition.y, tTextSize, tTColor, tBColor, 2, FW1_CENTER);
		i++;
	}
}

ScreenType PrePlayScreen::GetScreenType()
{
	return PRE_PLAY_SCREEN;
}
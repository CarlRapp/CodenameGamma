#include "PlayScreen.h"


PlayScreen::PlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

}

bool PlayScreen::Load()
{
	return true;
}

bool PlayScreen::Unload()
{
	return true;
}

void PlayScreen::Update(float DeltaTime)
{

}


void PlayScreen::Render()
{

}

ScreenType PlayScreen::GetScreenType()
{
	return PLAY_SCREEN;
}


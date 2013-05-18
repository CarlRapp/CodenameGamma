#include "PostPlayScreen.h"

PostPlayScreen::PostPlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

	gScreenData	=	Setup;
}

void PostPlayScreen::Update(float DeltaTime)
{
	Controller*	controller	=	InputManager::GetInstance()->GetController(0);

	if( controller->GetButtonState( A ) == PRESSED )
		gGotoNextFrame	=	MAIN_MENU_SCREEN;

}

void PostPlayScreen::Render()
{
	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.1f);
	DrawString(*gTextInstance, "LOL U BE DEEEEAD!!!!", tPos.x, tPos.y, 72, Yellow, Black, 2, FW1_CENTER);
	tPos.y	=	gScreenHeight * 0.5f;
	DrawString(*gTextInstance, "Press (A) to go back to the main menu.", tPos.x, tPos.y, 48, White, Black, 2, FW1_CENTER);
}

ScreenType PostPlayScreen::GetScreenType()
{
	return POST_PLAY_SCREEN;
}
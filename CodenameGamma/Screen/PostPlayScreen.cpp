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
	tPos.y	=	gScreenHeight * 0.9f;
	DrawString(*gTextInstance, "Press (A) to go back to the main menu.", tPos.x, tPos.y, 48, White, Black, 2, FW1_CENTER);
	tPos.y	=	gScreenHeight * 0.25f;

	int	textSize	=	28;
	for( int i = 0; i < gScreenData->PLAYER_SCORE_LIST.size(); ++i )
	{
		PlayerScore	tScore	=	gScreenData->PLAYER_SCORE_LIST[i];
		if( !tScore.IsValid() )
			continue;

		if( i == 0 || i == 2 )
		{
			tPos.x	=	gScreenWidth * 0.20f;
			tPos.y	=	gScreenHeight * 0.4f + i * gScreenHeight * 0.05f;
			DrawString(*gTextInstance, "Player " + to_string( (long double)tScore.PlayerIndex + 1), tPos.x, tPos.y + (2 * i + 0) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Kill Count: " + to_string( (long double)tScore.PlayKillCount ), tPos.x, tPos.y + (2 * i + 1) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Score: " + to_string( (long double)tScore.PlayScore ), tPos.x, tPos.y + (2 * i + 2) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Play Time: " + to_string( (long double)tScore.PlayTime ), tPos.x, tPos.y + (2 * i + 3) * textSize, textSize, White, Black, 2, FW1_LEFT);
		}
		else if( i == 1 || i == 3 )
		{
			tPos.x	=	gScreenWidth * 0.60f;
			tPos.y	=	gScreenHeight * 0.4f + (i - 1) * gScreenHeight * 0.05f;
			DrawString(*gTextInstance, "Player " + to_string( (long double)tScore.PlayerIndex + 1 ), tPos.x, tPos.y + (2 * (i - 1) + 0) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Kill Count: " + to_string( (long double)tScore.PlayKillCount ), tPos.x, tPos.y + (2 * (i - 1) + 1) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Score: " + to_string( (long double)tScore.PlayScore ), tPos.x, tPos.y + (2 * (i - 1) + 2) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Play Time: " + to_string( (long double)tScore.PlayTime ), tPos.x, tPos.y + (2 * (i - 1) + 3) * textSize, textSize, White, Black, 2, FW1_LEFT);
		}

	}
}

ScreenType PostPlayScreen::GetScreenType()
{
	return POST_PLAY_SCREEN;
}
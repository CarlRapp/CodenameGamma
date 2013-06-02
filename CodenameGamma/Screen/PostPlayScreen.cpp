#include "PostPlayScreen.h"

PostPlayScreen::PostPlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

	gScreenData	=	Setup;
}

bool PostPlayScreen::Load()
{
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/POST_PLAY_SCREEN.png", 0, 0, &gBackground, 0 );

	IFW1Factory				*pFW1Factory = 0;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(gDevice, L"Apocalypse 1", &gTextInstance);
	pFW1Factory->Release();

	return true;
}

bool PostPlayScreen::Unload()
{
	SAFE_RELEASE( gBackground );
	SAFE_RELEASE( gTextInstance );

	return true;
}

void PostPlayScreen::Update(float DeltaTime)
{
	Controller*	tC	=	InputManager::GetInstance()->GetController(0);
	Keyboard*	tK	=	InputManager::GetInstance()->GetKeyboard();

	bool	UP		=	tC->GetButtonState( D_UP ) == PRESSED		|| tK->GetKeyState( VK_UP ) == PRESSED		|| tK->GetKeyState( 'W' ) == PRESSED;
	bool	DOWN	=	tC->GetButtonState( D_DOWN ) == PRESSED		|| tK->GetKeyState( VK_DOWN ) == PRESSED	|| tK->GetKeyState( 'S' ) == PRESSED;
	bool	CONFIRM	=	tC->GetButtonState( A ) == PRESSED			|| tK->GetKeyState( VK_RETURN ) == PRESSED;
	bool	BACK	=	tC->GetButtonState( B ) == PRESSED			|| tK->GetKeyState( VK_ESCAPE ) == PRESSED;

	if( CONFIRM )
	{
		gGotoNextFrame	=	MAIN_MENU_SCREEN;
		gScreenData->PLAYER_SCORE_LIST.clear();
	}
}

void PostPlayScreen::Render()
{
	gGraphicsManager->RenderQuad( gFullscreenVP, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );

	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.1f);
	DrawString(*gTextInstance, "Game Over!", tPos.x, tPos.y, 72, White, WhiteTrans, 2, FW1_CENTER);

	int	textSize	=	28;
	for( int i = 0; i < gScreenData->PLAYER_SCORE_LIST.size(); ++i ) 
	{
		PlayerScore	tScore	=	gScreenData->PLAYER_SCORE_LIST[i];
		if( !tScore.IsValid() )
			continue;

		if( i == 0 || i == 2 )
		{
			tPos.x	=	gScreenWidth * 0.20f;
			tPos.y	=	gScreenHeight * 0.30f + i * gScreenHeight * 0.05f;
			DrawString(*gTextInstance, "Player " + to_string( (long double)tScore.PlayerIndex + 1), tPos.x, tPos.y + (2 * i + 0) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Kill Count: " + to_string( (long double)tScore.PlayKillCount ), tPos.x, tPos.y + (2 * i + 1) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Score: " + to_string( (long double)tScore.PlayScore ), tPos.x, tPos.y + (2 * i + 2) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Play Time: " + to_string( (long double)tScore.PlayTime ), tPos.x, tPos.y + (2 * i + 3) * textSize, textSize, White, Black, 2, FW1_LEFT);
		}
		else if( i == 1 || i == 3 )
		{
			tPos.x	=	gScreenWidth * 0.60f;
			tPos.y	=	gScreenHeight * 0.30f + (i - 1) * gScreenHeight * 0.05f;
			DrawString(*gTextInstance, "Player " + to_string( (long double)tScore.PlayerIndex + 1 ), tPos.x, tPos.y + (2 * (i - 1) + 0) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Kill Count: " + to_string( (long double)tScore.PlayKillCount ), tPos.x, tPos.y + (2 * (i - 1) + 1) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Score: " + to_string( (long double)tScore.PlayScore ), tPos.x, tPos.y + (2 * (i - 1) + 2) * textSize, textSize, White, Black, 2, FW1_LEFT);
			DrawString(*gTextInstance, "Play Time: " + to_string( (long double)tScore.PlayTime ), tPos.x, tPos.y + (2 * (i - 1) + 3) * textSize, textSize, White, Black, 2, FW1_LEFT);
		}

	}

	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.9f);
	DrawString(*gTextInstance, "Press (A) to go back to the main menu.", tPos.x, tPos.y, 32, White, WhiteTrans, 2, FW1_CENTER);
}

ScreenType PostPlayScreen::GetScreenType()
{
	return POST_PLAY_SCREEN;
}
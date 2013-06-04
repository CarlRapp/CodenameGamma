#include "CreditsScreen.h"

CreditsScreen::CreditsScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

	gScreenData	=	Setup;
}

bool CreditsScreen::Load()
{
	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/CREDITS_SCREEN.png", 0, 0, &gBackground, 0 );

	IFW1Factory				*pFW1Factory = 0;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(gDevice, L"Apocalypse 1", &gTextInstance);
	pFW1Factory->CreateFontWrapper(gDevice, L"Visitor TT1 BRK", &gWaveTextWrapper);
	pFW1Factory->Release();

	return true;
}

bool CreditsScreen::Unload()
{
	SAFE_RELEASE( gBackground );
	SAFE_RELEASE( gTextInstance );
	SAFE_RELEASE( gWaveTextWrapper );

	return true;
}

void CreditsScreen::Update(float DeltaTime)
{
	Controller*	tC	=	InputManager::GetInstance()->GetController(0);
	Keyboard*	tK	=	InputManager::GetInstance()->GetKeyboard();

	bool	UP		=	tC->GetButtonState( D_UP ) == PRESSED		|| tK->GetKeyState( VK_UP ) == PRESSED		|| tK->GetKeyState( 'W' ) == PRESSED;
	bool	DOWN	=	tC->GetButtonState( D_DOWN ) == PRESSED		|| tK->GetKeyState( VK_DOWN ) == PRESSED	|| tK->GetKeyState( 'S' ) == PRESSED;
	bool	CONFIRM	=	tC->GetButtonState( A ) == PRESSED			|| tK->GetKeyState( VK_RETURN ) == PRESSED;
	bool	BACK	=	tC->GetButtonState( B ) == PRESSED			|| tK->GetKeyState( VK_ESCAPE ) == PRESSED;

	if( BACK || CONFIRM )
		gGotoNextFrame	=	MAIN_MENU_SCREEN;
}

void CreditsScreen::Render()
{
	gGraphicsManager->RenderQuad( gFullscreenVP, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );

	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.025f);
	DrawString(*gTextInstance, "Credits", tPos.x, tPos.y, 72, Yellow, YellowTrans, 2, FW1_CENTER);
	
	tPos	=	XMFLOAT2( gScreenWidth * 0.25f, gScreenHeight * 0.035f + 72 * 1.5f );
	DrawString(*gTextInstance, "Carl Rapp", tPos.x, tPos.y, 36, Yellow, RedTrans, 2, FW1_CENTER);
	tPos.y	+=	36 * 1.5f;
	DrawString(*gWaveTextWrapper, "Project Leader, Coding, Input / Screens / Gameplay", tPos.x, tPos.y, 18, White, WhiteTrans, 1, FW1_CENTER);
	tPos.y	+=	36 * 2.5f;

	DrawString(*gTextInstance, "Carl Hakansson", tPos.x, tPos.y, 36, Yellow, RedTrans, 2, FW1_CENTER);
	tPos.y	+=	36 * 1.5f;
	DrawString(*gWaveTextWrapper, "Coding, Editor / Gameplay", tPos.x, tPos.y, 18, White, WhiteTrans, 1, FW1_CENTER);
	tPos.y	+=	36 * 2.5f;

	DrawString(*gTextInstance, "Erik Nilsson", tPos.x, tPos.y, 36, Yellow, RedTrans, 2, FW1_CENTER);
	tPos.y	+=	36 * 1.5f;
	DrawString(*gWaveTextWrapper, "Coding, Graphics / Anmiations / Gameplay", tPos.x, tPos.y, 18, White, WhiteTrans, 1, FW1_CENTER);
	tPos.y	+=	36 * 2.5f;

	tPos	=	XMFLOAT2( gScreenWidth * 0.75f, gScreenHeight * 0.035f + 72 * 1.5f );
	DrawString(*gTextInstance, "Lucas Linderoth", tPos.x, tPos.y, 36, Yellow, RedTrans, 2, FW1_CENTER);
	tPos.y	+=	36 * 1.5f;
	DrawString(*gWaveTextWrapper, "Lead Graphic Artist, Animated Characters, Objects", tPos.x, tPos.y, 18, White, WhiteTrans, 1, FW1_CENTER);
	tPos.y	+=	36 * 2.5f;

	DrawString(*gTextInstance, "Axel Ernstedt", tPos.x, tPos.y, 36, Yellow, RedTrans, 2, FW1_CENTER);
	tPos.y	+=	36 * 1.5f;
	DrawString(*gWaveTextWrapper, "Artist, Structures / Vehicles", tPos.x, tPos.y, 18, White, WhiteTrans, 1, FW1_CENTER);
	tPos.y	+=	36 * 2.5f;

	DrawString(*gTextInstance, "Karl Abom", tPos.x, tPos.y, 36, Yellow, RedTrans, 2, FW1_CENTER);
	tPos.y	+=	36 * 1.5f;
	DrawString(*gWaveTextWrapper, "Lead Game Designer, Artist, Structures / Vehicles / Rat", tPos.x, tPos.y, 18, White, WhiteTrans, 1, FW1_CENTER);


	tPos	=	XMFLOAT2( gScreenWidth * 0.35f, tPos.y + 36 * 2.5f);
	DrawString(*gTextInstance, "Linus Thorell", tPos.x, tPos.y, 36, Yellow, RedTrans, 2, FW1_CENTER);
	tPos.y	+=	36 * 1.5f;
	DrawString(*gWaveTextWrapper, "Musician, Main Theme", tPos.x, tPos.y, 18, White, WhiteTrans, 1, FW1_CENTER);

	tPos	=	XMFLOAT2( gScreenWidth * 0.65f, tPos.y - 36 * 1.5f);
	DrawString(*gTextInstance, "Michael Mostrom", tPos.x, tPos.y, 36, Yellow, RedTrans, 2, FW1_CENTER);
	tPos.y	+=	36 * 1.5f;
	DrawString(*gWaveTextWrapper, "Musician, Sound Effects", tPos.x, tPos.y, 18, White, WhiteTrans, 1, FW1_CENTER);



	

	tPos	=	XMFLOAT2(gScreenWidth * 0.5f, gScreenHeight * 0.90f);
	DrawString(*gTextInstance, "Press (A) to go back to the main menu.", tPos.x, tPos.y, 32, White, WhiteTrans, 2, FW1_CENTER);
}

ScreenType CreditsScreen::GetScreenType()
{
	return CREDITS_SCREEN;
}
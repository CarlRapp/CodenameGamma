#include "PlayScreen.h"

PlayScreen::PlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);
	gScreenData	=	Setup;
	gScreenData->PLAYER_SCORE_LIST.clear();

	SystemData	tData			=	SystemData();
	tData.DEVICE				=	gDevice;
	tData.DEVICE_CONTEXT		=	gDeviceContext;
	tData.RENDER_TARGET_VIEW	=	gRenderTargetView;
	tData.SCREEN_WIDTH			=	gScreenWidth;
	tData.SCREEN_HEIGHT			=	gScreenHeight;

	gLevel	=	new Level(tData);
	gLevel->LoadLevel("City_2");

	SetNumberOfPlayers(Setup->NUMBER_OF_PLAYERS);
}

#pragma region Load / Unload
bool PlayScreen::Load()
{
	for( int n = 0; n < 10; ++n)
	{
		string	tPath	=	"DATA/GUI/Health/Health_" + to_string( (long double) n + 1 ) + ".png";
		D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, tPath.c_str(), 0, 0, &gHealthBar[n], 0 );
	}
	return true;
}

bool PlayScreen::Unload()
{
	
	if ( gLevel )
		delete	gLevel;

	for( int n = 0; n < 10; ++n)
	{
		gHealthBar[n]->Release();
	}
		
	return true;
}
#pragma endregion
void PlayScreen::Update(float DeltaTime)
{
	gLevel->Update(DeltaTime);


	if ( InputManager::GetInstance()->GetController(0)->GetButtonState( START ) == PRESSED ||
		InputManager::GetInstance()->GetKeyboard()->GetKeyState(VK_ESCAPE) == PRESSED)
		gGotoNextFrame	=	MAIN_MENU_SCREEN;
	
	if( gLevel->IsGameOver() )
	{
		vector<PlayerScore>	PlayerScores	=	vector<PlayerScore>();
		for each( Player* p in gLevel->GetPlayers() )
			gScreenData->PLAYER_SCORE_LIST.push_back( *p->GetPlayerScore() );

		gGotoNextFrame	=	POST_PLAY_SCREEN;
	}

	if( InputManager::GetInstance()->GetController(0)->GetButtonState( A ) == PRESSED )
		if( gLevel->GetPlayers()[0]->GetUnit() )
			gLevel->GetPlayers()[0]->GetUnit()->Hurt( 10 );
}

void PlayScreen::Render()
{
	gLevel->Render();

	for each( Player* p in gLevel->GetPlayers() )
		RenderGUI( p );

	/*
	for each( Player* p in gLevel->GetPlayers() )
	{
		PlayerUnit*	tUnit	=	p->GetUnit();
		if( tUnit == 0 )
			continue;

		UnitHealth	HEALTH	=	tUnit->GetHealth();
		UnitHunger	HUNGER	=	tUnit->GetHungerMeter();
		UnitThirst	THIRST	=	tUnit->GetThirstMeter();

		D3D11_VIEWPORT	VP	=	p->GetCamera()->GetViewPort();

		for ( int i = 0; i < 3; ++i)
		{
			float	a, b;
			string title;
			if ( i == 0 )
			{
				title	=	"Health: ";
				a = (int)HEALTH.first;
				b = (int)HEALTH.second;
			}
			else if ( i == 1 )
			{
				title	=	"Hunger: ";
				a = (int)HUNGER.first;
				b = (int)HUNGER.second;
			}
			else if ( i == 2)
			{
				title	=	"Thirst: ";
				a = (int)THIRST.first;
				b = (int)THIRST.second;
			}
			DrawString(
				*gTextInstance,
				title + to_string((long double)a) + " / " + to_string((long double)b),
				VP.TopLeftX + 10,
				VP.TopLeftY + 10 + i * 20,
				20,
				White,
				0
			);
		}
	}*/
}

ScreenType PlayScreen::GetScreenType()
{
	return PLAY_SCREEN;
}

void PlayScreen::Reset()
{
	if( gLevel->IsGameOver() )
		SetNumberOfPlayers( gScreenData->NUMBER_OF_PLAYERS );
}

void PlayScreen::RenderGUI( Player* P )
{
	if( !P->GetUnit() )
		return;

	GraphicsManager*	GM	=	gLevel->GetGraphicsManager();
	D3D11_VIEWPORT		pVP	=	P->GetCamera()->GetViewPort();

	D3D11_VIEWPORT	tVP;
	tVP.TopLeftX	=	pVP.TopLeftX;
	tVP.TopLeftY	=	pVP.TopLeftY;
	tVP.MinDepth	=	0.0f;
	tVP.MaxDepth	=	1.0f;
	tVP.Width		=	70;
	tVP.Height		=	58;

	UnitHealth	uHealth	=	P->GetUnit()->GetHealth();
	int HP		=	(int)( 100.0f * (uHealth.first/uHealth.second) );
	int	index	=	HP / 10;
	if( index > 0 )
	{
		GM->RenderQuad( tVP, gHealthBar[index - 1], Effects::CombineFinalFX->AlphaClipColorTech );

		DrawString(
			*gTextInstance,
			to_string( (long double)HP ),
			tVP.TopLeftX + tVP.Width * 0.5f,
			tVP.TopLeftY + tVP.Height * 0.5f,
			15,
			White,
			Black,
			1.0f,
			FW1_CENTER | FW1_VCENTER
		);
	}
}
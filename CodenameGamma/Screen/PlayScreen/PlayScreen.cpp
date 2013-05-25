#include "PlayScreen.h"

PlayScreen::PlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);
	gScreenData	=	Setup;
	
}

#pragma region Load / Unload
bool PlayScreen::Load()
{
	string	tPath;
	for( int n = 0; n < 10; ++n)
	{
		tPath	=	"DATA/GUI/Health/Health_" + to_string( (long double) n + 1 ) + ".png";
		D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, tPath.c_str(), 0, 0, &gHealthBar[n], 0 );
	}
	for( int n = 0; n <= 5; ++n)
	{
		tPath	=	"DATA/GUI/Hunger/Hunger_" + to_string( (long double) n ) + ".png";
		D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, tPath.c_str(), 0, 0, &gHungerBar[n], 0 );

		tPath	=	"DATA/GUI/Thirst/Thirst_" + to_string( (long double) n ) + ".png";
		D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, tPath.c_str(), 0, 0, &gThirstBar[n], 0 );
	}

	SystemData	tData			=	SystemData();
	tData.DEVICE				=	gDevice;
	tData.DEVICE_CONTEXT		=	gDeviceContext;
	tData.RENDER_TARGET_VIEW	=	gRenderTargetView;
	tData.SCREEN_WIDTH			=	gScreenWidth;
	tData.SCREEN_HEIGHT			=	gScreenHeight;

	gLevel	=	new Level(tData);
	gLevel->LoadLevel( gScreenData->LEVEL_NAME );
	gScreenData->PLAYER_SCORE_LIST.clear();

	SetNumberOfPlayers(gScreenData->NUMBER_OF_PLAYERS);

	return true;
}

bool PlayScreen::Unload()
{
	
	if ( gLevel )
		delete	gLevel;

	for( int n = 0; n < 10; ++n)
	{
		gHealthBar[n]->Release();

		if( n <= 5 )
		{
			gHungerBar[n]->Release();
			gThirstBar[n]->Release();
		}
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
			gLevel->GetPlayers()[0]->GetUnit()->Hurt( 1 );
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

	XMFLOAT2	tHealthPos, tHungerPos, tThirstPos;
	D3D11_VIEWPORT		pVP		=	P->GetCamera()->GetViewPort();

	D3D11_VIEWPORT	tVP;
	tVP.TopLeftX	=	pVP.TopLeftX + pVP.Height * 0.014f;
	tVP.TopLeftY	=	pVP.TopLeftY + pVP.Height * 0.014f;
	tVP.MinDepth	=	0.0f;
	tVP.MaxDepth	=	1.0f;
	tVP.Width		=	pVP.Height * 0.11f;
	tVP.Height		=	pVP.Height * 0.09f;

	//	The stats
	UnitHealth			uHealth	=	P->GetUnit()->GetHealth();
	UnitHunger			uHunger	=	P->GetUnit()->GetHungerMeter();
	UnitThirst			uThirst	=	P->GetUnit()->GetThirstMeter();
	int	tPercent;
	int	tIndex;
	
	//	Health
	tPercent	=	(int)ceil( 100.f * ( uHealth.first / uHealth.second ) );
	tPercent	=	MathHelper::Clamp(0, tPercent, 100);
	tIndex		=	(int)( (float)0.1f * (float)tPercent );
	tIndex		=	MathHelper::Clamp(0, tIndex, 9);
	RenderGUISprite( tVP, gHealthBar[tIndex] );
	tHealthPos.x	=	tVP.TopLeftX + tVP.Width * 0.5f;
	tHealthPos.y	=	tVP.TopLeftY + tVP.Height * 0.5f;
	
	//	Move the viewport	
	tVP.TopLeftX	=	pVP.TopLeftX + pVP.Height * 0.02f;
	tVP.TopLeftY	=	pVP.TopLeftY + pVP.Height * 0.111f;
	tVP.Width		=	pVP.Height * 0.035f;
	tVP.Height		=	pVP.Height * 0.0695f;


	//	Hunger
	tPercent	=	(int)ceil( 100.f * ( uHunger.first / uHunger.second ) );
	tPercent	=	MathHelper::Clamp(0, tPercent, 100);
	tIndex		=	(int)( 0.05f * tPercent );
	tIndex		=	MathHelper::Clamp(0, tIndex, 5);
	RenderGUISprite( tVP, gHungerBar[tIndex] );
	tHungerPos.x	=	tVP.TopLeftX + tVP.Width * 0.5f;
	tHungerPos.y	=	tVP.TopLeftY + tVP.Height * 0.5f;

	//	Move the viewport
	tVP.TopLeftX	=	pVP.TopLeftX + pVP.Height * 0.083f;
	tVP.TopLeftY	=	pVP.TopLeftY + pVP.Height * 0.111f;
	tVP.Width		=	pVP.Height * 0.035f;
	tVP.Height		=	pVP.Height * 0.0695f;

	//	Thirst
	tPercent	=	(int)ceil( 100.f * ( uThirst.first / uThirst.second ) );
	tPercent	=	MathHelper::Clamp(0, tPercent, 100);
	tIndex		=	(int)( 0.05f * tPercent );
	tIndex		=	MathHelper::Clamp(0, tIndex, 5);
	gLevel->GetGraphicsManager()->RenderQuad( tVP, gThirstBar[tIndex], Effects::CombineFinalFX->AlphaTransparencyColorTech );
	//RenderGUISprite( tVP,  );
	tThirstPos.x	=	tVP.TopLeftX + tVP.Width * 0.5f;
	tThirstPos.y	=	tVP.TopLeftY + tVP.Height * 0.5f;

	//	Render the text
	//RenderGUIText( tHealthPos, to_string( (long double)( (int)(100.0f * ( uHealth.first / uHealth.second ) ) ) ), 18, White );
	//RenderGUIText( tHungerPos, to_string( (long double)( (int)(100.0f * ( uHunger.first / uHunger.second ) ) ) ), 10, White );
	//RenderGUIText( tThirstPos, to_string( (long double)( (int)(100.0f * ( uThirst.first / uThirst.second ) ) ) ), 10, White );
}

void PlayScreen::RenderGUISprite( D3D11_VIEWPORT VP, ID3D11ShaderResourceView* Sprite )
{
	gLevel->GetGraphicsManager()->RenderQuad( VP, Sprite, Effects::CombineFinalFX->AlphaClipColorTech );
}

void PlayScreen::RenderGUIText( XMFLOAT2 Position, string Text, float TextSize, TextColor Color )
{
	DrawString(
		*gTextInstance,
		Text,
		Position.x,
		Position.y,
		TextSize,
		Color,
		Black,
		2.0f,
		FW1_CENTER | FW1_VCENTER
	);
}
/*
void PlayScreen::RenderHunger( Player* P )
{
	GraphicsManager*	GM		=	gLevel->GetGraphicsManager();
	D3D11_VIEWPORT		pVP		=	P->GetCamera()->GetViewPort();
	UnitHunger			uHunger	=	P->GetUnit()->GetHungerMeter();

	D3D11_VIEWPORT	tVP;
	tVP.TopLeftX	=	pVP.TopLeftX + 200;
	tVP.TopLeftY	=	pVP.TopLeftY;
	tVP.MinDepth	=	0.0f;
	tVP.MaxDepth	=	1.0f;
	tVP.Width		=	50;
	tVP.Height		=	100;

	

	int	tPercent	=	( uHunger.first/uHunger.second );
	int	tIndex		=	(int)( 0.05f * ceil( 100.0f * tPercent ) );

	GM->RenderQuad( tVP, gHungerBar[tIndex], Effects::CombineFinalFX->AlphaClipColorTech );

	DrawString(
		*gTextInstance,
		to_string( (long double)( 100.0f * tPercent ) ),
		tVP.TopLeftX + tVP.Width * 0.5f,
		tVP.TopLeftY + tVP.Height * 0.5f,
		15,
		White,
		Black,
		2.0f,
		FW1_CENTER | FW1_VCENTER
	);
}

void PlayScreen::RenderThirst( Player* P )
{
	GraphicsManager*	GM		=	gLevel->GetGraphicsManager();
	D3D11_VIEWPORT		pVP		=	P->GetCamera()->GetViewPort();
	UnitThirst			uThirst	=	P->GetUnit()->GetThirstMeter();

	D3D11_VIEWPORT	tVP;
	tVP.TopLeftX	=	pVP.TopLeftX + 200;
	tVP.TopLeftY	=	pVP.TopLeftY;
	tVP.MinDepth	=	0.0f;
	tVP.MaxDepth	=	1.0f;
	tVP.Width		=	90;
	tVP.Height		=	169;

	

	int	tPercent	=	( uThirst.first/uThirst.second );
	int	tIndex		=	(int)( 0.05f * ceil( 100.0f * tPercent ) );

	GM->RenderQuad( tVP, gThirstBar[tIndex], Effects::CombineFinalFX->AlphaClipColorTech );

	DrawString(
		*gTextInstance,
		to_string( (long double)( 100.0f * tPercent ) ),
		tVP.TopLeftX + tVP.Width * 0.5f,
		tVP.TopLeftY + tVP.Height * 0.5f,
		15,
		White,
		Black,
		2.0f,
		FW1_CENTER | FW1_VCENTER
	);
}*/
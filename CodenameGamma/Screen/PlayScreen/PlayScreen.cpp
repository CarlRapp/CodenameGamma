#include "PlayScreen.h"

PlayScreen::PlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);
	gScreenData	=	Setup;

	SystemData	tData			=	SystemData();
	tData.DEVICE				=	gDevice;
	tData.DEVICE_CONTEXT		=	gDeviceContext;
	tData.RENDER_TARGET_VIEW	=	gRenderTargetView;
	tData.SCREEN_WIDTH			=	gScreenWidth;
	tData.SCREEN_HEIGHT			=	gScreenHeight;

	gLevel	=	new Level(tData);
	gLevel->LoadLevel("FlatMap");
	//gLevel->LoadLevel("TestMap");

	/*
	gModel = new Model(gDevice, gTexMgr, "DATA\\Models\\obj\\pacman.obj", "DATA/Models/Textures/");

	for (int i = 0; i < 10; ++i)
	{
		float x = MathHelper::RandF(0, 4000);
		float y = 12;
		float z = MathHelper::RandF(0, 4000);

		AddInstance(x, y, z, gModel);
	}

	AddDirectionalLight();
	AddPointLight();
	AddSpotLight();
	*/

	SetNumberOfPlayers(Setup->NUMBER_OF_PLAYERS);
	/*
	for (int i = 0; i < gPlayers.size(); ++i)
	{
		if (gLevel->GetGameObjects().size() > i)
		{
			gPlayers.at(i)->SetGameObject(gLevel->GetGameObjects().at(i));		
			gLevel->GetGameObjects().at(i)->SetTeam(Team1);
		}
	}
	*/
}

#pragma region Load / Unload
bool PlayScreen::Load()
{
	return true;
}

bool PlayScreen::Unload()
{
	
	if ( gLevel )
		delete	gLevel;
		
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
		gGotoNextFrame	=	POST_PLAY_SCREEN;
}

void PlayScreen::Render()
{
	gLevel->Render();


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
	}
}

ScreenType PlayScreen::GetScreenType()
{
	return PLAY_SCREEN;
}

void PlayScreen::Reset()
{
	SetNumberOfPlayers(gScreenData->NUMBER_OF_PLAYERS);
}
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

}

void PlayScreen::Render()
{
	gLevel->Render();


	PlayerUnit*	DERP	=	gLevel->GetPlayers()[0]->GetUnit();
	UnitHunger	HUNGER	=	DERP->GetHungerMeter();
	UnitThirst	THIRST	=	DERP->GetThirstMeter();
	DrawString(
		*gTextInstance,
		"Foods: " + to_string((long double)HUNGER.first) + " / " + to_string((long double)HUNGER.second),
		10,
		10,
		20,
		White,
		0
	);

	DrawString(
		*gTextInstance,
		"THIRSTZ: " + to_string((long double)THIRST.first) + " / " + to_string((long double)THIRST.second),
		10,
		40,
		20,
		White,
		0
	);
}

ScreenType PlayScreen::GetScreenType()
{
	return PLAY_SCREEN;
}

void PlayScreen::Reset()
{
	SetNumberOfPlayers(gScreenData->NUMBER_OF_PLAYERS);
}
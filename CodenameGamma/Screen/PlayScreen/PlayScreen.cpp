#include "PlayScreen.h"

PlayScreen::PlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

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

	SetNumberOfPlayers(1);
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
	if ( gGameObjects.size() > 0)
	{
		for each( GameObject* GO in gGameObjects)
			delete GO;
		gGameObjects.clear();
	}
	
	if ( gLevel )
		delete	gLevel;
		
	return true;
}
#pragma endregion

void PlayScreen::Update(float DeltaTime)
{
	gLevel->Update(DeltaTime);
}

void PlayScreen::Render()
{
	gLevel->Render();
}

ScreenType PlayScreen::GetScreenType()
{
	return PLAY_SCREEN;
}
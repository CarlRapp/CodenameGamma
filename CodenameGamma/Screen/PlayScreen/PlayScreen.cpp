#include "PlayScreen.h"


PlayScreen::PlayScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

	Effects::InitAll(gDevice);
	RenderStates::InitAll(gDevice);
	InputLayouts::InitAll(gDevice);
	gTexMgr.Init(gDevice);

	gTerrain = new Terrain(gDevice, gDeviceContext);
	gTerrain->LoadMap("DATA\\Maps\\TestMap");

	gGraphicsManager = new GraphicsManager(gDevice, gDeviceContext, gRenderTargetView, gDepthStencilView, gScreenWidth, gScreenHeight);
	gGraphicsManager->SetTerrain(gTerrain);
	SetNumberOfPlayers(1);

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
	for each (Player *p in gPlayers)
	{
		p->Update(DeltaTime);
	}
}

void PlayScreen::Render()
{
	gGraphicsManager->Render(gPlayers);
}

ScreenType PlayScreen::GetScreenType()
{
	return PLAY_SCREEN;
}

void PlayScreen::SetNumberOfPlayers(int noPlayers)
{
	gPlayers.clear();
	Player *player1 =  new Player(0);
	Player *player2 = new Player(1);
	Player *player3 = new Player(2);
	Player *player4 = new Player(3);
	switch (noPlayers)
	{
		case 1:
			player1->SetScreen(0,					0,						gScreenWidth, gScreenHeight);	
			gPlayers.push_back(player1);
			break;
		case 2:
			player1->SetScreen(0,					0,						gScreenWidth / 2, gScreenHeight);	
			player2->SetScreen(gScreenWidth / 2,	0,						gScreenWidth / 2, gScreenHeight);
			gPlayers.push_back(player1);	
			gPlayers.push_back(player2);
			break;
		case 3:
			player1->SetScreen(0,					0,						gScreenWidth,	  gScreenHeight / 2);	
			player2->SetScreen(0,					gScreenHeight / 2,		gScreenWidth / 2, gScreenHeight / 2);
			player3->SetScreen(gScreenWidth / 2,	gScreenHeight / 2,		gScreenWidth / 2, gScreenHeight / 2);
			gPlayers.push_back(player1);	
			gPlayers.push_back(player2);
			gPlayers.push_back(player3);
			break;
		case 4:
			player1->SetScreen(0,					0,						gScreenWidth / 2, gScreenHeight / 2);	
			player2->SetScreen(gScreenWidth / 2,	0,						gScreenWidth / 2, gScreenHeight / 2);
			player3->SetScreen(0,					gScreenHeight / 2,		gScreenWidth / 2, gScreenHeight / 2);
			player4->SetScreen(gScreenWidth / 2,	gScreenHeight / 2,		gScreenWidth / 2, gScreenHeight / 2);
			gPlayers.push_back(player1);	
			gPlayers.push_back(player2);
			gPlayers.push_back(player3);
			gPlayers.push_back(player4);	
			break;
	}
}

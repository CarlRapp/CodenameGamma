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

	
	gModel = new Model(gDevice, gTexMgr, "DATA\\Models\\obj\\pacman.obj", "DATA\\Models\\Textures\\");

	for (int i = 0; i < 10; ++i)
	{
		float x = MathHelper::RandF(0, 4000);
		float y = 12;
		float z = MathHelper::RandF(0, 4000);

		AddInstance(x, y, z, gModel);
	}

	SetNumberOfPlayers(4);

}

void PlayScreen::AddInstance(float x, float y, float z, Model *model)
{
	XMMATRIX scale, rot, trans, world;
	scale	= XMMatrixScaling(10.0f, 10.0f, 10.0f);

	y += gTerrain->GetHeight(x, z);

	rot		= XMMatrixRotationX(0);
	//rot		= XMMatrixRotationX(-PI/2);
	trans	= XMMatrixTranslation(x, y, z);

	ModelInstance *instance = new ModelInstance();
	instance->m_Model = model;	
	//instance->m_World = scale * rot * trans;
	world = XMMatrixMultiply(scale, rot);
	world = XMMatrixMultiply(world, trans);
	XMStoreFloat4x4(&instance->m_World, world);
	instance->m_OldBoundingSphere = instance->GetBoundingSphere();

	gGraphicsManager->AddModelInstance(instance);


	GameObject *go = new GameObject();
	go->m_Position = DirectX::XMFLOAT3(x, y, z);
	go->m_ModelInstance = instance;
	
	float speed = 80;

	if (MathHelper::RandF(0, 1) > 0.0f)
		go->m_Velocity = DirectX::XMFLOAT3(MathHelper::RandF(-speed, speed), 0, MathHelper::RandF(-speed, speed));
	gGameObjects.push_back(go);
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
	//Updaterar gameobjects
	vector<ModelInstance*>	updatedInstances;	
	for each (GameObject *go in gGameObjects)
	{
		//go->Update(deltaTime);
		if (go->Update(DeltaTime, gTerrain))
			updatedInstances.push_back(go->m_ModelInstance);
	}
	gGraphicsManager->Update(updatedInstances);

	for each (Player *p in gPlayers)
	{
		p->Update(DeltaTime);
	}


	Controller* controller = InputManager::GetInstance()->GetController(0);
	if (controller->GetButtonState(Xbox_Button::B) == InputState::DOWN)
	//if (GetAsyncKeyState('L'))
	{
		if (!gGameObjects.empty())
		{
			ModelInstance *instance = gGameObjects.back()->m_ModelInstance;
			gGraphicsManager->RemoveModelInstance(instance);
			gGameObjects.pop_back();
		}
	}
	if (controller->GetButtonState(Xbox_Button::A) == InputState::DOWN)
	//if (GetAsyncKeyState('K'))
		AddInstance(MathHelper::RandF(0, 4000), 12, MathHelper::RandF(0, 4000), gModel);
	/*
	if (controller->GetButtonState(Xbox_Button::START) == InputState::DOWN)
		gGotoNextFrame = MAIN_MENU_SCREEN;
		*/
	
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

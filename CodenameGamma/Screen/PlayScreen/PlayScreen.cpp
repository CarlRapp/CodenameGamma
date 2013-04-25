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


	gGraphicsManager = new GraphicsManager(gDevice, gDeviceContext, gRenderTargetView, gScreenWidth, gScreenHeight);
	gGraphicsManager->SetTerrain(gTerrain);
	gGraphicsManager->SetLights(&gDirLights, &gPointLights, &gSpotLights);

	
	gModel = new Model(gDevice, gTexMgr, "DATA\\Models\\obj\\pacman.obj", "DATA\\Models\\Textures\\");

	for (int i = 0; i < 10; ++i)
	{
		float x = MathHelper::RandF(0, 4000);
		float y = 12;
		float z = MathHelper::RandF(0, 4000);

		AddInstance(x, y, z, gModel);
	}

	AddDirectionalLight();

	SetNumberOfPlayers(1);
}

void PlayScreen::AddDirectionalLight()
{
	float x = MathHelper::RandF(-10.0f, 10.0f);
	float z = MathHelper::RandF(-10.0f, 10.0f);
	/*
	float r = MathHelper::RandF(0.0f, 1.0f);
	float g = MathHelper::RandF(0.0f, 1.0f);
	float b = MathHelper::RandF(0.0f, 1.0f);
	*/
	
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	

	DirectionalLight *dirLight = new DirectionalLight();
	dirLight->Color = XMFLOAT4(r,g,b,1);
	dirLight->Direction = XMFLOAT4(x, -1.0f, z,0);

	gDirLights.push_back(dirLight);
}


void PlayScreen::AddPointLight()
{
	float x = MathHelper::RandF(0.0f, 4000.0f);
	float z = MathHelper::RandF(0.0f, 4000.0f);
		
	float r = MathHelper::RandF(0.0f, 0.6f);
	float g = MathHelper::RandF(0.0f, 0.6f);
	float b = MathHelper::RandF(0.0f, 0.6f);
	/*
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	*/
	PointLight *pointLight = new PointLight();
	pointLight->Color		= XMFLOAT4(r, g, b, 0);
	pointLight->Position	= XMFLOAT3(x, 0.0f, z);
	pointLight->Range		= 250;
	gPointLights.push_back(pointLight);

	float Ox = pointLight->Position.x + MathHelper::RandF(-300.0f, 300.0f);
	float Oz = pointLight->Position.z + MathHelper::RandF(-300.0f, 300.0f);

	rotpos.push_back(XMFLOAT3(Ox, 0.0f, Oz));
	angle.push_back(MathHelper::RandF(-45.0f, 45.0f));
}

void PlayScreen::AddSpotLight()
{
	float x = MathHelper::RandF(0.0f, 4000.0f);
	float z = MathHelper::RandF(0.0f, 4000.0f);
		
	float r = MathHelper::RandF(0.0f, 1.0f);
	float g = MathHelper::RandF(0.0f, 1.0f);
	float b = MathHelper::RandF(0.0f, 1.0f);
	/*
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	*/



	SpotLight *spotLight	= new SpotLight();
	spotLight->Color		= XMFLOAT4(r, g, b, 0.0f);
	spotLight->Position		= XMFLOAT3(x, 0, z);
	spotLight->Direction	= XMFLOAT3(MathHelper::RandF(-1.0f, 1.0f), -1, MathHelper::RandF(-1.0f, 1.0f));
	spotLight->angle		= XMConvertToRadians(30);
	spotLight->Range		= 500;

	gSpotLights.push_back(spotLight);

	anglespot.push_back(MathHelper::RandF(-90.0f, 90.0f));

	//float Ox = pointLight->Position.x + MathHelper::RandF(-300.0f, 300.0f);
	//float Oz = pointLight->Position.z + MathHelper::RandF(-300.0f, 300.0f);

	//rotpos.push_back(XMFLOAT3(Ox, 0.0f, Oz));
	//angle.push_back(MathHelper::RandF(-45.0f, 45.0f));
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

	
	for (int i = 0; i < gPointLights.size(); ++i)
	{
		PointLight *pLight = gPointLights[i];

		XMFLOAT3 rp = rotpos[i];
		float a		= angle[i];


		XMVECTOR pos  = XMLoadFloat3(&pLight->Position);
		//XMVECTOR move = XMLoadFloat3(&XMFLOAT3(0,0,0.1f));

		
		XMMATRIX transl1 = XMMatrixTranslation(-rp.x, -2000.0f, -rp.z);
		XMMATRIX transl2 = XMMatrixTranslation(rp.x, 2000.0f, rp.z);	
		XMMATRIX rot	 = XMMatrixRotationY(DeltaTime * XMConvertToRadians(a));

		XMMATRIX transf = transl1 * rot * transl2;


		pos = XMVector3TransformCoord(pos, transf);

		//pos = pos + move;

		XMStoreFloat3(&pLight->Position, pos);

		//pLight->Position.z -= 0.1f;
		pLight->Position.y = gTerrain->GetHeight(pLight->Position.x, pLight->Position.z) + 50;
	}

	for (int i = 0; i < gSpotLights.size(); ++i)
	{
		SpotLight *sLight = gSpotLights[i];

		float a		= anglespot[i];

		XMVECTOR dir  = XMLoadFloat3(&sLight->Direction);

		//XMMATRIX transl1 = XMMatrixTranslation(-sLight->Position.x, -2000.0f, -sLight->Position.z);
		//XMMATRIX transl2 = XMMatrixTranslation(sLight->Position.x, 2000.0f, sLight->Position.z);
		XMMATRIX rot	 = XMMatrixRotationY(DeltaTime * XMConvertToRadians(a));

		dir = XMVector3TransformCoord(dir, rot);

		XMStoreFloat3(&sLight->Direction, dir);
		//XMMATRIX transf = transl1 * rot * transl2;

		sLight->Position.y = gTerrain->GetHeight(sLight->Position.x, sLight->Position.z) + 200;
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
		AddInstance(MathHelper::RandF(0, 4000), 12, MathHelper::RandF(0, 4000), gModel);

	if (controller->GetButtonState(Xbox_Button::Y) == InputState::DOWN)
	{
		if (!gPointLights.empty())
		{
			gPointLights.pop_back();
			rotpos.pop_back();
			angle.pop_back();
		}
	}

	if (controller->GetButtonState(Xbox_Button::X) == InputState::DOWN)
		AddPointLight();


	if (controller->GetButtonState(Xbox_Button::LEFT_BUMPER) == InputState::DOWN)
	{
		if (!gSpotLights.empty())
		{
			gSpotLights.pop_back();
			anglespot.pop_back();
		}
	}

	if (controller->GetButtonState(Xbox_Button::RIGHT_BUMPER) == InputState::DOWN)
		AddSpotLight();


	if (controller->GetButtonState(Xbox_Button::D_DOWN) == InputState::PRESSED)
	{
		if (!gDirLights.empty())
		{
			gDirLights.pop_back();
		}
	}

	if (controller->GetButtonState(Xbox_Button::D_UP) == InputState::PRESSED)
			AddDirectionalLight();
		

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

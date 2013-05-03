#include "Level.h"

Level::Level(){}
Level::Level(SystemData LData)
{
	gLData = LData;
	Effects::InitAll(LData.DEVICE);
	RenderStates::InitAll(LData.DEVICE);
	InputLayouts::InitAll(LData.DEVICE);
	gTextureManager.Init(LData.DEVICE);

	gTerrain	=	new Terrain(LData.DEVICE, LData.DEVICE_CONTEXT);

	gGraphicsManager	=	new GraphicsManager(LData.DEVICE, LData.DEVICE_CONTEXT, LData.RENDER_TARGET_VIEW, LData.SCREEN_WIDTH, LData.SCREEN_HEIGHT);
	gGraphicsManager->SetTerrain(gTerrain);
	gGraphicsManager->SetLights(&gDirLights, &gPointLights, &gSpotLights);


	for(int i = 0; i < 5; i++)
		AddDirectionalLight(0);

	for( int i = 0; i < 30; ++i)
	{
		AddPointLight(0);
		AddSpotLight(0);
	}

	gQuadTree = NULL;

	gLoadedModels	=	vector<Model*>();
}

Level::~Level()
{
	if ( gTerrain )
		delete	gTerrain;
	if ( gGraphicsManager )
		delete	gGraphicsManager;

	Effects::DestroyAll();
	RenderStates::DestroyAll();
	InputLayouts::DestroyAll();

	for ( int i = 0; i < gLoadedModels.size(); ++i )
		if ( gLoadedModels.at( i ) )
			delete gLoadedModels.at( i );
	gLoadedModels.clear();
}

void Level::LoadLevel(string Levelname)
{
	//	TODO:	Fix a more dynamic
	//			way to load maps.
	//			I.e. send the extracted
	//			information to each component

	string	tLevelRootPath	=	"DATA/Maps/";

	LevelData	LData	=	LevelParser::ParseLevel(Levelname, tLevelRootPath);

	if ( LData.IsLoaded() )
		gTerrain->LoadTerrain(LData);

	SAFE_DELETE(gQuadTree);
	XMFLOAT2 halfWorldSize = gTerrain->GetDimensions();
	halfWorldSize.x /= 2.0f;
	halfWorldSize.y /= 2.0f;
	BoundingBox world = BoundingBox(XMFLOAT3(halfWorldSize.x, 0, halfWorldSize.y), XMFLOAT3(halfWorldSize.x, 2000, halfWorldSize.y));
	gQuadTree = new QuadTree(world, 8);
	gGraphicsManager->SetQuadTree(gQuadTree);
	Model* model = new Model(gLData.DEVICE, gTextureManager, "DATA\\Models\\obj\\pacman.obj", "DATA/Models/Textures/");

	for (int i = 0; i < 50; ++i)
	{
		float x = MathHelper::RandF(0, 4000);
		float y = 12;
		float z = MathHelper::RandF(0, 4000);

		AddInstance(x, y, z, model);
	}

	gLoadedModels.push_back( model );
}


void Level::AddDirectionalLight(DirectionalLight* Instance)
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
void Level::AddPointLight(PointLight* Instance)
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
void Level::AddSpotLight(SpotLight* Instance)
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

void Level::AddInstance(float x, float y, float z, Model *model)
{
	ModelInstance *instance = new ModelInstance();
	instance->m_Model = model;

	GameObject *go = new GameObject();
	go->MoveTo(DirectX::XMFLOAT3(x, y, z));
	go->SetModelInstance(instance);
	go->SetScale(XMFLOAT3(10, 10, 10));

	instance->m_World	=	go->GetFloat4x4Value(World);

	float speed = 80;

	if (MathHelper::RandF(0, 1) > 0.0f)
		go->SetVelocity(DirectX::XMFLOAT3(MathHelper::RandF(-speed, speed), 0, MathHelper::RandF(-speed, speed)));

	instance->m_OldBoundingSphere = instance->GetBoundingSphere();

	AddGameObject(go);
}

void Level::Update(float DeltaTime)
{
	//Updaterar gameobjects
	vector<GameObject*>	updatedGO;	
	for each (GameObject *go in gGameObjects)
	{
		//go->Update(deltaTime);
		if (go->Update(DeltaTime, gTerrain))
			updatedGO.push_back(go);
	}

	BoundingSphere As, Bs;
	for each (GameObject* A in gGameObjects)
	{
		As	=	A->GetModelInstance()->GetBoundingSphere();

		
		if ( A == gGameObjects[0] )
		for each (GameObject* B in gGameObjects)
		{
			if ( A == B )
				continue;

			Bs	=	B->GetModelInstance()->GetBoundingSphere();

			if ( As.Intersects( Bs ) )
				B->SetState( Dead );
		}
	}

	for each (GameObject *go in updatedGO)
	{
		if ( go->IsAlive() )
			gQuadTree->Update(go);
		else
			gQuadTree->Delete(go);
	}

	for ( int i = gGameObjects.size() - 1; i >= 0; --i )
		if ( !gGameObjects[i]->IsAlive() )
			gGameObjects.erase( gGameObjects.begin() + i );

	//Updaterar ljus
	//Updaterar pointlights
	for (int i = 0; i < (int)gPointLights.size(); ++i)
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

	//Updaterar spotlights
	for (int i = 0; i < (int)gSpotLights.size(); ++i)
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
}
void Level::Render(vector<Camera*>& Cameras)
{
	gGraphicsManager->Render(Cameras);
}



Terrain* Level::GetTerrain()
{	
	return gTerrain;
}
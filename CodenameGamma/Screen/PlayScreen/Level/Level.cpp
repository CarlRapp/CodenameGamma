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

	for(int i = 0; i < 0; ++i)
		AddDirectionalLight(false);

	for(int i = 0; i < 1; ++i)
		AddDirectionalLight(true);

	for( int i = 0; i < 0; ++i)	
		AddPointLight(false);

	for( int i = 0; i < 10; ++i)	
		AddPointLight(true);

	for( int i = 0; i < 0; ++i)
		AddSpotLight(false);

	for( int i = 0; i < 10; ++i)	
		AddSpotLight(true);

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

	for ( UINT i = 0; i < gLoadedModels.size(); ++i )
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
	gQuadTree = new QuadTree(world, 250 * 250);
	gGraphicsManager->SetQuadTree(gQuadTree);
	Model* model = new Model(gLData.DEVICE, gTextureManager, "DATA\\Models\\obj\\pacman.obj", "DATA/Models/Textures/");

	for (int i = 0; i < 100; ++i)
	{
		float x = MathHelper::RandF(0, 4000);
		float y = 12;
		float z = MathHelper::RandF(0, 4000);

		AddInstance(x, y, z, model);
	}

	Model* pistolModel = new Model(gLData.DEVICE, gTextureManager, "DATA\\Models\\obj\\BoxWeapon.obj", "DATA/Models/Textures/");
	Model* bulletModel = new Model(gLData.DEVICE, gTextureManager, "DATA\\Models\\obj\\GunShot.obj", "DATA/Models/Textures/");

	ModelInstance *instance = new ModelInstance();
	instance->m_Model = pistolModel;
	instance->m_OldBoundingSphere = instance->GetBoundingSphere();

	Pistol *pistol = new Pistol();



	pistol->SetModelInstance(instance);
	AddGameObject(pistol);
	((Unit*)gGameObjects[0])->SetWeapon(pistol);

	gLoadedModels.push_back( model );
	gLoadedModels.push_back( pistolModel );
	gLoadedModels.push_back( bulletModel );
}


void Level::AddDirectionalLight(bool hasShadow)
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
	dirLight->Direction = XMFLOAT4(0.0f, -1.0f, 0.0f,0);
	//dirLight->Direction = XMFLOAT4(x, -1.0f, z,0);
	dirLight->HasShadow = hasShadow;
	dirLight->Resolution = SHADOWMAP_4096;
	//dirLight->Resolution = SHADOWMAP_2048;
	//dirLight->Resolution = SHADOWMAP_1024;
	gDirLights.push_back(dirLight);
}
void Level::AddPointLight(bool hasShadow)
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
	PointLight *pointLight = new PointLight();
	pointLight->Color		= XMFLOAT4(r, g, b, 0);
	pointLight->Position	= XMFLOAT3(x, 0.0f, z);
	pointLight->Range		= 300;
	pointLight->HasShadow = hasShadow;
	gPointLights.push_back(pointLight);

	float Ox = pointLight->Position.x + MathHelper::RandF(-300.0f, 300.0f);
	float Oz = pointLight->Position.z + MathHelper::RandF(-300.0f, 300.0f);

	rotpos.push_back(XMFLOAT3(Ox, 0.0f, Oz));
	angle.push_back(MathHelper::RandF(-45.0f, 45.0f));
}
void Level::AddSpotLight(bool hasShadow)
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
	if (!hasShadow)
	{
		g = 0.0f;
		b = 0.0f;
	}

	SpotLight *spotLight	= new SpotLight();
	spotLight->Color		= XMFLOAT4(r, g, b, 0.0f);
	spotLight->Position		= XMFLOAT3(x, 300.0f, z);

	//spotLight->Direction	= XMFLOAT3(MathHelper::RandF(-5.0f, 5.0f), -1, MathHelper::RandF(-5.0f, 5.0f));
	spotLight->Direction	= XMFLOAT3(0, -1, 1);

	spotLight->angle		= XMConvertToRadians(45);
	spotLight->Range		= 1000.0f;

	spotLight->HasShadow = hasShadow;
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

	Unit *go = new Unit();
	go->MoveTo(DirectX::XMFLOAT3(x, y, z));
	go->SetModelInstance(instance);
	go->SetScale(XMFLOAT3(10, 10, 10));

	instance->m_World	=	go->GetFloat4x4Value(World);

	float speed = 80;

	if (MathHelper::RandF(0, 1) > 0.0f)
		go->SetVelocity(DirectX::XMFLOAT3(MathHelper::RandF(-speed, speed), 0, MathHelper::RandF(-speed, speed)));

	instance->m_OldBoundingSphere = instance->GetBoundingSphere();

	int a = (int)(MathHelper::RandF(0, 1) * 4) + 1;

	go->SetTeam((GOTeam)a);


	AddGameObject(go);
}

void Level::Update(float DeltaTime)
{
	if ( InputManager::GetInstance()->GetController(0)->GetButtonState( RIGHT_BUMPER ) == DOWN )
	{
		vector<Projectile*>	tBullets	=	((Unit*)gGameObjects[0])->FireWeapon();

		if ( tBullets.size() > 0)
			for each ( Projectile* p in tBullets )
			{
				ModelInstance *instance = new ModelInstance();
				instance->m_Model = gLoadedModels[2];

				p->SetModelInstance(instance);

				//instance->m_World	=	p->GetFloat4x4Value(World);
				instance->m_OldBoundingSphere = instance->GetBoundingSphere();
				
				AddGameObject(p);
			}
	}

	for ( int i = gGameObjects.size() - 1; i >= 0; --i )
	{
		GameObject*	tObject	=	gGameObjects[i];

		if ( tObject->Update(DeltaTime, gTerrain) )
		{
			if ( tObject->IsAlive() )
				gQuadTree->Update(tObject);
			else
				gQuadTree->Delete(tObject);
		}

		if ( !tObject->IsAlive() )
		{
			tObject	=	0;
			gGameObjects.erase( gGameObjects.begin() + i );
		}
	}

	RunCollisionTest();

	//Updaterar ljus
	//Updaterar dirlights	
	for (int i = 0; i < (int)gDirLights.size(); ++i)
	{
		if (InputManager::GetInstance()->GetController(0)->GetButtonState(Xbox_Button::A) == InputState::PRESSED)
		{
			gDirLights[i]->Direction =  XMFLOAT4(3.0f, -1.0f, 0.0f,0);
		}
		else if (InputManager::GetInstance()->GetController(0)->GetButtonState(Xbox_Button::B) == InputState::PRESSED)
		{
			gDirLights[i]->Direction =  XMFLOAT4(-3.0f, 1.0f, 0.0f,0);
		}
		else
		{
			DirectionalLight *dLight = gDirLights[i];

			float a = -5.0f;

			XMVECTOR dir  = XMLoadFloat4(&dLight->Direction);
			XMMATRIX rot	 = XMMatrixRotationZ(DeltaTime * XMConvertToRadians(a));
			dir = XMVector3TransformCoord(dir, rot);
			XMStoreFloat4(&dLight->Direction, dir);
		}
	}


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

		sLight->Position.y = gTerrain->GetHeight(sLight->Position.x, sLight->Position.z) + 100.0f;
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



//	Will run after every
//	object has been updated
//	so all the positions are
//	up to date
void Level::RunCollisionTest()
{
	vector<CollisionEvent>	tCollisionEvents	=	vector<CollisionEvent>();

	BoundingSphere As, Bs;
	for each (GameObject* A in gGameObjects)
	{
		As	=	A->GetModelInstance()->GetBoundingSphere();

		vector<GameObject*>	collidingWith	=	vector<GameObject*>();

		gQuadTree->GetObjectsCollidingWith(A, collidingWith);

		for each (GameObject* B in collidingWith)
		{
			if ( A == B || !A->IsAlive() || !B->IsAlive() )
				continue;

			Bs	=	B->GetModelInstance()->GetBoundingSphere();

			if ( As.Intersects( Bs ) )
			{
				CollisionEvent	tEvent(A, B);

				bool	hasOccured	=	false;
				for each ( CollisionEvent Event in tCollisionEvents )
					if ( Event == tEvent )
						hasOccured	=	true;

				if ( !hasOccured )
					tCollisionEvents.push_back(tEvent);
			}
		}
	}
	
	if ( tCollisionEvents.size() > 0 )
	{
		for each( CollisionEvent tEvent in tCollisionEvents )
		{
			//if ( tEvent.A->IsEnemy( tEvent.B ) )
			//{
				if ( typeid( *tEvent.A ) == typeid( *tEvent.B ) )
					tEvent.A->CollideWith( tEvent.B );
				else
				{
					//cout << typeid( *tEvent.A ).name() << " - " << typeid( *tEvent.B ).name() << endl;

					tEvent.A->CollideWith( tEvent.B );
					tEvent.B->CollideWith( tEvent.A );
				}
			//}
		}
	}

}
#include "Level.h"
#include "../Units/CrazyBitch.h"
#include "../Structures/StructureList.h"
#include "../Items/ItemList.h"

#define TESTSCALE 0.3f

Level::Level(){}
Level::Level(SystemData LData)
{
	gLData = LData;
	Effects::InitAll(LData.DEVICE);
	RenderStates::InitAll(LData.DEVICE);
	InputLayouts::InitAll(LData.DEVICE);

	gTerrain	=	new Terrain(LData.DEVICE, LData.DEVICE_CONTEXT);

	gGraphicsManager	=	new GraphicsManager(LData.DEVICE, LData.DEVICE_CONTEXT, LData.RENDER_TARGET_VIEW, LData.SCREEN_WIDTH, LData.SCREEN_HEIGHT);
	gGraphicsManager->SetTerrain(gTerrain);
	gGraphicsManager->SetLights(&gDirLights, &gPointLights, &gSpotLights);

	
	for(int i = 0; i < 0; ++i)
		AddDirectionalLight(false);

	for(int i = 0; i < 1; ++i)
		AddDirectionalLight(true);
	/*
	for( int i = 0; i < 0; ++i)	
		AddPointLight(false);

	for( int i = 0; i < 0; ++i)	
		AddPointLight(true);

	for( int i = 0; i < 0; ++i)
		AddSpotLight(false);

	for( int i = 0; i < 0; ++i)	
		AddSpotLight(true);
		*/

	AddPointLight(true, XMFLOAT3(1700,0,2200));

	//AddPointLight(true, XMFLOAT3(1800,0,1800));
	//AddPointLight(true, XMFLOAT3(2200,0,2800));
	//AddPointLight(true, XMFLOAT3(1800,0,2800));


	AddPointLight(true, XMFLOAT3(1000,0,1000));
	AddPointLight(true, XMFLOAT3(1000,0,3000));
	AddPointLight(true, XMFLOAT3(3000,0,1000));
	AddPointLight(true, XMFLOAT3(3000,0,3000));


	AddSpotLight(true, XMFLOAT3(2200,0,2100));
	AddSpotLight(true, XMFLOAT3(1700,0,2650));


	AddSpotLight(true, XMFLOAT3(500,0,500));
	AddSpotLight(true, XMFLOAT3(500,0,3500));
	AddSpotLight(true, XMFLOAT3(3500,0,500));
	AddSpotLight(true, XMFLOAT3(3500,0,3500));


	/*


	AddSpotLight(true, XMFLOAT3(0,0,0));
	AddSpotLight(true, XMFLOAT3(0,0,0));
	AddSpotLight(true, XMFLOAT3(0,0,0));
	AddSpotLight(true, XMFLOAT3(0,0,0));
	AddSpotLight(true, XMFLOAT3(0,0,0));
	AddSpotLight(true, XMFLOAT3(0,0,0));
	*/


	gQuadTree = NULL;
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
	

	ModelManager::GetInstance()->LoadModel("CrazyBitch", "dae export 2.dae", "DATA/Models/TestChar/");
	//ModelManager::GetInstance()->LoadModel("CrazyBitch", "CrazyBitch.obj", "DATA/Models/CrazyBitch/");

	ModelManager::GetInstance()->LoadModel("Glock", "Glock.obj", "DATA/Models/Glock/");
	ModelManager::GetInstance()->LoadModel("Shotgun", "Shotgun.obj", "DATA/Models/Shotgun/");

	ModelManager::GetInstance()->LoadModel("CannedFood", "CannedFood.obj", "DATA/Models/CannedFood/");

	ModelManager::GetInstance()->LoadModel("Bullet", "PistolBullet.obj", "DATA/Models/PistolBullet/");
	ModelManager::GetInstance()->LoadModel("CrowdBarrier", "CrowdBarrier.obj", "DATA/Models/CrowdBarrier/");
	//ModelManager::GetInstance()->LoadModel("TownHall", "TownHall.obj", "DATA/Models/TownHall/");
	ModelManager::GetInstance()->LoadModel("TownHall", "TownHall.dae", "DATA/Models/TownHall/");

	ModelManager::GetInstance()->LoadModel("Canister", "Canister.obj", "DATA/Models/Canister/");
	ModelManager::GetInstance()->LoadModel("SmallStore", "SmallStore.obj", "DATA/Models/SmallStore/");
	ModelManager::GetInstance()->LoadModel("TrashCan", "TrashCan.obj", "DATA/Models/TrashCan/");
	ModelManager::GetInstance()->LoadModel("VolvoCar", "VolvoCar.obj", "DATA/Models/VolvoCar/");

	ModelManager::GetInstance()->LoadModel("Container", "Container.obj", "DATA/Models/Container/");

	ModelManager::GetInstance()->LoadModel("UnitCube", "UnitCube.obj", "DATA/Models/UnitCube/");

	Model*	model	=	ModelManager::GetInstance()->GetModel("CrazyBitch");
	for (int i = 0; i < 10; ++i)
	{
		float x = MathHelper::RandF(0, 4000);
		float y = 0;
		float z = MathHelper::RandF(0, 4000);

		AddInstance(x, y, z, model);
	}


	GameObject*	tGO;
	tGO	=	new TownHall();
	tGO->MoveTo( XMFLOAT3( 2000, 0, 2000 + 400 ) );
	AddGameObject(tGO);

	tGO	=	new Pistol();
	tGO->MoveTo( XMFLOAT3( 800, 0, 800 ) );
	AddGameObject(tGO);
	
	//	Lilla scenen 
	tGO	=	new CrowdBarrier();
	tGO->MoveTo( XMFLOAT3( 2000 - 85, 0, 2000 ) );
	AddGameObject(tGO);
	
	tGO	=	new SmallStore();
	tGO->MoveTo( XMFLOAT3( 2000 - 200, 0, 2000 ) );
	AddGameObject(tGO);
	
	tGO	=	new TrashCan();
	tGO->MoveTo( XMFLOAT3( 2000 - 130, 0, 2000 - 80 ) );
	AddGameObject(tGO);
	
	tGO	=	new VolvoCar();
	tGO->MoveTo( XMFLOAT3( 2000 - 350, 0, 2000 - 100 ) );
	tGO->SetRotation( XMFLOAT3( 0, PI * 0.25f, 0 ) );
	AddGameObject(tGO);
	
	tGO	=	new Container();
	tGO->MoveTo( XMFLOAT3( 2000 + 200, 0, 2000 - 400 ) );
	AddGameObject(tGO);
	
	for (int i = 0; i < 20; ++i)
	{
		float x = MathHelper::RandF(0, 500);
		float y = 0;
		float z = MathHelper::RandF(0, 500);

		tGO	=	new CannedFood();
		tGO->MoveTo( XMFLOAT3( x, 0, z ) );
		AddGameObject(tGO);
	}


	

	float	SIZE	=	2 * 16.6665f;
	for ( int n = 0; n <= 4; ++n )
	{
		int test	=	7 - 2 * n;
		for ( int i = 1; i <= test; ++i )
		{
			tGO	=	new UnitCube();
			tGO->MoveTo( XMFLOAT3( 2000 - (-(int)(test*0.5f) + i) * SIZE, n * SIZE, 2000 - 400 ) );
			AddGameObject(tGO);
		}

	}

	for( int n = 0; n < 10; ++n )
	{
		tGO	=	new UnitCube();
		tGO->MoveTo( XMFLOAT3( 2000 - n * SIZE, 0, 2000 - 800 ) );
		AddGameObject(tGO);
	}
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
	dirLight->Direction = XMFLOAT4(1.0f, -2.0f, 1.0f,0);
	//dirLight->Direction = XMFLOAT4(x, -1.0f, z,0);
	dirLight->HasShadow = hasShadow;
	dirLight->Resolution = SHADOWMAP_4096;
	//dirLight->Resolution = SHADOWMAP_2048;
	//dirLight->Resolution = SHADOWMAP_1024;
	gDirLights.push_back(dirLight);
}
void Level::AddPointLight(bool hasShadow, XMFLOAT3 pos)
{
	float x = MathHelper::RandF(0.0f, 4000.0f);
	float z = MathHelper::RandF(0.0f, 4000.0f);
		
	float r = MathHelper::RandF(0.0f, 2.0f);
	float g = MathHelper::RandF(0.0f, 2.0f);
	float b = MathHelper::RandF(0.0f, 2.0f);
	/*
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	*/
	PointLight *pointLight = new PointLight();
	pointLight->Color		= XMFLOAT4(r, g, b, 0);
	//pointLight->Position	= XMFLOAT3(x, 0.0f, z);
	pointLight->Position	= pos;
	pointLight->Range		= 300;
	pointLight->HasShadow = hasShadow;
	gPointLights.push_back(pointLight);

	float Ox = pointLight->Position.x + 300;
	float Oz = pointLight->Position.z + 300;

	rotpos.push_back(XMFLOAT3(Ox, 0.0f, Oz));
	angle.push_back(MathHelper::RandF(-45.0f, 45.0f));
}
void Level::AddSpotLight(bool hasShadow, XMFLOAT3 pos)
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
	//spotLight->Position		= XMFLOAT3(x, 300.0f, z);
	spotLight->Position		= pos;

	//spotLight->Direction	= XMFLOAT3(MathHelper::RandF(-5.0f, 5.0f), -1, MathHelper::RandF(-5.0f, 5.0f));
	spotLight->Direction	= XMFLOAT3(0, -1, 1);

	spotLight->angle		= XMConvertToRadians(45);
	spotLight->Range		= 1000.0f;

	spotLight->HasShadow = hasShadow;
	gSpotLights.push_back(spotLight);


	float aspeed = MathHelper::RandF(40.0f, 90.0f);

	if (MathHelper::RandF(0.0f, 1.0f) < 0.5f)
		aspeed *= -1;

	anglespot.push_back(aspeed);

	//float Ox = pointLight->Position.x + MathHelper::RandF(-300.0f, 300.0f);
	//float Oz = pointLight->Position.z + MathHelper::RandF(-300.0f, 300.0f);

	//rotpos.push_back(XMFLOAT3(Ox, 0.0f, Oz));
	//angle.push_back(MathHelper::RandF(-45.0f, 45.0f));
}

void Level::AddInstance(float x, float y, float z, Model *model)
{
	ModelInstance *instance = new ModelInstance();
	instance->SetModel(model);
	//instance->m_Model = model;

	Unit *go = new Unit();
	go->MoveTo(DirectX::XMFLOAT3(x, y, z));
	go->SetScale(TESTSCALE);	
	go->SetModelInstance(instance);
	go->PlayAnimation("ALL");
	float speed = 160;

	if (MathHelper::RandF(0, 1) > 0.0f)
		go->SetVelocity(DirectX::XMFLOAT3(MathHelper::RandF(-speed, speed), 0, MathHelper::RandF(-speed, speed)));

	instance->m_OldBoundingSphere = instance->GetBoundingSphere();

	int a = (int)(MathHelper::RandF(0, 1) * 4) + 1;

	go->SetTeam((GOTeam)a);


	AddGameObject(go);
}

void Level::Update(float DeltaTime)
{
	vector<GameObject*> trash;
	//for ( int i = gGameObjects.size() - 1; i >= 0; --i )
	for each (GameObject*	tObject in gGameObjects)
	{
		//GameObject*	tObject	=	gGameObjects[i];

		if ( tObject->Update(DeltaTime, gTerrain) )
		{
			if ( tObject->IsAlive() )
				gQuadTree->Update(tObject);
			else
			{
				trash.push_back(tObject);
			}
		}
	}

	for each (GameObject* tObject in trash)
	{
		RemoveGameObject(tObject);
	}

	for each (Player *p in gPlayers)
	{
		vector<Projectile*>	tBullets = p->Update(DeltaTime);
		for each ( Projectile* p in tBullets )		
			AddGameObject(p);		
	}

	RunCollisionTest();

	//Updaterar ljus
	//Updaterar dirlights	
	for (int i = 0; i < (int)gDirLights.size(); ++i)
	{
		if (InputManager::GetInstance()->GetController(0)->GetButtonState(Xbox_Button::A) == InputState::PRESSED)
		{
			gDirLights[i]->Direction =  XMFLOAT4(1.0f, -2.0f, 1.0f,0);
		}
		else if (InputManager::GetInstance()->GetController(0)->GetButtonState(Xbox_Button::B) == InputState::PRESSED)
		{
			gDirLights[i]->Direction =  XMFLOAT4(0.0f, 1.0f, 1.0f,0);
		}
		else
		{
			/*
			DirectionalLight *dLight = gDirLights[i];

			float a = -5.0f;

			XMVECTOR dir  = XMLoadFloat4(&dLight->Direction);
			XMMATRIX rot	 = XMMatrixRotationZ(DeltaTime * XMConvertToRadians(a));
			dir = XMVector3TransformCoord(dir, rot);
			XMStoreFloat4(&dLight->Direction, dir);
			*/
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

void Level::Render()
{
	vector<Camera*> tCams = vector<Camera*>();

	for each(Player* p in gPlayers)
		tCams.push_back( p->GetCamera() );

	gGraphicsManager->Render(tCams);
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

	for each (GameObject* A in gGameObjects)
	{
		vector<GameObject*>				collidingWith	=	vector<GameObject*>();
		vector<vector<CollisionData>>	CD				=	vector<vector<CollisionData>>();

		gQuadTree->GetObjectsCollidingWith(A, collidingWith, CD);

		//for each (GameObject* B in collidingWith)
		for (int i = 0; i < collidingWith.size(); ++i)
		{
			GameObject* B = collidingWith[i];
			if ( A == B || !A->IsAlive() || !B->IsAlive() )
				continue;

			CollisionEvent	tEvent(A, B, CD[i]);

			bool	hasOccured	=	false;
			for each ( CollisionEvent Event in tCollisionEvents )
				if ( Event == tEvent )
					hasOccured	=	true;

			if ( !hasOccured )
				tCollisionEvents.push_back(tEvent);
			
		}
	}
	
	if ( tCollisionEvents.size() > 0 )
	{
		for each( CollisionEvent tEvent in tCollisionEvents )
		{
			//cout << typeid( *tEvent.A ).name() << " - " << typeid( *tEvent.B ).name() << endl;
			if ( typeid( *tEvent.A ) == typeid( *tEvent.B ) )
				tEvent.A->CollideWith( tEvent.B, tEvent.CD );
			else
			{	
				tEvent.A->CollideWith( tEvent.B, tEvent.CD );


				//for each (CollisionData cd in tEvent.CD)				
				//	cd.Swap();

				for (int i = 0; i < tEvent.CD.size(); ++i)
				{
					HitBox temp = tEvent.CD[i].A;
					tEvent.CD[i].A = tEvent.CD[i].B;
					tEvent.CD[i].B = temp;
				}


				//HitBox temp = tEvent.CD.A;
				//tEvent.CD.A = tEvent.CD.B;
				//tEvent.CD.B = temp;
				tEvent.B->CollideWith( tEvent.A, tEvent.CD );
			}
		}
	}

}

#pragma region Setup Viewports (SetNumberOfPlayers)
void Level::SetNumberOfPlayers(int noPlayers, int screenWidth, int screenHeight)
{
	for each (Player *player in gPlayers)
	{
		Unit *unit = player->GetUnit();

		if (unit)
		{
			Weapon* w = unit->GetWeapon();
			if (w)
				RemoveGameObject(w);
			RemoveGameObject(unit);
		}
	}


	gPlayers.clear();
	Player *player1 =  new Player(0);
	Player *player2 = new Player(1);
	Player *player3 = new Player(2);
	Player *player4 = new Player(3);

	switch (noPlayers)
	{
		case 1:
			player1->SetScreen(0,				0,						screenWidth,	screenHeight);	
			gPlayers.push_back(player1);
			break;
		case 2:
			player1->SetScreen(0,				0,						screenWidth / 2, screenHeight);	
			player2->SetScreen(screenWidth / 2,	0,						screenWidth / 2, screenHeight);
			gPlayers.push_back(player1);	
			gPlayers.push_back(player2);
			break;
		case 3:
			player1->SetScreen(0,				0,						screenWidth,	 screenHeight / 2);	
			player2->SetScreen(0,				screenHeight / 2,		screenWidth / 2, screenHeight / 2);
			player3->SetScreen(screenWidth / 2,	screenHeight / 2,		screenWidth / 2, screenHeight / 2);
			gPlayers.push_back(player1);	
			gPlayers.push_back(player2);
			gPlayers.push_back(player3);
			break;
		case 4:
			player1->SetScreen(0,				0,						screenWidth / 2, screenHeight / 2);	
			player2->SetScreen(screenWidth / 2,	0,						screenWidth / 2, screenHeight / 2);
			player3->SetScreen(0,				screenHeight / 2,		screenWidth / 2, screenHeight / 2);
			player4->SetScreen(screenWidth / 2,	screenHeight / 2,		screenWidth / 2, screenHeight / 2);
			gPlayers.push_back(player1);	
			gPlayers.push_back(player2);
			gPlayers.push_back(player3);
			gPlayers.push_back(player4);	
			break;
	}

	for (int i = 0; i < gPlayers.size(); ++i)
	{
		if ( gPlayers[i]->GetUnit() == 0 )
		{
			PlayerUnit* unit	=	new CrazyBitch();
			//GO->UsePose("Stand");
			//GO->PlayAnimation("Walk");
			unit->PlayAnimation("Back");
			unit->MoveTo(DirectX::XMFLOAT3(600, 0, 600));
			unit->SetScale(TESTSCALE);
			AddGameObject(unit);

			gPlayers[i]->SetUnit(unit);
			gPlayers[i]->GetUnit()->SetTeam(Team1);

			Pistol *pistol = new Pistol();
			AddGameObject(pistol);

			gPlayers[i]->GetUnit()->SetWeapon(pistol);
		}
	}

}
#pragma endregion
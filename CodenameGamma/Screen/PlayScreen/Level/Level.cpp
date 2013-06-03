#include "Level.h"
#include "../Units/CrazyBitch.h"
#include "../Structures/StructureList.h"
#include "../Items/ItemList.h"
#include "../Weapons/WeaponList.h"
#include "LevelParser.h"
#include "../Units/Rat.h"
#include "../Units/Tank.h"
#include "../Units/Ghost.h"

#define TESTSCALE 1.0f

Level::Level(){}
Level::Level(SystemData LData, GraphicsManager* Instance)
{
	gLData = LData;

	gTerrain	=	new Terrain(LData.DEVICE, LData.DEVICE_CONTEXT);

	gGraphicsManager	=	Instance;
	gGraphicsManager->SetTerrain(gTerrain);
	gGraphicsManager->SetLights(&gDirLights, &gPointLights, &gSpotLights, &gGlobalLight);

	gQuadTree = NULL;

	AutomaticRifleSpawnTimer = 0.0f;
	ShotgunSpawnTimer = 0.0f;
	SniperRifleSpawnTimer = 0.0f;

	FoodSpawnTimer = 0.0f;
	WaterSpawnTimer = 0.0f;
	AmmoBoxSpawnTimer = 0.0f;
	MedicPackSpawnTimer = 0.0f;
}

Level::~Level()
{
	if ( gTerrain )
		delete	gTerrain;

	for each( Player* P in gPlayers )
		delete P;

	for each( GameObject* GO in gGameObjects )
		DeleteGameObject( GO );

	for each( DirectionalLight* tLight in gDirLights )
		RemoveLight( tLight );

	for each( PointLight* tLight in gPointLights )
		RemoveLight( tLight );

	for each( SpotLight* tLight in gSpotLights )
		RemoveLight( tLight );

	for each( GameObject* GO in gGameObjects )
		delete GO;
}

void Level::LoadLevel(string Levelname)
{
	//	TODO:	Fix a more dynamic
	//			way to load maps.

	string	tLevelRootPath	=	"DATA/Maps/";

	LevelData	LData	=	LevelParser::ParseLevel(Levelname, tLevelRootPath);
	
	gGlobalLight = LData.GlobalLight;

	if (!XMVector4Equal(XMLoadFloat4(&LData.SunColor), XMLoadFloat4(&XMFLOAT4(0,0,0,1))))
	{
		DirectionalLight *dirLight = new DirectionalLight();
		dirLight->GetGPULight()->Color = LData.SunColor;
		dirLight->GetGPULight()->Direction = LData.SunDirection;
		dirLight->GetGPULight()->HasShadow = true;
		dirLight->GetGPULight()->Resolution = SHADOWMAP_4096;
		AddLight(dirLight);
	}

	if ( LData.IsLoaded() )
		gTerrain->LoadTerrain(LData);


	SAFE_DELETE(gQuadTree);
	XMFLOAT2 halfWorldSize = gTerrain->GetDimensions();
	halfWorldSize.x /= 2.0f;
	halfWorldSize.y /= 2.0f;
	BoundingBox world = BoundingBox(XMFLOAT3(halfWorldSize.x, 0, halfWorldSize.y), XMFLOAT3(halfWorldSize.x + 1000, 2000, halfWorldSize.y + 1000));
	gQuadTree = new QuadTree(world, 500 * 500, 4);
	gGraphicsManager->SetQuadTree(gQuadTree);

	ModelManager::GetInstance()->LoadModel("CrazyBitch", "CrazyBitch.dae", "DATA/Models/CrazyBitch/");
	ModelManager::GetInstance()->LoadModel("Rat", "Rat.dae", "DATA/Models/Rat/");
	ModelManager::GetInstance()->LoadModel("Ghost", "Ghost.dae", "DATA/Models/Ghost/");
	ModelManager::GetInstance()->LoadModel("Tank", "Tank.dae", "DATA/Models/Tank/");

	ModelManager::GetInstance()->LoadModel("Glock", "Glock.dae", "DATA/Models/Glock/");
	ModelManager::GetInstance()->LoadModel("Shotgun", "Shotgun.dae", "DATA/Models/Shotgun/");
	ModelManager::GetInstance()->LoadModel("SniperRifle", "SniperRifle.dae", "DATA/Models/SniperRifle/");
	ModelManager::GetInstance()->LoadModel("AutomaticRifle", "AutomaticRifle.dae", "DATA/Models/AutomaticRifle/");

	ModelManager::GetInstance()->LoadModel("Shotgun-WOG", "Shotgun.obj", "DATA/Models/Shotgun/");
	ModelManager::GetInstance()->LoadModel("SniperRifle-WOG", "SniperRifle.obj", "DATA/Models/SniperRifle/");
	ModelManager::GetInstance()->LoadModel("AutomaticRifle-WOG", "AutomaticRifle.obj", "DATA/Models/AutomaticRifle/");
	
	ModelManager::GetInstance()->LoadModel("Fire", "Fire.obj", "DATA/Models/Fire/");

	ModelManager::GetInstance()->LoadModel("CannedFood", "CannedFood.obj", "DATA/Models/CannedFood/");
	ModelManager::GetInstance()->LoadModel("Canister", "Canister.obj", "DATA/Models/Canister/");
	ModelManager::GetInstance()->LoadModel("MediPack", "MediPack.obj", "DATA/Models/MediPack/");
	ModelManager::GetInstance()->LoadModel("AmmoBox", "AmmoBox.obj", "DATA/Models/AmmoBox/");

	ModelManager::GetInstance()->LoadModel("Bullet", "PistolBullet.obj", "DATA/Models/PistolBullet/");
	ModelManager::GetInstance()->LoadModel("EnergyOrb", "EnergyOrb.obj", "DATA/Models/EnergyOrb/");

	EntityData	Result	=	LevelParser::ParseLevelEntities(LData);

	//	Structures from the level
	for each( GameObjectData* GOD in Result.gameObjectData )
	{
		AddGameObject( GOD->gameObject );
		BlockPathWithObject( GOD->gameObject );

		GOD->gameObject->SetTextureIndex(GOD->textureIndex);

		if (GOD->vegetation)
			((Structure*)GOD->gameObject)->SetOvergrown(true);

	}

	for each( Light* LIGHT in Result.Lights )
		AddLight( LIGHT );


	gNodeMap	=	Result.NodeMapInstance;

	//	Create the Wave system
	gWave		=	new Wave( gNodeMap );
	gWave->LoadWaveData( tLevelRootPath + Levelname + "/");
	gWave->SetAddGameObject(std::bind(&Level::AddGameObject, this, std::placeholders::_1));
}

void Level::Update(float DeltaTime)
{
	DeltaTime	=	min( 1.0f, DeltaTime );
	vector<GameObject*> trash;
	vector<GameObject*>	tempGameObjects	=	gGameObjects;

	for each (Player *p in gPlayers)
	{
		p->Update(DeltaTime);

		PlayerUnit*	tUnit	=	p->GetUnit();
		if ( gWave->IsLimitReached() && tUnit )
		{
			if( !tUnit->IsAlive() )
				continue;

			tUnit->Drink( -1.0f * DeltaTime );
			tUnit->Eat( -1.0f * DeltaTime );
			tUnit->Hurt( 1.0f * DeltaTime );
		}

		if (p->IsDead())
		{
			for each (Player *p2 in gPlayers)
			{
				if (!p2->IsDead())
				{
					p->Spectate(p2);
					break;
				}
			}
		}

		UpdateSpectator(p);
	}

	//for ( int i = gGameObjects.size() - 1; i >= 0; --i )
	for each (GameObject*	tObject in tempGameObjects)
	{
		//GameObject*	tObject	=	gGameObjects[i];

		tObject->Update(DeltaTime, gTerrain);
		if ( tObject->IsAlive() )
		{
			gQuadTree->Update(tObject);
		}
		else if ( tObject->WantsRemove() )		
			trash.push_back(tObject);

		if( IsOfType<EnemyUnit>(tObject) )
		{
			vector<PlayerUnit*> targets;

			for each (Player* player in gPlayers)
			{
				PlayerUnit* pUnit = player->GetUnit();
				if (pUnit)
					targets.push_back(pUnit);
			}

			((EnemyUnit*)tObject)->SetTargets( targets );
		}
	}

	for each (GameObject* tObject in trash)
	{
		DeleteGameObject(tObject);
	}

	RunCollisionTest();

	gWave->Update( DeltaTime );

	SpawnItems(DeltaTime);
}

void Level::UpdateSpectator(Player* p)
{
	if (p->IsSpectating())
	{
		bool goPrev = p->GetController()->GetButtonState(Xbox_Button::LEFT_BUMPER) == PRESSED;
		bool goNext = p->GetController()->GetButtonState(Xbox_Button::RIGHT_BUMPER) == PRESSED;

		if (goPrev || goNext && goPrev != goNext)
		{
			vector<Player*> tempPlayers;

			for each (Player *p2 in gPlayers)
			{
				if (!p2->IsDead() && !p2->IsSpectating())
					tempPlayers.push_back(p2);
			}

			Player* first = 0;
			Player* last = 0;
			Player* next = 0;
			Player* prev = 0;
			int current = (int)p->GetSpectateIndex();
			for each (Player *p2 in tempPlayers)
			{
				int temp = p2->GetIndex();

				if (temp < current)
				{
					if (prev)
					{
						if (temp > prev->GetIndex())
							prev = p2;

						if (temp < first->GetIndex())
							first = p2;
					}
					else
					{
						prev = p2;
						first = p2;
					}
				}

				else if (temp > current)
				{
					if (next)
					{
						if (temp < next->GetIndex())
							next = p2;

						if (temp > last->GetIndex())
							last = p2;
					}
					else
					{
						next = p2;
						last = p2;
					}
				}
			}

			if (goPrev)
			{
				if (prev)
					p->Spectate(prev);
				else if (last)
					p->Spectate(last);

			}

			else if (goNext)
			{
				if (next)
					p->Spectate(next);
				else if (first)
					p->Spectate(first);
			}
		}
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

void Level::SpawnItem(Item* item)
{
	XMFLOAT3 pos;
	bool foundFreePos = false;

	float border = 30;
	XMFLOAT2 dim = gTerrain->GetDimensions();
	XMFLOAT2 min = XMFLOAT2(border, border);
	XMFLOAT2 max = XMFLOAT2(dim.x - border, dim.y - border);
	for (int i = 0; i < 20; ++i)
	{
		float X = MathHelper::RandF(min.x, max.x);
		float Z = MathHelper::RandF(min.y, max.y);


		if (gTerrain->IsWalkable(X, Z))
		{
			foundFreePos = true;
			pos = XMFLOAT3(X, 10.0f, Z);
		}
	}

	if (!foundFreePos)
	{
		XMFLOAT2 tPos = gNodeMap->GetRandomNode()->Position;
		pos = XMFLOAT3(tPos.x, 10.0f, tPos.y);
	}

	item->MoveTo( pos );
	AddGameObject(item);
}

void Level::SpawnItems(float deltaTime)
{
	int activePlayers = 0;

	for each (Player* p in gPlayers)
	{
		if (p->IsAlive())
			++activePlayers;
	}

	XMFLOAT2 dim = gTerrain->GetDimensions();
	float area = dim.x * dim.y;
	float areaFactor = area / StandardArea;

	float timerDelta = areaFactor * activePlayers * deltaTime;

	AutomaticRifleSpawnTimer += timerDelta;
	ShotgunSpawnTimer += timerDelta;
	SniperRifleSpawnTimer += timerDelta;
	FoodSpawnTimer += timerDelta;
	WaterSpawnTimer += timerDelta;
	AmmoBoxSpawnTimer += timerDelta;
	MedicPackSpawnTimer += timerDelta;

	if (AutomaticRifleSpawnTimer >= AutomaticRifleSpawnTimerStop)
	{
		AutomaticRifleSpawnTimer = 0.0f;
		SpawnItem(new WeaponOnGround(new AutomaticRifle()));
	}

	if (ShotgunSpawnTimer >= ShotgunSpawnTimerStop)
	{
		ShotgunSpawnTimer = 0.0f;
		SpawnItem(new WeaponOnGround(new Shotgun()));
	}

	if (SniperRifleSpawnTimer >= SniperRifleSpawnTimerStop)
	{
		SniperRifleSpawnTimer = 0.0f;
		SpawnItem(new WeaponOnGround(new SniperRifle()));
	}

	if (FoodSpawnTimer >= FoodSpawnTimerStop)
	{
		FoodSpawnTimer = 0.0f;
		SpawnItem(new CannedFood());
	}

	if (WaterSpawnTimer >= WaterSpawnTimerStop)
	{
		WaterSpawnTimer = 0.0f;
		SpawnItem(new Canister());
	}

	if (AmmoBoxSpawnTimer >= AmmoBoxSpawnTimerStop)
	{
		AmmoBoxSpawnTimer = 0.0f;
		SpawnItem(new AmmoBox());
	}

	if (MedicPackSpawnTimer >= MedicPackSpawnTimerStop)
	{
		MedicPackSpawnTimer = 0.0f;
		SpawnItem(new MediPack());
	}
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
				DeleteGameObject(w);
			DeleteGameObject(unit);
		}
	}


	gPlayers.clear();
	Player*	player1	=	new Player(0);
	Player*	player2	=	new Player(1);
	Player*	player3	=	new Player(2);
	Player*	player4	=	new Player(3);

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

	int team = 0;
	XMFLOAT2	Pos;
	for (int i = 0; i < gPlayers.size(); ++i)
	//for each( Player* p in gPlayers )
	{
		Player* p = gPlayers[i];
		if( p->GetUnit() != 0 )
			continue;
		
		Pos	=	XMFLOAT2( 1000, 1000 );

		PlayerUnit*	pUnit	=	new CrazyBitch();
		pUnit->MoveTo( XMFLOAT3( Pos.x, 0, Pos.y ) );
		pUnit->SetPlayerScore( p->GetPlayerScore() );
		pUnit->SetTeam( (GOTeam)team );
		pUnit->LoopAnimation( "StartPose" );
		pUnit->SetTextureIndex(i);
		p->SetUnit( pUnit );

		AddGameObject( pUnit );
		AddGameObject( pUnit->GetWeapon() );
	}
	/*
	WeaponOnGround*	shotgun	=	new WeaponOnGround( new Shotgun() );
	shotgun->MoveTo( XMFLOAT3(Pos.x + 50, 0, Pos.y) );
	AddGameObject(shotgun);

	WeaponOnGround*	sniperRifle	=	new WeaponOnGround( new SniperRifle() );
	sniperRifle->MoveTo( XMFLOAT3(Pos.x - 50, 0, Pos.y) );
	AddGameObject(sniperRifle);

	WeaponOnGround*	automaticRifle	=	new WeaponOnGround( new AutomaticRifle() );
	automaticRifle->MoveTo( XMFLOAT3(Pos.x, 0, Pos.y - 50) );
	AddGameObject(automaticRifle);
	*/
}
#pragma endregion

void Level::SetCallbackFunctions(GameObject* go)
{
	go->SetAddLight(std::bind(&Level::AddLight, this, std::placeholders::_1));
	go->SetRemoveLight(std::bind(&Level::RemoveLight, this, std::placeholders::_1));
	go->SetSetCallbackFunctions(std::bind(&Level::SetCallbackFunctions, this, std::placeholders::_1));
	go->SetAddGameObject(std::bind(&Level::AddGameObject, this, std::placeholders::_1));
}

void Level::AddGameObject(GameObject* go)
{ 
	SetCallbackFunctions(go);

	gGameObjects.push_back(go); 
	sort( gGameObjects.begin(), gGameObjects.end() );
	gQuadTree->Insert(go); 
}
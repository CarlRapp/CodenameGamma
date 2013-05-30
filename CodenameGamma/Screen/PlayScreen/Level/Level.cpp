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
	ModelManager::GetInstance()->LoadModel("Shotgun", "Shotgun.obj", "DATA/Models/Shotgun/");

	ModelManager::GetInstance()->LoadModel("CannedFood", "CannedFood.obj", "DATA/Models/CannedFood/");
	ModelManager::GetInstance()->LoadModel("Canister", "Canister.obj", "DATA/Models/Canister/");
	ModelManager::GetInstance()->LoadModel("MediPack", "MediPack.obj", "DATA/Models/MediPack/");

	ModelManager::GetInstance()->LoadModel("Bullet", "PistolBullet.obj", "DATA/Models/PistolBullet/");
	ModelManager::GetInstance()->LoadModel("EnergyOrb", "EnergyOrb.obj", "DATA/Models/EnergyOrb/");
	

	

	Model*	model	=	ModelManager::GetInstance()->GetModel("CrazyBitch");

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


	
	GameObject*	tGO;

	for (int i = 0; i < 0; ++i)
	{	
		tGO	=	new CrazyBitch();

		float x = MathHelper::RandF(100, 3900);
		float y = 0;
		float z = MathHelper::RandF(100, 3900);
		tGO->MoveTo( XMFLOAT3( x, y, z ) );

		float speed = 160;
		if (MathHelper::RandF(0, 1) > 0.0f)
			tGO->SetVelocity(DirectX::XMFLOAT3(MathHelper::RandF(-speed, speed), 0, MathHelper::RandF(-speed, speed)));

		int a = (int)(MathHelper::RandF(0, 1) * 4) + 1;
		tGO->SetTeam((GOTeam)a);

		AddGameObject(tGO);
	}

	for (int i = 0; i < 100; ++i)
	{
		float x = MathHelper::RandF(100, 3900);
		float y = 10;
		float z = MathHelper::RandF(100, 3900);

		tGO	=	new CannedFood();


		//Hur man sätter callbackmetoden.
		//tGO->SetAddLight(std::bind(&Level::AddLight, this, std::placeholders::_1));
		//tGO->SetRemoveLight(std::bind(&Level::RemoveLight, this, std::placeholders::_1));

		tGO->MoveTo( XMFLOAT3( x, y, z ) );
		AddGameObject(tGO);
	}

	for (int i = 0; i < 100; ++i)
	{
		float x = MathHelper::RandF(100, 3900);
		float y = 10;
		float z = MathHelper::RandF(100, 3900);

		//tWeapon->SetAddGameObject(std::bind(&Level::AddGameObject, this, std::placeholders::_1));

		if ( i % 2 == 0 )
			tGO	=	new WeaponOnGround( new Pistol() );
		else
			tGO	=	new WeaponOnGround( new Shotgun() );

		tGO->MoveTo( XMFLOAT3( x, y, z ) );

		//Hur man sätter callbackmetoden.
		//tGO->SetAddLight(std::bind(&Level::AddLight, this, std::placeholders::_1));
		//tGO->SetRemoveLight(std::bind(&Level::RemoveLight, this, std::placeholders::_1));

		
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
	dirLight->GetGPULight()->Color = XMFLOAT4(r,g,b,1);
	dirLight->GetGPULight()->Direction = XMFLOAT4(1.0f, -2.0f, 1.0f,0);
	//dirLight->Direction = XMFLOAT4(x, -1.0f, z,0);
	dirLight->GetGPULight()->HasShadow = hasShadow;
	dirLight->GetGPULight()->Resolution = SHADOWMAP_4096;
	//dirLight->Resolution = SHADOWMAP_2048;
	//dirLight->Resolution = SHADOWMAP_1024;

	AddLight(dirLight);

	//gDirLights.push_back(dirLight);
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
	pointLight->GetGPULight()->Color		= XMFLOAT4(r, g, b, 0);
	//pointLight->Position	= XMFLOAT3(x, 0.0f, z);
	pointLight->GetGPULight()->Position	= pos;
	pointLight->GetGPULight()->Range		= 300;
	pointLight->GetGPULight()->HasShadow = hasShadow;

	AddLight(pointLight);
	//gPointLights.push_back(pointLight);

	float Ox = pointLight->GetGPULight()->Position.x + 300;
	float Oz = pointLight->GetGPULight()->Position.z + 300;

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
	spotLight->GetGPULight()->Color		= XMFLOAT4(r, g, b, 0.0f);
	//spotLight->Position		= XMFLOAT3(x, 300.0f, z);
	spotLight->GetGPULight()->Position		= pos;

	//spotLight->Direction	= XMFLOAT3(MathHelper::RandF(-5.0f, 5.0f), -1, MathHelper::RandF(-5.0f, 5.0f));
	spotLight->GetGPULight()->Direction	= XMFLOAT3(0, -1, 1);

	spotLight->GetGPULight()->angle		= XMConvertToRadians(45);
	spotLight->GetGPULight()->Range		= 1000.0f;

	spotLight->GetGPULight()->HasShadow = hasShadow;
	AddLight(spotLight);
	//gSpotLights.push_back(spotLight);


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
	go->LoopAnimation("ALL");
	float speed = 160;

	if (MathHelper::RandF(0, 1) > 0.0f)
		go->SetVelocity(DirectX::XMFLOAT3(MathHelper::RandF(-speed, speed), 0, MathHelper::RandF(-speed, speed)));

	int a = (int)(MathHelper::RandF(0, 1) * 4) + 1;

	go->SetTeam((GOTeam)a);


	AddGameObject(go);
}

void Level::Update(float DeltaTime)
{
	DeltaTime	=	min( 1.0f, DeltaTime );
	vector<GameObject*> trash;
	vector<GameObject*>	tempGameObjects	=	gGameObjects;

	for each (Player *p in gPlayers)
	{
		p->Update(DeltaTime);

		if( gWave->IsLimitReached() )
		{
			PlayerUnit*	tUnit	=	p->GetUnit();
			tUnit->Drink( -1.0f * DeltaTime );
			tUnit->Eat( -1.0f * DeltaTime );
			tUnit->Hurt( 1.0f * DeltaTime );
		}
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

	

	//Updaterar ljus
	//Updaterar dirlights	
	/*for (int i = 0; i < (int)gDirLights.size(); ++i)
	{
		if (InputManager::GetInstance()->GetController(0)->GetButtonState(Xbox_Button::A) == InputState::PRESSED)
		{
			gDirLights[i]->GetGPULight()->Direction =  XMFLOAT4(1.0f, -2.0f, 1.0f,0);
		}
		else if (InputManager::GetInstance()->GetController(0)->GetButtonState(Xbox_Button::B) == InputState::PRESSED)
		{
			gDirLights[i]->GetGPULight()->Direction =  XMFLOAT4(0.0f, 1.0f, 1.0f,0);
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
	}*/


	//Updaterar pointlights
	for (int i = 0; i < (int)gPointLights.size(); ++i)
	{
		PointLight *pLight = gPointLights[i];
/*
		XMFLOAT3 rp = rotpos[i];
		float a		= angle[i];


		XMVECTOR pos  = XMLoadFloat3(&pLight->GetGPULight()->Position);
		//XMVECTOR move = XMLoadFloat3(&XMFLOAT3(0,0,0.1f));

		
		XMMATRIX transl1 = XMMatrixTranslation(-rp.x, -2000.0f, -rp.z);
		XMMATRIX transl2 = XMMatrixTranslation(rp.x, 2000.0f, rp.z);	
		XMMATRIX rot	 = XMMatrixRotationY(DeltaTime * XMConvertToRadians(a));

		XMMATRIX transf = transl1 * rot * transl2;


		pos = XMVector3TransformCoord(pos, transf);
		*/
		//pos = pos + move;

		//XMStoreFloat3(&pLight->GetGPULight()->Position, pos);

		//pLight->Position.z -= 0.1f;
		//pLight->GetGPULight()->Position.y = gTerrain->GetHeight(pLight->GetGPULight()->Position.x, pLight->GetGPULight()->Position.z) + 50;
	}
	
	//Updaterar spotlights	
	for (int i = 0; i < (int)gSpotLights.size(); ++i)
	{
		SpotLight *sLight = gSpotLights[i];

		float a		= anglespot[i];

		XMVECTOR dir  = XMLoadFloat3(&sLight->GetGPULight()->Direction);

		//XMMATRIX transl1 = XMMatrixTranslation(-sLight->Position.x, -2000.0f, -sLight->Position.z);
		//XMMATRIX transl2 = XMMatrixTranslation(sLight->Position.x, 2000.0f, sLight->Position.z);
		XMMATRIX rot	 = XMMatrixRotationY(DeltaTime * XMConvertToRadians(a));

		dir = XMVector3TransformCoord(dir, rot);

		XMStoreFloat3(&sLight->GetGPULight()->Direction, dir);
		//XMMATRIX transf = transl1 * rot * transl2;

		sLight->GetGPULight()->Position.y = gTerrain->GetHeight(sLight->GetGPULight()->Position.x, sLight->GetGPULight()->Position.z) + 100.0f;
	}

	gWave->Update( DeltaTime );
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

	for (int i = 0; i < gPlayers.size(); ++i)
	//for each( Player* p in gPlayers )
	{
		Player* p = gPlayers[i];
		if( p->GetUnit() != 0 )
			continue;
		
		XMFLOAT2	Pos	=	gNodeMap->GetRandomNode()->Position;

		PlayerUnit*	pUnit	=	new CrazyBitch();
		pUnit->MoveTo( XMFLOAT3( Pos.x, 0, Pos.y ) );
		pUnit->SetPlayerScore( p->GetPlayerScore() );
		pUnit->SetTeam( (GOTeam)team );
		pUnit->LoopAnimation( "StartPose" );
		pUnit->SetTextureIndex(i);

		Pistol* pistol = new Pistol();
		pUnit->SetWeapon(pistol);

		p->SetUnit( pUnit );

		AddGameObject( pUnit );
		AddGameObject( pistol );
		++team;
	}

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
#pragma once
#ifndef LEVEL_H
#define LEVEL_H

#include "../../../stdafx.h"
#include "../../../AI/Patrol/NodeMap.h"
#include "../Graphics/Effects.h"
#include "../Graphics/RenderStates.h"
#include "../Graphics/Buffer.h"
#include "../Graphics/ModelManager.h"
#include "../Graphics/GraphicsManager.h"
#include "../Terrain/Terrain.h"
#include "../GameObject.h"
#include "../Graphics/QuadTree.h"
#include "LevelParser.h"
#include "../Units/Unit.h"
#include "../Units/EnemyUnit.h"
#include "../Player.h"
#include "Wave.h"
#include "../Items/Item.h"
#include <functional>

static const float	AutomaticRifleSpawnTimerStop = 50.0f;
static const float  ShotgunSpawnTimerStop = 85.0f;
static const float  SniperRifleSpawnTimerStop = 120.0f;

static const float  FoodSpawnTimerStop = 30.0f;
static const float  WaterSpawnTimerStop = 30.0f;
static const float 	AmmoBoxSpawnTimerStop = 25.0f;
static const float  MedicPackSpawnTimerStop = 50.0f;

static const float  StandardArea = 100 * 100 * UnitsPerMeter * UnitsPerMeter;

struct SystemData
{
	SystemData(){}

	ID3D11Device*			DEVICE;
	ID3D11DeviceContext*	DEVICE_CONTEXT;
	ID3D11RenderTargetView*	RENDER_TARGET_VIEW;
	int	SCREEN_WIDTH, SCREEN_HEIGHT;
};

struct CollisionEvent
{
	GameObject* A;
	GameObject* B;

	vector<CollisionData> CD;

	CollisionEvent(GameObject* InstanceA, GameObject* InstanceB, vector<CollisionData> collisionData)
	{
		A	=	InstanceA;
		B	=	InstanceB;
		CD  =   collisionData;
	}

	bool operator==(CollisionEvent Event)
	{
		return (Event.A == A || Event.B == A) && ( Event.A == B || Event.B == B );
	}
};

using namespace std;
class Level
{
private:

	SystemData			gLData;

	GraphicsManager*	gGraphicsManager;
	Terrain*			gTerrain;

	vector<Player*>		gPlayers;
	NodeMap*			gNodeMap;
	Wave*				gWave;

	//	Lights
	vector<DirectionalLight*>	gDirLights;
	vector<PointLight*>			gPointLights;
	vector<SpotLight*>			gSpotLights;
	XMFLOAT3					gGlobalLight;

	vector<XMFLOAT3> rotpos;
	vector<float>	 angle;
	vector<float>	 anglespot;


	//GameObjects
	vector<GameObject*>		gGameObjects;
	QuadTree				*gQuadTree;

	void BlockPathWithObject(GameObject* go)
	{
		vector<BoundingOrientedBox> boxes = go->GetBoundingBoxes();

		for each (BoundingOrientedBox box in boxes)
		{
			gTerrain->BlockPath(box);
		}		
	}

	void SetCallbackFunctions(GameObject* go);

	void AddGameObject(GameObject* go);
	
	void RemoveGameObject(GameObject* go) 
	{ 
		gQuadTree->Delete(go); 
		gGameObjects.erase(remove(gGameObjects.begin(), gGameObjects.end(), go), gGameObjects.end());
	}
	
	void DeleteGameObject(GameObject* go)
	{
		RemoveGameObject(go);

		if( IsOfType<EnemyUnit>( go ) )
			gWave->UnitKilled();

		if ( go->GetState() == Dead )
		{
			if( IsOfType<PlayerUnit>( go ) )
				for each( Player* p in gPlayers )
					if( p->GetUnit() == go )
						p->SetUnit( 0 );
			delete go;
			go	=	0;
		}
	}

	void AddLight(Light* light)
	{
		
		if (IsOfType<DirectionalLight>(light))
			gDirLights.push_back((DirectionalLight*) light);

		else if (IsOfType<PointLight>(light))
			gPointLights.push_back((PointLight*) light);

		else if (IsOfType<SpotLight>(light))
			gSpotLights.push_back((SpotLight*) light);			
	}
	
	void RemoveLight(Light* light)
	{
		if (IsOfType<DirectionalLight>(light))
		{
			gDirLights.erase(remove(gDirLights.begin(), gDirLights.end(), light), gDirLights.end()); 
		}

		else if (IsOfType<PointLight>(light))
		{
			gPointLights.erase(remove(gPointLights.begin(), gPointLights.end(), light), gPointLights.end()); 
		}

		else if (IsOfType<SpotLight>(light))
		{
			gSpotLights.erase(remove(gSpotLights.begin(), gSpotLights.end(), light), gSpotLights.end()); 
		}
	}
	void DeleteLight(Light *light)
	{
		RemoveLight( light );
		SAFE_DELETE( light );
	}

	float AutomaticRifleSpawnTimer;
	float ShotgunSpawnTimer;
	float SniperRifleSpawnTimer;

	float FoodSpawnTimer;
	float WaterSpawnTimer;
	float AmmoBoxSpawnTimer;
	float MedicPackSpawnTimer;
	
	void RunCollisionTest();

public:
	Level(void);
	Level(SystemData Data, GraphicsManager* Instance);
	~Level(void);

	void	LoadLevel(string Levelname);

	void	Update(float DeltaTime);

	void	UpdateSpectator(Player* p);

	void	Render();
	void	SetNumberOfPlayers(int noPlayers, int screenWidth, int screenHeight);

	Terrain*	GetTerrain();

	vector<GameObject*>	GetGameObjects() { return gGameObjects; }

	vector<Player*>		GetPlayers()	{ return gPlayers; }

	bool	IsGameOver()
	{
		bool	allDead	=	true;
		for each( Player* p in gPlayers )
			if( p->GetUnit() != 0 )
				allDead	=	false;

		return allDead;
	}

	void	SpawnItem(Item* item);
	void	SpawnItems(float deltaTime);

	Wave::WaveGUIInfo	GetWaveInfo(){ return gWave->GetGUIInfo(); }
};

#endif
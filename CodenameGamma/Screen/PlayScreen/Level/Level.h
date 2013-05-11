#pragma once
#ifndef LEVEL_H
#define LEVEL_H

#include "../../../stdafx.h"
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
#include "../Player.h"


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

	//	Lights
	vector<DirectionalLight*>	gDirLights;
	vector<PointLight*>			gPointLights;
	vector<SpotLight*>			gSpotLights;

	vector<XMFLOAT3> rotpos;
	vector<float>	 angle;
	vector<float>	 anglespot;


	//GameObjects
	vector<GameObject*>		gGameObjects;
	QuadTree				*gQuadTree;


	void	AddDirectionalLight(bool hasShadow);
	void	AddPointLight(bool hasShadow, XMFLOAT3 pos);
	void	AddSpotLight(bool hasShadow,  XMFLOAT3 pos);
	void	AddInstance(float x, float y, float z, Model *model);

	void AddGameObject(GameObject* go) 
	{ 
		gGameObjects.push_back(go); 
		sort( gGameObjects.begin(), gGameObjects.end() );
		gQuadTree->Insert(go); 
	}

	void RemoveGameObject(GameObject* go) 
	{ 
		gQuadTree->Delete(go); 
		gGameObjects.erase(remove(gGameObjects.begin(), gGameObjects.end(), go), gGameObjects.end());		

		delete go;
	}

	void RunCollisionTest();

public:
	Level(void);
	Level(SystemData Data);
	~Level(void);

	void	LoadLevel(string Levelname);

	void	Update(float DeltaTime);
	void	Render();
	void	SetNumberOfPlayers(int noPlayers, int screenWidth, int screenHeight);

	Terrain*	GetTerrain();

	vector<GameObject*>	GetGameObjects() { return gGameObjects; }

	vector<Player*>		GetPlayers()	{ return gPlayers; }
};

#endif
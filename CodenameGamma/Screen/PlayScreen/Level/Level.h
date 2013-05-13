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
#include <functional>
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

	void AddGameObject(GameObject* go);
	
	void RemoveGameObject(GameObject* go) 
	{ 
		gQuadTree->Delete(go); 
		gGameObjects.erase(remove(gGameObjects.begin(), gGameObjects.end(), go), gGameObjects.end());		
	}
	
	void DeleteGameObject(GameObject* go)
	{
		RemoveGameObject(go);

		if ( go->GetState() != Hidden )
			delete go;
	}

	/*
	void AddLight(DirectionalLight* light) { gDirLights.push_back(light); }
	void AddLight(PointLight* light) { gPointLights.push_back(light); }
	void AddLight(SpotLight* light) { gSpotLights.push_back(light); }
	
	void RemoveLight(DirectionalLight* light) { gDirLights.erase(remove(gDirLights.begin(), gDirLights.end(), light), gDirLights.end()); delete light; }
	void RemoveLight(PointLight* light) { gPointLights.erase(remove(gPointLights.begin(), gPointLights.end(), light), gPointLights.end()); delete light; }
	void RemoveLight(SpotLight* light) { gSpotLights.erase(remove(gSpotLights.begin(), gSpotLights.end(), light), gSpotLights.end()); delete light; }
	*/
	
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
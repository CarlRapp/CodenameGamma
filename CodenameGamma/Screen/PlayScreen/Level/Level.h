#pragma once
#ifndef LEVEL_H
#define LEVEL_H

#include "../../../stdafx.h"
#include "../Graphics/Effects.h"
#include "../Graphics/RenderStates.h"
#include "../Graphics/Buffer.h"
#include "../Graphics/TextureManager.h"
#include "../Graphics/GraphicsManager.h"
#include "../Terrain/Terrain.h"
#include "../GameObject.h"
#include "../Graphics/QuadTree.h"
#include "LevelParser.h"

struct SystemData
{
	SystemData(){}

	ID3D11Device*			DEVICE;
	ID3D11DeviceContext*	DEVICE_CONTEXT;
	ID3D11RenderTargetView*	RENDER_TARGET_VIEW;
	int	SCREEN_WIDTH, SCREEN_HEIGHT;
};

using namespace std;
class Level
{
private:

	SystemData			gLData;

	TextureManager		gTextureManager;

	GraphicsManager*	gGraphicsManager;
	Terrain*			gTerrain;

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

	//	Loaded Models
	vector<Model*>			gLoadedModels;


	void	AddDirectionalLight(DirectionalLight* Instance);
	void	AddPointLight(PointLight* Instance);
	void	AddSpotLight(SpotLight* Instance);
	void	AddInstance(float x, float y, float z, Model *model);

	void AddGameObject(GameObject* go) 
	{ 
		gGameObjects.push_back(go); 
		sort( gGameObjects.begin(), gGameObjects.end() );
		gQuadTree->Insert(go); 
	}

	void RemoveGameObject(GameObject* go) 
	{ 
		if (!gGameObjects.empty()) 
		{ 
			gQuadTree->Delete(go); 
			gGameObjects.erase(remove(gGameObjects.begin(), gGameObjects.end(), go), gGameObjects.end());
		}

		delete go;
	}

public:
	Level(void);
	Level(SystemData Data);
	~Level(void);

	void	LoadLevel(string Levelname);

	void	Update(float DeltaTime);
	void	Render(vector<Camera*>& Cameras);

	Terrain*	GetTerrain();

	vector<GameObject*>	GetGameObjects() { return gGameObjects; }
};

#endif
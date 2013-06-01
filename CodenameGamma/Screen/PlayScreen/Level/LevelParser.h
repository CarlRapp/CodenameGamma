
#pragma once
#ifndef LEVELPARSER_H
#define LEVELPARSER_H


#include "../../../stdafx.h"
#include "../../ScreenEnums.h"
#include "../GameObject.h"
#include "../Graphics/LightHelper.h"
#include "../../../AI/Patrol/NodeMap.h"
#include "LevelData.h"
#include <fstream>

using namespace std;

struct GameObjectData
{
	GameObject*	gameObject;
	bool		vegetation;
	int			textureIndex;

	GameObjectData()
	{
		gameObject = NULL;
		vegetation = false;
		textureIndex = 0;
	}
};

struct EntityData
{
	EntityData() {}
	vector<GameObjectData*>	gameObjectData;
	vector<Light*>		Lights;
	NodeMap*			NodeMapInstance;
};

class LevelParser
{
private:
	LevelParser(void);
	static	LevelParser*	gInstance;

	static	string	GetToken(string Line, bool IncludeDigits);

	struct	GO_Name
	{
		GO_Name(){}

		string	Name;
		bool	HasVegetation;
		int		TextureIndex;

	};

	static	GameObject*	GetGameObject( string GameObjectName );
	static	GameObjectData*	ParseGameObject( string Line, GO_Name GOData, LevelData Data );
	static	SpotLight*	ParseSpotLight( string Line, LevelData Data );
	static	PointLight*	ParsePointLight( string Line, LevelData Data );
	static	PatrolNode*	ParsePatrolNode( string Line, LevelData Data );

public:
	~LevelParser(void);
	static	LevelData	ParseLevel(string LevelName, string LevelsRootPath);
	static	EntityData	ParseLevelEntities(LevelData Data);
};

#endif
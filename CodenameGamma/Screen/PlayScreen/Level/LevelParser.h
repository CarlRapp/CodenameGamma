
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
struct EntityData
{
	EntityData() {}
	vector<GameObject*>	GameObjects;
	vector<Light*>		Lights;
	NodeMap*			NodeMapInstance;
};

class LevelParser
{
private:
	LevelParser(void);
	static	LevelParser*	gInstance;

	static	string	GetToken(string Line, bool IncludeDigits);

	static	GameObject*	GetGameObject( string GameObjectName );
	static	GameObject*	ParseGameObject( string Line, string GameObjectName, LevelData Data );
	static	SpotLight*	ParseSpotLight( string Line, LevelData Data );
	static	PointLight*	ParsePointLight( string Line, LevelData Data );
	static	PatrolNode*	ParsePatrolNode( string Line, LevelData Data );

public:
	~LevelParser(void);
	static	LevelData	ParseLevel(string LevelName, string LevelsRootPath);
	static	EntityData	ParseLevelEntities(LevelData Data);
};

#endif
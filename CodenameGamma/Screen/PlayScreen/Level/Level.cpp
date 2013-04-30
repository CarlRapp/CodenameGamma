#include "Level.h"


Level::Level(){}
Level::Level(LevelData LData)
{
	gTerrain	=	new Terrain(LData.DEVICE, LData.DEVICE_CONTEXT);
	gTerrain->LoadMap("DATA\\Maps\\TestMap");
}

Level::~Level()
{
	gTerrain->~Terrain();
}
















Terrain* Level::GetTerrain()
{	
	return gTerrain;
}
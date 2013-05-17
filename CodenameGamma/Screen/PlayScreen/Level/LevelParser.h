#pragma once
#ifndef LEVELPARSER_H
#define LEVELPARSER_H

#include "../../../stdafx.h"
#include "../../ScreenEnums.h"
#include <fstream>

using namespace std;

struct LevelTexture
{
	LevelTexture(){}
	string	Texture;
	string	NormalMap;
};
struct LevelMap
{
	LevelMap(){}
	string	Filename;
	int		Width, Height;
};
struct LevelData
{
	LevelData(){}
	string	LevelRootPath;
	int	Width, Height;
	int	ResolutionX, ResolutionY;
	int	TextureX, TextureY;
	int PathMapResX, PathMapResY, NumPathMaps;

	LevelTexture	Textures[4];
	LevelMap		HeightMap;
	string			BlendMap;

	bool IsLoaded()
	{
		return ( Width > 0 );
	}
};

class LevelParser
{
private:
	LevelParser(void);
	static	LevelParser*	gInstance;

	static	string	GetToken(string Line, bool IncludeDigits);

public:
	~LevelParser(void);
	static	LevelData	ParseLevel(string LevelName, string LevelsRootPath);




};

#endif
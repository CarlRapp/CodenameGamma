#pragma once
#ifndef LEVELDATA_H
#define LEVELDATA_H

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
	int PathMapResX, PathMapResY;
	int	NumPathMaps;

	LevelTexture	Textures[4];
	LevelMap		HeightMap;
	string			BlendMap;

	XMFLOAT4		SunColor;
	XMFLOAT4		SunDirection;
	XMFLOAT3		GlobalLight;

	bool IsLoaded()
	{
		return ( Width > 0 );
	}
};

#endif
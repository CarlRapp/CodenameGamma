#pragma once
#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H	
#include "..\..\..\stdafx.h"
#include <iostream>
#include <vector>
class HeigthMap
{
private:
	float** m_Map;
	int		m_Width;
	int		m_Height;

private:
	void Filter(int iterations);

public:
	HeigthMap();
	~HeigthMap(void);

	void Load(std::string filename, int Width, int Height);

	float GetY(float x, float z);
	float GetHeight(float x, float z);
};
#endif


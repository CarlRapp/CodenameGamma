#pragma once
#ifndef PATHMAP_H
#define PATHMAP_H	
#include "..\..\..\stdafx.h"

class PathMap
{
	bool** m_Map;

	int		m_Width;
	int		m_Height;

public:
	PathMap(void);
	~PathMap(void);

	void Init(int Width, int Height);

	void SetWalkable(bool walkable, float x, float z);

	bool IsWalkable(float x, float z);
};

#endif
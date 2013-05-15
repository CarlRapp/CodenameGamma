#include "PathMap.h"


PathMap::PathMap(void)
{
	Init(1,1);
}


PathMap::~PathMap(void)
{
}



void PathMap::Init(int Width, int Height)
{
	m_Width		= Width;
	m_Height	= Height;

	m_Map = new bool*[Height];
	for (int z = 0; z < Height; z++)
		m_Map[z] = new bool[Width];
	
	//sätter alla noderna till true (walkable)
	for (int z = 0; z < Height; z++)
	{
		int Z = Height - 1 - z;
		for (int x = 0; x < Width; x++)
		{
			m_Map[Z][x] = true;
		}
	}
}

void PathMap::SetWalkable(bool walkable, float x, float z)
{
	if (x < 0 || x >= 1 || z < 0 || z >= 1 ) 
		return;

	int X = x * m_Width;
	int Y = z * m_Height;

	m_Map[Y][X] = walkable;
}

bool PathMap::IsWalkable(float x, float z)
{
	if (x < 0 || x >= 1 || z < 0 || z >= 1 ) 
		return false;

	int X = x * m_Width;
	int Y = z * m_Height;

	return m_Map[Y][X];
}
#pragma once
#ifndef PATHMAP_H
#define PATHMAP_H	
#include "..\..\..\stdafx.h"
#include <set>
#include <queue>
#include <iostream>
#include <unordered_map>

using namespace std;

struct Node
{
	int		m_X, m_Y;
	int		m_Id;
	Node	*m_Parent;

	float	G;	//Start till hit
	float	H;	//Här ifrån till mål (gissning)


	Node() : m_Parent(0) {}

	Node(int x, int y, int id, Node *parent = 0)
	{
		m_X		= x;
		m_Y		= y;
		m_Id	= id;
		
		m_Parent = parent;

		G = 0;
		H = 0;
	}

	float GetF() { return G + H; }

	int ManHattanDistance(int _x, int _y)
	{
		int x = abs(m_X - _x);
		int y = abs(m_Y - _y);

		return x + y;
	}

	bool operator==(Node n)
	{
		return (n.m_Id == m_Id);
	}
	
	bool operator()(Node* A, Node* B) 
	{
		float AF = A->GetF();
		float BF = B->GetF();

		if (AF == BF)		
			return A->H > B->H;
		return AF > BF;
	}
};

struct Map
{
	bool**	m_Map;
	int		m_Width;
	int		m_Height;

	void Init(int Width, int Height);

	void BlockPath(BoundingOrientedBox box);

	bool FindPath(XMFLOAT2 A, XMFLOAT2 B, vector<XMFLOAT2>& path);

	void SetWalkable(bool walkable, float x, float y);
	bool IsWalkable(float x, float y);
	bool IsShortestPathFree(XMFLOAT2 A, XMFLOAT2 B);

private:

	//Globals during pathfinding
	priority_queue<Node*, vector<Node*>, Node> openList;
	set<int>			  closedList;

	Node* currentNode;
	int EndX;
	int EndY;

	void AddToOpen(int x, int y, int ID, float distance)
	{
		if (closedList.find(ID) != closedList.end())
			return;

		Node* temp = new Node(x, y, GenerateID(x, y), currentNode);
		temp->G = distance;
		temp->H = temp->ManHattanDistance(EndX, EndY);
		openList.push(temp);
	}

	bool IsWalkable(int x, int y);

	int  GenerateID(int x, int y) { return y * m_Width + x; }

	static bool TileIntersectsOBB(XMFLOAT2 min, XMFLOAT2 max, BoundingOrientedBox box)
	{
		XMVECTOR minV = XMLoadFloat3(&XMFLOAT3(min.x, -10, min.y));
		XMVECTOR maxV = XMLoadFloat3(&XMFLOAT3(max.x, 10, max.y));
		XMVECTOR centerV = (minV + maxV) / 2;
		XMVECTOR extentsV = centerV - minV;

		XMFLOAT3 center, extents;
		XMStoreFloat3(&center, centerV);
		XMStoreFloat3(&extents, extentsV);

		BoundingBox tile = BoundingBox(center, extents);

		return tile.Intersects(box);
	}

};

class PathMap
{
#define numMaps 4

	Map m_Maps[numMaps];
public:
	PathMap(void);
	~PathMap(void);	

	void Init(int Width, int Height);

	void BlockPath(BoundingOrientedBox box);

	bool FindPath(XMFLOAT2 A, XMFLOAT2 B, vector<XMFLOAT2>& path);

	bool IsShortestPathFree(XMFLOAT2 A, XMFLOAT2 B);	

	void SetWalkable(bool walkable, float x, float y);

	bool IsWalkable(float x, float y);
};

#endif
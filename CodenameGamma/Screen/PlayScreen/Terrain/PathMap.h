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

	float DistanceLineVsLine(XMFLOAT2 A1, XMFLOAT2 A2, XMFLOAT2 B1, XMFLOAT2 B2) 
	{
		// Store the values for fast access and easy
		// equations-to-code conversion
		float x1 = A1.x, x2 = A2.x, x3 = B1.x, x4 = B2.x;
		float y1 = A1.y, y2 = A2.y, y3 = B1.y, y4 = B2.y;
 
		float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		// If d is zero, there is no intersection
		if (d == 0) return INFINITE;
 
		// Get the x and y
		float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
		float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
		float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
		// Check if the x and y coordinates are within both lines
		if ( x < min(x1, x2) || x > max(x1, x2) ||
		x < min(x3, x4) || x > max(x3, x4) ) return INFINITE;
		if ( y < min(y1, y2) || y > max(y1, y2) ||
		y < min(y3, y4) || y > max(y3, y4) ) return INFINITE;
 
		// Return the point of intersection

		XMVECTOR P1 = XMLoadFloat2(&A1);
		XMVECTOR P2 = XMLoadFloat2(&XMFLOAT2(x, y));

		XMVECTOR L  = XMVector2Length(P2-P1);
		float l;
		XMStoreFloat(&l, L);

		return l;
	}

	static float LineVSLine(XMFLOAT2 a1, XMFLOAT2 a2, XMFLOAT2 b1, XMFLOAT2 b2)
	{
		XMVECTOR A1 = XMLoadFloat2(&a1);
		XMVECTOR A2 = XMLoadFloat2(&a2);
		XMVECTOR B1 = XMLoadFloat2(&b1);
		XMVECTOR B2 = XMLoadFloat2(&b2);

		XMVECTOR A = A2 - A1;
		XMVECTOR B = B2 - B1;

		//XMVector2Dot(A, B


	}

	static bool TileVSLine(XMFLOAT2 min, XMFLOAT2 max, XMFLOAT2 A, XMFLOAT2 B);

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
	int m_NumMaps;

	vector<Map> m_Maps;
public:
	PathMap(void);
	~PathMap(void);	

	void Init(int Width, int Height, int numMaps);

	void BlockPath(BoundingOrientedBox box);

	bool FindPath(XMFLOAT2 A, XMFLOAT2 B, vector<XMFLOAT2>& path);

	bool IsShortestPathFree(XMFLOAT2 A, XMFLOAT2 B);	

	void SetWalkable(bool walkable, float x, float y);

	bool IsWalkable(float x, float y);
};

#endif
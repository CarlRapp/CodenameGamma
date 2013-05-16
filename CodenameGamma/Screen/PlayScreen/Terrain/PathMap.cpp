#include "PathMap.h"


void Map::Init(int Width, int Height)
{
	m_Width		= Width;
	m_Height	= Height;

	m_Map = new bool*[Height];
	for (int y = 0; y < Height; y++)
		m_Map[y] = new bool[Width];
	
	//sätter alla noderna till true (walkable)
	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			m_Map[y][x] = true;
		}
	}
}

void Map::BlockPath(BoundingOrientedBox box)
{
	XMMATRIX scale;
	scale = XMMatrixScaling((float)m_Width, 1.0f, (float)m_Height);

	//skala upp boxen
	box.Transform(box, scale);

	//Flyttar boxens centrum
	box.Center.y = 0;
	box.Center.x *= m_Width;
	box.Center.z *= m_Height;

	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			XMFLOAT2 min = XMFLOAT2(x, y);
			XMFLOAT2 max = XMFLOAT2(x + 1, y + 1);
			if (TileIntersectsOBB(min, max, box))
				m_Map[y][x] = false;
		}
	}
}

bool Map::FindPath(XMFLOAT2 A, XMFLOAT2 B, vector<XMFLOAT2>& path)
{
	if (A.x < 0 || A.y < 0 || A.x >= 1 || A.y >= 1  ||  B.x < 0 || B.y < 0 || B.x >= 1 || B.y >= 1) 
		return false;

	//Reset open and close list
	openList = priority_queue<Node*>();
	closedList.clear();

	//Ändrar om från [(0, 0) - (1, 1)] till [(0, 0) - (m_Width, m_Height)]
	int StartX = B.x * m_Width;
	int StartY = B.y * m_Height;

	EndX = A.x * m_Width;
	EndY = A.y * m_Height;

	if (!IsWalkable(EndX, EndY))
		return false;

	Node* startNode = new Node(StartX, StartY, GenerateID(StartX, StartY), NULL);
	Node* endNode = NULL;
	//Node* endNode = new Node(EndX, EndY, GenerateID(EndX, EndY), NULL);

	int endID = GenerateID(EndX, EndY);

	openList.push(startNode);

	do
	{
		currentNode = openList.top();
		openList.pop();

		int X = currentNode->m_X;
		int Y = currentNode->m_Y;

		float G = currentNode->G;

		bool Up		= false;
		bool Down	= false;
		bool Right	= false;
		bool Left	= false;

#pragma region AddToOpen
		//Up
		if (IsWalkable(X, Y + 1))
		{			
			int ID = GenerateID(X, Y + 1);
			if (ID == endID)
			{
				endNode = new Node(X, Y + 1, ID, currentNode);
				break;
			}
			Up = true;
			AddToOpen(X, Y + 1, ID, G + 1.0f);
		}

		//Down
		if (IsWalkable(X, Y - 1))
		{			
			int ID = GenerateID(X, Y - 1);
			if (ID == endID)
			{
				endNode = new Node(X, Y - 1, ID, currentNode);
				break;
			}
			Down = true;
			AddToOpen(X, Y - 1, ID, G + 1.0f);
		}

		//Right
		if (IsWalkable(X + 1, Y))
		{			
			int ID = GenerateID(X + 1, Y);
			if (ID == endID)
			{
				endNode = new Node(X + 1, Y, ID, currentNode);
				break;
			}
			Right = true;
			AddToOpen(X + 1, Y, ID, G + 1.0f);
		}

		//Left
		if (IsWalkable(X - 1, Y))
		{			
			int ID = GenerateID(X - 1, Y);
			if (ID == endID)
			{
				endNode = new Node(X - 1, Y, ID, currentNode);
				break;
			}
			Left = true;
			AddToOpen(X - 1, Y, ID, G + 1.0f);
		}

		//RightUp
		if (Right && Up)
		{
			if (IsWalkable(X + 1, Y + 1))
			{			
				int ID = GenerateID(X + 1, Y + 1);
				if (ID == endID)
				{
					endNode = new Node(X + 1, Y + 1, ID, currentNode);
					break;
				}
				AddToOpen(X + 1, Y + 1, ID, G + 1.4142f);
			}
		}

		//LeftUp
		if (Left && Up)
		{
			if (IsWalkable(X - 1, Y + 1))
			{			
				int ID = GenerateID(X - 1, Y + 1);
				if (ID == endID)
				{
					endNode = new Node(X - 1, Y + 1, ID, currentNode);
					break;
				}
				AddToOpen(X - 1, Y + 1, ID, G + 1.4142f);
			}
		}

		//RightDown
		if (Right && Down)
		{
			if (IsWalkable(X + 1, Y - 1))
			{
				int ID = GenerateID(X + 1, Y - 1);
				if (ID == endID)
				{
					endNode = new Node(X + 1, Y - 1, ID, currentNode);
					break;
				}
				AddToOpen(X + 1, Y - 1, ID, G + 1.4142f);
			}
		}

		//LeftDown
		if (Left && Down)
		{
			if (IsWalkable(X - 1, Y - 1))
			{			
				int ID = GenerateID(X - 1, Y - 1);
				if (ID == endID)
				{
					endNode = new Node(X - 1, Y - 1, ID, currentNode);
					break;
				}
				AddToOpen(X - 1, Y - 1, ID, G + 1.4142f);
			}
		}
#pragma endregion

		closedList.insert(GenerateID(X, Y));

	}
	while (!openList.empty() && currentNode);

	//Bygg pathen
	if (endNode != NULL)
	{
		float offsetX = 0.5f / (float)m_Width;
		float offsetY = 0.5f / (float)m_Height;
		while (endNode != NULL)
		{
			//Ändrar om från [(0, 0) - (m_Width, m_Height)] till [(0, 0) - (1, 1)]
			float x = ((float)endNode->m_X / (float)m_Width)  + offsetX;
			float y = ((float)endNode->m_Y / (float)m_Height) + offsetY;
			path.push_back(XMFLOAT2(x, y));

			endNode = endNode->m_Parent;
		}
		return  false;
	}

	return false;
}

bool Map::IsShortestPathFree(XMFLOAT2 A, XMFLOAT2 B)
{
	/*
	if (A.x < 0 || A.x >= 1 || A.y < 0 || A.y >= 1   ||    B.x < 0 || B.x >= 1 || B.y < 0 || B.y >= 1) 
		return false;

	XMVECTOR nodeA = XMLoadUInt2(&XMUINT2(A.x * m_Width, A.y * m_Height));
	XMVECTOR nodeB = XMLoadUInt2(&XMUINT2(B.x * m_Width, B.y * m_Height));

	if (XMVector2Equal(nodeA, nodeB))
		return true;

	XMVECTOR direction = posB - posA;
	XMVECTOR lengthV   = XMVector2Length(direction);

	float length;
	
	XMStoreFloat(&length, lengthV);
	direction = direction / length;

	int steps = ceil(length);
	XMVECTOR posTempV = posA;
	XMFLOAT2 posTemp;
	while (true)
	{
		XMStoreFloat2(&posTemp, posTempV);

		if (!IsWalkable(posTemp.x, posTemp.y))
			return false;
		else if (

	}

	for (int i = 0; i < ceil(length); ++i)
	{

	}



	XMFLOAT2 step;
	XMStoreFloat2(&step, direction);*/
	return true;
}

bool Map::IsWalkable(int x, int y)
{
	if (x >= m_Width || y >= m_Height ) 
		return false;

	return m_Map[y][x];
}

bool Map::IsWalkable(float x, float y)
{
	if (x < 0 || y < 0 ) 
		return false;

	int X = x * m_Width;
	int Y = y * m_Height;

	return IsWalkable(X, Y);
}

void Map::SetWalkable(bool walkable, float x, float y)
{
	if (x < 0 || x >= 1 || y < 0 || y >= 1 ) 
		return;

	int X = x * m_Width;
	int Y = y * m_Height;

	m_Map[Y][X] = walkable;
}

PathMap::PathMap(void)
{
	Init(1,1);
}

PathMap::~PathMap(void)
{
}

void PathMap::Init(int Width, int Height)
{
	for (int i = 0; i < numMaps; ++i)
	{
		float factor = pow(0.5f, i);
		m_Maps[i].Init(ceil((float)Width * factor), ceil((float)Height * factor));
	}
}

void PathMap::BlockPath(BoundingOrientedBox box)
{
	for (int i = 0; i < numMaps; ++i)
	{
		m_Maps[i].BlockPath(box);
	}
}

bool PathMap::FindPath(XMFLOAT2 A, XMFLOAT2 B, vector<XMFLOAT2>& path)
{
	path.clear();
	for (int i = numMaps - 1; i >= 0; --i)
	{
		if (m_Maps[i].FindPath(A, B, path))
		{
			return true;
		}
	}
	return false;

	/*
	for (int i = numMaps - 1; i >= 1; --i)
	{
		
		vector<XMFLOAT2> temp1;
		if (m_Maps[i].FindPath(A, B, temp1))
		{
			for (int j = 0; j < temp1.size() - 1; ++j)
			{
				XMFLOAT2 tempA = temp1[j];
				XMFLOAT2 tempB = temp1[j + 1];

				vector<XMFLOAT2> temp2;
				if (!m_Maps[0].FindPath(tempA, tempB, temp2))
					return false;
				
				for (int k = 0; k < temp2.size(); ++k)
				{
					path.push_back(temp2[k]);
				}
			}
			return true;
		}

	}

	return m_Maps[0].FindPath(A, B, path);
	*/
}

bool PathMap::IsShortestPathFree(XMFLOAT2 A, XMFLOAT2 B)
{
	return m_Maps[0].IsShortestPathFree(A, B);
}	

void PathMap::SetWalkable(bool walkable, float x, float y)
{
	for (int i = 0; i < numMaps; ++i)
	{
		m_Maps[i].SetWalkable(walkable, x, y);
	}
}

bool PathMap::IsWalkable(float x, float y)
{
	return m_Maps[0].IsWalkable(x, y);
}


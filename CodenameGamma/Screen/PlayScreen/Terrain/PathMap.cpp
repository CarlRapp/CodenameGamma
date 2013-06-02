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
	//box.Center.x *= m_Width;
	//box.Center.z *= m_Height;
	
	XMFLOAT3 corners[BoundingOrientedBox::CORNER_COUNT];
	box.GetCorners(&corners[0]);

	BoundingBox AABB;
	AABB.CreateFromPoints(AABB, BoundingOrientedBox::CORNER_COUNT, &corners[0], sizeof(XMFLOAT3));
	
	XMVECTOR Center = XMLoadFloat3(&AABB.Center);
	XMVECTOR Extents = XMLoadFloat3(&AABB.Extents);

	XMVECTOR minV = Center - Extents;
	XMVECTOR maxV = Center + Extents;

	XMFLOAT3 min, max;
	XMStoreFloat3(&min, minV);
	XMStoreFloat3(&max, maxV);

	int stopX = min(ceil(max.x), m_Width);
	int stopY = min(ceil(max.z), m_Height);

	int startX = max(floor(min.x), 0);
	int startY = max(floor(min.z), 0);

	for (int y = startY; y < stopY; y++)
	{
		for (int x = startX; x < stopX; x++)
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
	openList = priority_queue<Node*, vector<Node*>, Node>();
	closedList.clear();

	//Ändrar om från [(0, 0) - (1, 1)] till [(0, 0) - (m_Width, m_Height)]
	int StartX = A.x * m_Width;
	int StartY = A.y * m_Height;

	EndX = B.x * m_Width;
	EndY = B.y * m_Height;

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

		//poppar ut de noder som vi redan har besökt.
		while (true)
		{
			if (openList.empty())
				break;

			else if (closedList.find(openList.top()->m_Id) != closedList.end())
				openList.pop();

			else 
				break;
		}
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
		return  true;
	}

	return false;
}

bool Map::IsShortestPathFree(XMFLOAT2 Start, XMFLOAT2 End)
{
	float x0 = Start.x * m_Width;
	float y0 = Start.y * m_Height;
	float x1 = End.x   * m_Width;
	float y1 = End.y   * m_Height;

	float dx = fabs(x1 - x0);
    float dy = fabs(y1 - y0);

    int x = int(floor(x0));
    int y = int(floor(y0));

    int n = 1;
    int x_inc, y_inc;
    float error;

    if (dx == 0)
    {
        x_inc = 0;
        error = std::numeric_limits<float>::infinity();
    }
    else if (x1 > x0)
    {
        x_inc = 1;
        n += int(floor(x1)) - x;
        error = (floor(x0) + 1 - x0) * dy;
    }
    else
    {
        x_inc = -1;
        n += x - int(floor(x1));
        error = (x0 - floor(x0)) * dy;
    }

    if (dy == 0)
    {
        y_inc = 0;
        error -= std::numeric_limits<float>::infinity();
    }
    else if (y1 > y0)
    {
        y_inc = 1;
        n += int(floor(y1)) - y;
        error -= (floor(y0) + 1 - y0) * dx;
    }
    else
    {
        y_inc = -1;
        n += y - int(floor(y1));
        error -= (y0 - floor(y0)) * dx;
    }

    for (; n > 0; --n)
    {

		if (!IsWalkable(x, y))
			return false;
       // visit(x, y);

        if (error > 0)
        {
            y += y_inc;
            error -= dx;
        }
        else
        {
            x += x_inc;
            error += dy;
        }
    }
	return true;
}
/*
bool Map::IsShortestPathFree(XMFLOAT2 Start, XMFLOAT2 End)
{
	XMVECTOR A = XMLoadFloat2(&XMFLOAT2(Start.x * m_Width, Start.y * m_Height));
	XMVECTOR B = XMLoadFloat2(&XMFLOAT2(End.x * m_Width, End.y * m_Height));

	float distance;
	XMVECTOR distanceV;
	XMFLOAT2 DirectionN;

	XMVECTOR Direction = B - A;	
	distanceV = XMVector2Length(Direction);
	XMStoreFloat(&distance, distanceV);

	Direction = XMVector2Normalize(Direction);
	XMStoreFloat2(&DirectionN, Direction);

	
	while (distance >= 0.0f)
	{
		XMFLOAT2 Atemp;
		XMStoreFloat2(&Atemp, A);

		if (!IsWalkable((int)Atemp.x, (int)Atemp.y))
			return false;

		float distanceToNextTile = INFINITE;
		if (DirectionN.y > 0)//up
		{
			XMFLOAT2 LStart = XMFLOAT2((int)(Atemp.x), (int)(Atemp.y + 1));
			XMFLOAT2 LEnd   = XMFLOAT2((int)(Atemp.x + 1), (int)(Atemp.y + 1));
			distanceToNextTile =  min(distanceToNextTile, DistanceLineVsLine(Atemp, End, LStart, LEnd));
		}
		else if (DirectionN.y < 0)//down
		{
			XMFLOAT2 LStart = XMFLOAT2((int)(Atemp.x),	   ceil(Atemp.y) - 1);
			XMFLOAT2 LEnd   = XMFLOAT2((int)(Atemp.x + 1), ceil(Atemp.y) - 1);
			distanceToNextTile =  min(distanceToNextTile, DistanceLineVsLine(Atemp, End, LStart, LEnd));
		}

		if (DirectionN.x > 0)//right
		{
			XMFLOAT2 LStart = XMFLOAT2((int)(Atemp.x + 1), (int)(Atemp.y));
			XMFLOAT2 LEnd   = XMFLOAT2((int)(Atemp.x + 1), (int)(Atemp.y + 1));
			distanceToNextTile =  min(distanceToNextTile, DistanceLineVsLine(Atemp, End, LStart, LEnd));
		}
		else if (DirectionN.x < 0)//left
		{
			XMFLOAT2 LStart = XMFLOAT2(ceil(Atemp.x) - 1, (int)(Atemp.y));
			XMFLOAT2 LEnd   = XMFLOAT2(ceil(Atemp.x) - 1, (int)(Atemp.y + 1));
			distanceToNextTile =  min(distanceToNextTile, DistanceLineVsLine(Atemp, End, LStart, LEnd));
		}

		if (distanceToNextTile == INFINITE)
			break;

		A += distanceToNextTile * Direction;
		distance -= distanceToNextTile;
	}
	return true;
}
*/
bool Map::IsWalkable(int x, int y)
{
	if (x < 0 || y < 0  ||  x >= m_Width || y >= m_Height ) 
		return false;

	return m_Map[y][x];
}

bool Map::IsWalkable(float x, float y)
{
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
	Init(1, 1, 1);
}

PathMap::~PathMap(void)
{
}

void PathMap::Init(int Width, int Height, int numMaps)
{
	m_NumMaps = numMaps;
	m_Maps.clear();
	for (int i = 0; i < m_NumMaps; ++i)
	{
		float factor = pow(0.5f, i);
		Map map;
		map.Init(ceil((float)Width * factor), ceil((float)Height * factor));
		m_Maps.push_back(map);
	}
}

void PathMap::BlockPath(BoundingOrientedBox box)
{
	for (int i = 0; i < m_NumMaps; ++i)
	{
		m_Maps[i].BlockPath(box);
	}
}

bool PathMap::FindPath(XMFLOAT2 A, XMFLOAT2 B, vector<XMFLOAT2>& path)
{
	path.clear();
	for (int i = m_NumMaps - 1; i >= 0; --i)
	{
		if (m_Maps[i].FindPath(A, B, path))
		{
			path.pop_back();
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
	
	//return true;
	/*
	for (int i = m_NumMaps - 1; i >= 0; --i)
	{
		if (m_Maps[i].IsShortestPathFree(A, B))
		{
			return true;
		}
	}
	return false;*/

	if (m_Maps[m_NumMaps - 1].IsShortestPathFree(A, B))
		return true;

	if (m_NumMaps > 1)
		return m_Maps[0].IsShortestPathFree(A, B);
	
	return false;
	
	//return m_Maps[0].IsShortestPathFree(A, B);
}	

void PathMap::SetWalkable(bool walkable, float x, float y)
{
	for (int i = 0; i < m_NumMaps; ++i)
	{
		m_Maps[i].SetWalkable(walkable, x, y);
	}
}

bool PathMap::IsWalkable(float x, float y)
{
	return m_Maps[0].IsWalkable(x, y);
}


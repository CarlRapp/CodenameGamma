#pragma once
#ifndef QUADTREE_H
#define QUADTREE_H

#include "..\..\..\stdafx.h"
#include "Model.h"
#include "..\GameObject.h"

using namespace std;
using namespace DirectX;

class QuadTree
{
public:

	struct Node
	{
		vector<GameObject*> m_Content;

		Node			*m_Parent,	
						*m_NW, *m_NE, 
						*m_SW, *m_SE;

		BoundingBox m_BoundingBox;

		Node(Node *parent, BoundingBox boundingBox, int maxNodeSize)
		{
			m_Parent		= parent;
			m_BoundingBox	= boundingBox;

			m_Content		= vector<GameObject*>();
			m_Content.reserve(maxNodeSize);
			//m_Content.resize(maxNodeSize);

			m_NW			= NULL;
			m_NE			= NULL;
			m_SW			= NULL;
			m_SE			= NULL;
		}

		bool HasChildren()
		{
			return ( m_NW != NULL);
		}
	};

	void DestroyTree(Node* Instance);

private:
	int m_MaxNodeSize;
	float m_MinNodeArea;
	Node *m_RootNode;
	BoundingBox m_WorldBounds;

	void Insert(GameObject* instance, BoundingSphere& instanceSphere, Node* node, bool UpdateOld);
	void Delete(GameObject* instance, BoundingSphere& instanceSphere, Node* node);

	void DeleteChildren(Node* node);

	void CreateChildNodes(Node* node);

	void Clean(Node* node);

	int	 Size(Node* node);

	bool Empty(Node* node);

	bool NeedUpdate(GameObject* instance);

	void GetIntersectingInstances(BoundingFrustum frustum, vector<GameObject*> &instances, Node* node, ContainmentType containmentType);
	void GetIntersectingInstances(BoundingOrientedBox OBB, vector<GameObject*> &instances, Node* node, ContainmentType containmentType);

	void GetInstances(vector<GameObject*> &instances, Node* node);

	void GetObjectsCollidingWith(GameObject* go, BoundingSphere& instanceSphere, vector<GameObject*> &GameObjects, vector<vector<CollisionData>> &collisionData, Node* node, ContainmentType containmentType);

	BoundingSphere GetCurrentBoundingSphere(GameObject* go)
	{
		return go->GetQuadTreeType()->GetQuadTreeData().Current;
	}

	BoundingSphere GetOldBoundingSphere(GameObject* go)
	{
		return go->GetQuadTreeType()->GetQuadTreeData().Old;
	}

public:
	QuadTree(BoundingBox world, float minNodeArea, int maxNodeSize);
	~QuadTree(void);

	void Insert(GameObject* instance) 
	{ 
		Insert(instance, GetCurrentBoundingSphere(instance), m_RootNode, true); 
	}

	void Delete(GameObject* instance) 
	{ 
		if (instance) Delete(instance, GetOldBoundingSphere(instance), m_RootNode); 
	}

	void Update(GameObject* instance) 
	{ 
		if (instance)
			if (NeedUpdate(instance)) 
			{
				Delete(instance); 
				Insert(instance); 
			}
	}

	void GetIntersectingInstances(BoundingFrustum frustum, vector<GameObject*> &instances) 
	{ 
		GetIntersectingInstances(frustum, instances, m_RootNode, INTERSECTS); 
	}

	void GetIntersectingInstances(BoundingOrientedBox OBB, vector<GameObject*> &instances) 
	{ 
		GetIntersectingInstances(OBB, instances, m_RootNode, INTERSECTS); 
	}

	void GetObjectsCollidingWith(GameObject* go, vector<GameObject*> &GameObjects, vector<vector<CollisionData>> &collisionData)
	{
		return GetObjectsCollidingWith(go, GetCurrentBoundingSphere(go), GameObjects, collisionData, m_RootNode, INTERSECTS); 
	}
};

#endif
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

		Node(Node *parent, BoundingBox boundingBox)
		{
			m_Parent		= parent;
			m_BoundingBox	= boundingBox;

			m_Content		= vector<GameObject*>();

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
	static const int maxNodeSize = 4;
	float minNodeVolume;
	Node *m_RootNode;
	BoundingBox m_WorldBounds;

	void Insert(GameObject* instance, BoundingSphere& boundingSphere, Node* node, ContainmentType containmentType);
	void Delete(GameObject* instance, Node* node);

	void CreateChildNodes(Node* node);
	void InsertToChildren(GameObject* instance, BoundingSphere& boundingSphere, Node* node, ContainmentType containmentType);

	void Clean(Node* node);

	int	 Size(Node* node);

	bool Empty(Node* node);

	bool NeedUpdate(GameObject* instance);

	void GetIntersectingInstances(BoundingFrustum frusum, vector<GameObject*> &instances, Node* node, ContainmentType containmentType);
	void GetInstances(vector<GameObject*> &instances, Node* node);

	void GetObjectsCollidingWith(GameObject* go, vector<GameObject*> &GameObjects, Node* node, ContainmentType containmentType);

	BoundingSphere GetCurrentBoundingSphere(GameObject* go)
	{
		return go->GetQuadTreeType()->GetQuadTreeData().Current;
	}

	BoundingSphere GetOldBoundingSphere(GameObject* go)
	{
		return go->GetQuadTreeType()->GetQuadTreeData().Old;
	}

public:
	QuadTree(BoundingBox world, float minNodeVolume);
	~QuadTree(void);

	void Insert(GameObject* instance) 
	{ 
		Insert(instance, instance->GetQuadTreeType()->GetQuadTreeData().Current, m_RootNode, ContainmentType::INTERSECTS); 
	}

	void Delete(GameObject* instance) 
	{ 
		if (instance) Delete(instance, m_RootNode); 
	}

	void Update(GameObject* instance) 
	{ 
		if (instance)
			if (NeedUpdate(instance)) 
			{
				Delete(instance); 
				Insert(instance); 
				instance->GetQuadTreeType()->SetOldSphere(instance->GetQuadTreeType()->GetQuadTreeData().Current);
			}
	}

	void GetIntersectingInstances(BoundingFrustum frusum, vector<GameObject*> &instances) 
	{ 
		GetIntersectingInstances(frusum, instances, m_RootNode, ContainmentType::INTERSECTS); 
	}

	void GetObjectsCollidingWith(GameObject* go, vector<GameObject*> &GameObjects)
	{
		return GetObjectsCollidingWith(go, GameObjects, m_RootNode, ContainmentType::INTERSECTS); 
	}
};

#endif
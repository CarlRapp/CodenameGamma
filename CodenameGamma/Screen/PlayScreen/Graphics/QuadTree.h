#pragma once
#ifndef QUADTREE_H
#define QUADTREE_H

#include "..\..\..\stdafx.h"
#include "Model.h"

using namespace std;
using namespace DirectX;

class QuadTree
{
public:

	struct Node
	{
		vector<ModelInstance*> m_Content;

		Node			*m_Parent,	
						*m_NW, *m_NE, 
						*m_SW, *m_SE;

		BoundingBox m_BoundingBox;

		Node(Node *parent, BoundingBox boundingBox)
		{
			m_Parent		= parent;
			m_BoundingBox	= boundingBox;

			m_Content		= vector<ModelInstance*>();

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

private:
	static const int maxNodeSize = 4;
	float minNodeVolume;
	Node *m_RootNode;
	BoundingBox m_WorldBounds;

	void Insert(ModelInstance* instance, BoundingSphere& boundingSphere, Node* node, ContainmentType containmentType);
	void Delete(ModelInstance* instance, Node* node);

	void CreateChildNodes(Node* node);
	void InsertToChildren(ModelInstance* instance, BoundingSphere& boundingSphere, Node* node, ContainmentType containmentType);

	void Clean(Node* node);

	int	 Size(Node* node);

	bool Empty(Node* node);

	bool NeedUpdate(ModelInstance* instance);

	void GetIntersectingInstances(BoundingFrustum frusum, vector<ModelInstance*> &instances, Node* node, ContainmentType containmentType);
	void GetInstances(vector<ModelInstance*> &instances, Node* node);

public:
	QuadTree(BoundingBox world, float minNodeVolume);
	~QuadTree(void);

	void Insert(ModelInstance* instance) { 
		Insert(instance, instance->GetBoundingSphere(), m_RootNode, ContainmentType::INTERSECTS); 
	}
	void Delete(ModelInstance* instance) { if (instance) Delete(instance, m_RootNode); }
	void Update(ModelInstance* instance) 
	{ 
		if (instance)
			if (NeedUpdate(instance)) 
			{
				Delete(instance); 
				Insert(instance); 
				instance->m_OldBoundingSphere = instance->GetBoundingSphere();
			}
	}

	void GetIntersectingInstances(BoundingFrustum frusum, vector<ModelInstance*> &instances) 
	{ 
		GetIntersectingInstances(frusum, instances, m_RootNode, ContainmentType::INTERSECTS); 
	}
};

#endif
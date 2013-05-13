#include "QuadTree.h"


QuadTree::QuadTree(BoundingBox world, float minNodeArea, int maxNodeSize)
{
	m_WorldBounds	= world;
	m_RootNode		= new Node(NULL, world, maxNodeSize);
	m_MinNodeArea	= minNodeArea;
	m_MaxNodeSize	= maxNodeSize;
}


QuadTree::~QuadTree(void)
{
	//m_WorldBounds	= BoundingBox(XMFLOAT3(2000,0,2000), XMFLOAT3(2000, 2000, 2000));
	//m_RootNode		= new Node(NULL, m_WorldBounds);
	//minNodeVolume	= 1000;

	DestroyTree( m_RootNode );
}

void QuadTree::DestroyTree(Node* Instance)
{
	if ( Instance->m_NE )
		DestroyTree( Instance->m_NE );

	if ( Instance->m_NW )
		DestroyTree( Instance->m_NW );

	if ( Instance->m_SE )
		DestroyTree( Instance->m_SE );

	if ( Instance->m_SW )
		DestroyTree( Instance->m_SW );

	delete Instance;
}

void QuadTree::GetIntersectingInstances(BoundingFrustum frustum, vector<GameObject*> &instances, Node* node, ContainmentType containmentType)
{	
	if (containmentType == DISJOINT)
		return;

	switch (containmentType)
	{
	case CONTAINS:
		
		//Lägger till instancerna i listan.
		for each (GameObject *instance in node->m_Content)			
			instances.push_back(instance);		
		
		if (node->m_NW != NULL)
		{
			GetIntersectingInstances(frustum, instances, node->m_NW, CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_NE, CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_SW, CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_SE, CONTAINS);
		}
		break;
	case INTERSECTS:
		
		for each (GameObject *instance in node->m_Content)
		{
			//Om instansen kolliderar med furstum så lägger vi till den i listan.
			if (frustum.Intersects(GetCurrentBoundingSphere(instance)))
				instances.push_back(instance);
		}		
		
		if (node->m_NW != NULL)
		{
			BoundingBox nodeBox(XMFLOAT3(0,0,0), node->m_NW->m_BoundingBox.Extents);
			nodeBox.Extents.y = 0;

			nodeBox.Center = node->m_NW->m_BoundingBox.Center;
			GetIntersectingInstances(frustum, instances, node->m_NW, frustum.Contains(nodeBox));

			nodeBox.Center = node->m_NE->m_BoundingBox.Center;
			GetIntersectingInstances(frustum, instances, node->m_NE, frustum.Contains(nodeBox));

			nodeBox.Center = node->m_SW->m_BoundingBox.Center;
			GetIntersectingInstances(frustum, instances, node->m_SW, frustum.Contains(nodeBox));

			nodeBox.Center = node->m_SE->m_BoundingBox.Center;
			GetIntersectingInstances(frustum, instances, node->m_SE, frustum.Contains(nodeBox));
		}
			
		break;
	}
}

void QuadTree::GetIntersectingInstances(BoundingOrientedBox OBB, vector<GameObject*> &instances, Node* node, ContainmentType containmentType)
{	
	if (containmentType == DISJOINT)
		return;

	switch (containmentType)
	{
	case CONTAINS:
		
		//Lägger till instancerna i listan.
		for each (GameObject *instance in node->m_Content)			
			instances.push_back(instance);			
		
		if (node->m_NW != NULL)
		{
			GetIntersectingInstances(OBB, instances, node->m_NW, CONTAINS);
			GetIntersectingInstances(OBB, instances, node->m_NE, CONTAINS);
			GetIntersectingInstances(OBB, instances, node->m_SW, CONTAINS);
			GetIntersectingInstances(OBB, instances, node->m_SE, CONTAINS);
		}
		break;
	case INTERSECTS:
		
		for each (GameObject *instance in node->m_Content)
		{
			//Om instansen kolliderar med furstum så lägger vi till den i listan.
			if (OBB.Intersects(GetCurrentBoundingSphere(instance)))
				instances.push_back(instance);
		}		
		
		if (node->m_NW != NULL)
		{
			BoundingBox nodeBox(XMFLOAT3(0,0,0), node->m_NW->m_BoundingBox.Extents);
			nodeBox.Extents.y = 0;

			nodeBox.Center = node->m_NW->m_BoundingBox.Center;
			GetIntersectingInstances(OBB, instances, node->m_NW, OBB.Contains(nodeBox));

			nodeBox.Center = node->m_NE->m_BoundingBox.Center;
			GetIntersectingInstances(OBB, instances, node->m_NE, OBB.Contains(nodeBox));

			nodeBox.Center = node->m_SW->m_BoundingBox.Center;
			GetIntersectingInstances(OBB, instances, node->m_SW, OBB.Contains(nodeBox));

			nodeBox.Center = node->m_SE->m_BoundingBox.Center;
			GetIntersectingInstances(OBB, instances, node->m_SE, OBB.Contains(nodeBox));
		}
			
		break;
	}
}

void QuadTree::Insert(GameObject* instance, BoundingSphere& instanceSphere, Node* node, bool UpdateOld)
{
	if (node->m_NW != NULL)
	{
		if		(node->m_NW->m_BoundingBox.Contains(instanceSphere) == CONTAINS) Insert(instance, instanceSphere, node->m_NW, UpdateOld);
		else if (node->m_NE->m_BoundingBox.Contains(instanceSphere) == CONTAINS) Insert(instance, instanceSphere, node->m_NE, UpdateOld);
		else if (node->m_SW->m_BoundingBox.Contains(instanceSphere) == CONTAINS) Insert(instance, instanceSphere, node->m_SW, UpdateOld);
		else if (node->m_SE->m_BoundingBox.Contains(instanceSphere) == CONTAINS) Insert(instance, instanceSphere, node->m_SE, UpdateOld);
		else
		{
			node->m_Content.push_back(instance);
			instance->GetQuadTreeType()->SetNode(node);
			if (UpdateOld)
				instance->GetQuadTreeType()->SetOldSphere(GetCurrentBoundingSphere(instance));
		}
	}
	else
	{
		node->m_Content.push_back(instance);
		instance->GetQuadTreeType()->SetNode(node);
		if (UpdateOld)
			instance->GetQuadTreeType()->SetOldSphere(GetCurrentBoundingSphere(instance));

		float area = (node->m_BoundingBox.Extents.x * node->m_BoundingBox.Extents.z) / 4;
		if (node->m_Content.size() > m_MaxNodeSize && area >= m_MinNodeArea)
		{
			CreateChildNodes(node);

			vector<GameObject*> temp = node->m_Content;
			node->m_Content.clear();

			for each (GameObject* go in temp)
				Insert(go, GetOldBoundingSphere(go), node, false);
		}
	}
}

void QuadTree::Delete(GameObject* instance, BoundingSphere& instanceSphere, Node* node)
{
	if (node->m_NW != NULL)
	{
		if		(node->m_NW->m_BoundingBox.Contains(instanceSphere) == CONTAINS) Delete(instance, instanceSphere, node->m_NW);
		else if (node->m_NE->m_BoundingBox.Contains(instanceSphere) == CONTAINS) Delete(instance, instanceSphere, node->m_NE);
		else if (node->m_SW->m_BoundingBox.Contains(instanceSphere) == CONTAINS) Delete(instance, instanceSphere, node->m_SW);
		else if (node->m_SE->m_BoundingBox.Contains(instanceSphere) == CONTAINS) Delete(instance, instanceSphere, node->m_SE);
		else
		{
			//Delete from node
			node->m_Content.erase(remove(node->m_Content.begin(), node->m_Content.end(), instance), node->m_Content.end());

			
			//int size = node->m_Content.size() + node->m_NW->m_Content.size() + node->m_NE->m_Content.size() + node->m_SW->m_Content.size() + node->m_SE->m_Content.size();
			int size = Size(node);
			if (size <= m_MaxNodeSize)
			{
				/*
				node->m_Content.insert(node->m_Content.end(), node->m_NW->m_Content.begin(), node->m_NW->m_Content.end());
				node->m_Content.insert(node->m_Content.end(), node->m_NE->m_Content.begin(), node->m_NE->m_Content.end());
				node->m_Content.insert(node->m_Content.end(), node->m_SW->m_Content.begin(), node->m_SW->m_Content.end());
				node->m_Content.insert(node->m_Content.end(), node->m_SE->m_Content.begin(), node->m_SE->m_Content.end());
				*/

				GetInstances(node->m_Content, node->m_NW);
				GetInstances(node->m_Content, node->m_NE);
				GetInstances(node->m_Content, node->m_SW);
				GetInstances(node->m_Content, node->m_SE);

				for each (GameObject* go in node->m_Content)
				{
					go->GetQuadTreeType()->SetNode(node);
				}

				DeleteChildren(node);

				/*
				delete node->m_NW;
				delete node->m_NE;
				delete node->m_SW;
				delete node->m_SE;		

				node->m_NW = NULL;
				node->m_NE = NULL;
				node->m_SW = NULL;
				node->m_SE = NULL;
				*/
			}
		}
	}
	else
	{
		//Delete from node
		node->m_Content.erase(remove(node->m_Content.begin(), node->m_Content.end(), instance), node->m_Content.end());
	}
}

void QuadTree::DeleteChildren(Node* node)
{
	if (!node)
		return;
	
	DeleteChildren(node->m_NW);
	DeleteChildren(node->m_NE);
	DeleteChildren(node->m_SW);
	DeleteChildren(node->m_SE);

	delete node->m_NW;
	delete node->m_NE;
	delete node->m_SW;
	delete node->m_SE;		

	node->m_NW = NULL;
	node->m_NE = NULL;
	node->m_SW = NULL;
	node->m_SE = NULL;
}

void QuadTree::CreateChildNodes(Node* node)
{
	XMVECTOR center			= XMLoadFloat3(&node->m_BoundingBox.Center);

	XMFLOAT3 temp = node->m_BoundingBox.Extents;
	temp.x *= 0.5f;
	temp.z *= 0.5f;

	XMVECTOR newExtentsV	= XMLoadFloat3(&temp);
	//newExtentsV /= 2;

	XMVECTOR left			= XMLoadFloat3(&XMFLOAT3(-node->m_BoundingBox.Extents.x, 0, 0));
	XMVECTOR back			= XMLoadFloat3(&XMFLOAT3(0, 0, -node->m_BoundingBox.Extents.z));

	XMVECTOR NECenterV		= center + newExtentsV;
	NECenterV				= XMVectorSetY(NECenterV, node->m_BoundingBox.Center.y);

	XMVECTOR NWCenterV		= NECenterV + left;
	XMVECTOR SECenterV		= NECenterV + back;
	XMVECTOR SWCenterV		= SECenterV + left;

	XMFLOAT3 NWCenter, NECenter, SWCenter, SECenter, newExtents;

	XMStoreFloat3(&NWCenter, NWCenterV);
	XMStoreFloat3(&NECenter, NECenterV);
	XMStoreFloat3(&SWCenter, SWCenterV);
	XMStoreFloat3(&SECenter, SECenterV);

	XMStoreFloat3(&newExtents, newExtentsV);

	BoundingBox NW = BoundingBox(NWCenter, newExtents);
	BoundingBox NE = BoundingBox(NECenter, newExtents);
	BoundingBox SW = BoundingBox(SWCenter, newExtents);
	BoundingBox SE = BoundingBox(SECenter, newExtents);

	node->m_NW = new Node(node, NW, m_MaxNodeSize);
	node->m_NE = new Node(node, NE, m_MaxNodeSize);
	node->m_SW = new Node(node, SW, m_MaxNodeSize);
	node->m_SE = new Node(node, SE, m_MaxNodeSize);
}

void QuadTree::GetInstances(vector<GameObject*> &instances, Node* node)
{
	if (!node)
		return;
	for each (GameObject *instance in node->m_Content)
		instances.push_back(instance);

	GetInstances(instances, node->m_NW);
	GetInstances(instances, node->m_NE);
	GetInstances(instances, node->m_SW);
	GetInstances(instances, node->m_SE);
}

void QuadTree::Clean(Node* node)
{
	if (!node)
		return;

	int size = Size(node);

	if (size == 0)
	{
		Clean(node->m_Parent);
	}

	else if (size < m_MaxNodeSize && node->m_Content.size() != size)
	{
		vector<GameObject*> instances = vector<GameObject*>();
		GetInstances(instances, node);
		sort( instances.begin(), instances.end() );
		instances.erase( unique( instances.begin(), instances.end() ), instances.end() );

		node->m_Content = instances;

		delete node->m_NW;
		delete node->m_NE;
		delete node->m_SW;
		delete node->m_SE;


		node->m_NW = NULL;
		node->m_NE = NULL;
		node->m_SW = NULL;
		node->m_SE = NULL;
		Clean(node->m_Parent);
	}
}

int	QuadTree::Size(Node* node)
{
	if (!node)
		return 0;

	return node->m_Content.size() + Size(node->m_NW) + Size(node->m_NE) + Size(node->m_SW) + Size(node->m_SE);
}

bool QuadTree::Empty(Node* node)
{
	if (!node)
		return true;
	/*
	if (!node->HasChildren() && node->m_Content.empty())
		return true;

	return false;
	*/
	if (!Empty(node->m_NW))
		return false;
	if (!Empty(node->m_NE))
		return false;
	if (!Empty(node->m_SW))
		return false;
	if (!Empty(node->m_SE))
		return false;
	if (!node->m_Content.empty())
		return false;
	
	return true;
	
}

bool QuadTree::NeedUpdate(GameObject* instance)
{		
	Node* node = (Node*)instance->GetQuadTreeType()->GetNode();
	//if (node->m_BoundingBox.Contains(GetOldBoundingSphere(instance)) == CONTAINS)
		if (node->m_BoundingBox.Contains(GetCurrentBoundingSphere(instance)) == CONTAINS)
			return false;
	return true;
}

void QuadTree::GetObjectsCollidingWith(GameObject* go, BoundingSphere& instanceSphere, vector<GameObject*> &GameObjects, vector<vector<CollisionData>> &collisionData, Node* node, ContainmentType containmentType)
{
	if (containmentType == DISJOINT)
		return;

	for each (GameObject* go2 in node->m_Content)
	{
		vector<CollisionData> CD;
		if (go->Intersects(go, go2, CD))
		{
			GameObjects.push_back(go2);
			collisionData.push_back(CD);
		}
	}
	
	if (node->m_NW != NULL)
	{
	switch (containmentType)
	{
		case CONTAINS:
			GetObjectsCollidingWith(go, instanceSphere, GameObjects, collisionData, node->m_NW, CONTAINS);
			GetObjectsCollidingWith(go, instanceSphere, GameObjects, collisionData, node->m_NE, CONTAINS);
			GetObjectsCollidingWith(go, instanceSphere, GameObjects, collisionData, node->m_SW, CONTAINS);
			GetObjectsCollidingWith(go, instanceSphere, GameObjects, collisionData, node->m_SE, CONTAINS);
			break;
		case INTERSECTS:	

			BoundingBox nodeBox(XMFLOAT3(0,0,0), node->m_NW->m_BoundingBox.Extents);
			nodeBox.Extents.y = 0;

			nodeBox.Center = node->m_NW->m_BoundingBox.Center;
			GetObjectsCollidingWith(go, instanceSphere, GameObjects, collisionData, node->m_NW, instanceSphere.Contains(nodeBox));

			nodeBox.Center = node->m_NE->m_BoundingBox.Center;
			GetObjectsCollidingWith(go, instanceSphere, GameObjects, collisionData, node->m_NE, instanceSphere.Contains(nodeBox));

			nodeBox.Center = node->m_SW->m_BoundingBox.Center;
			GetObjectsCollidingWith(go, instanceSphere, GameObjects, collisionData, node->m_SW, instanceSphere.Contains(nodeBox));

			nodeBox.Center = node->m_SE->m_BoundingBox.Center;
			GetObjectsCollidingWith(go, instanceSphere, GameObjects, collisionData, node->m_SE, instanceSphere.Contains(nodeBox));

			break;
		
		}
	}
}

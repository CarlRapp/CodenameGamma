#include "QuadTree.h"


QuadTree::QuadTree(BoundingBox world, float minNodeArea)
{
	m_WorldBounds	= world;
	m_RootNode		= new Node(NULL, world);
	QuadTree::minNodeArea = minNodeArea;
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

	switch (containmentType)
	{
	case DISJOINT:
		return;
	case CONTAINS:
		if (node->m_NW == NULL)
		{
			//Lägger till instancerna i listan.
			for each (GameObject *instance in node->m_Content)			
				instances.push_back(instance);			
		}
		else
		{
			GetIntersectingInstances(frustum, instances, node->m_NW, CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_NE, CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_SW, CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_SE, CONTAINS);
		}
		break;
	case INTERSECTS:
		if (node->m_NW == NULL)
		{
			for each (GameObject *instance in node->m_Content)
			{
				//Om instansen kolliderar med furstum så lägger vi till den i listan.
				if (frustum.Intersects(GetCurrentBoundingSphere(instance)))
					instances.push_back(instance);
			}		
		}
		else
		{
			GetIntersectingInstances(frustum, instances, node->m_NW, frustum.Contains(node->m_NW->m_BoundingBox));
			GetIntersectingInstances(frustum, instances, node->m_NE, frustum.Contains(node->m_NE->m_BoundingBox));
			GetIntersectingInstances(frustum, instances, node->m_SW, frustum.Contains(node->m_SW->m_BoundingBox));
			GetIntersectingInstances(frustum, instances, node->m_SE, frustum.Contains(node->m_SE->m_BoundingBox));
		}
			
		break;
	}
}

void QuadTree::GetIntersectingInstances(BoundingOrientedBox OBB, vector<GameObject*> &instances, Node* node, ContainmentType containmentType)
{	

	switch (containmentType)
	{
	case DISJOINT:
		return;
	case CONTAINS:
		if (node->m_NW == NULL)
		{
			//Lägger till instancerna i listan.
			for each (GameObject *instance in node->m_Content)			
				instances.push_back(instance);			
		}
		else
		{
			GetIntersectingInstances(OBB, instances, node->m_NW, CONTAINS);
			GetIntersectingInstances(OBB, instances, node->m_NE, CONTAINS);
			GetIntersectingInstances(OBB, instances, node->m_SW, CONTAINS);
			GetIntersectingInstances(OBB, instances, node->m_SE, CONTAINS);
		}
		break;
	case INTERSECTS:
		if (node->m_NW == NULL)
		{
			for each (GameObject *instance in node->m_Content)
			{
				//Om instansen kolliderar med furstum så lägger vi till den i listan.
				if (OBB.Intersects(GetCurrentBoundingSphere(instance)))
					instances.push_back(instance);
			}		
		}
		else
		{
			GetIntersectingInstances(OBB, instances, node->m_NW, OBB.Contains(node->m_NW->m_BoundingBox));
			GetIntersectingInstances(OBB, instances, node->m_NE, OBB.Contains(node->m_NE->m_BoundingBox));
			GetIntersectingInstances(OBB, instances, node->m_SW, OBB.Contains(node->m_SW->m_BoundingBox));
			GetIntersectingInstances(OBB, instances, node->m_SE, OBB.Contains(node->m_SE->m_BoundingBox));
		}
			
		break;
	}
}


void QuadTree::Insert(GameObject* instance, BoundingSphere& boundingSphere, Node* node, ContainmentType containmentType)
{
	if (containmentType == DISJOINT)
		return;

	//Sätter in instansen i noden om den är ett löv.
	if (node->m_NW == NULL)
	{
		node->m_Content.push_back(instance);
		instance->GetQuadTreeType()->SetNode(node);
		//instance->GetQuadTreeType()->GetQuadTreeData().Node = node;

		//För många instanser i noden. Skapar barn till noden som vi skickar ner instanserna till.
		float area = (node->m_BoundingBox.Extents.x * node->m_BoundingBox.Extents.z) / 4;
		if (node->m_Content.size() > maxNodeSize && area >= minNodeArea)
		{
			CreateChildNodes(node);

			while (!node->m_Content.empty())
			{
				GameObject* temp = node->m_Content.back();
				InsertToChildren(temp, GetCurrentBoundingSphere(temp), node, containmentType);
				node->m_Content.pop_back();
			}			
		}
		
	}
	//Sätter in instansen i barnen
	else
	{
		InsertToChildren(instance, boundingSphere, node, containmentType);
	}
}

void QuadTree::Delete(GameObject* instance, Node* node)
{
	if (!node->m_BoundingBox.Intersects(GetOldBoundingSphere(instance)))
		return;

	int size = 0;
	//Tar bort instansen från barnen om den här noden inte har något content.
	if (node->HasChildren())
	{
		Delete(instance, node->m_NW);
		Delete(instance, node->m_NE);
		Delete(instance, node->m_SW);
		Delete(instance, node->m_SE);

		vector<GameObject*> instances = vector<GameObject*>();
		GetInstances(instances, node);
		sort( instances.begin(), instances.end() );
		instances.erase( unique( instances.begin(), instances.end() ), instances.end() );


		if (instances.size() < maxNodeSize)
		{
			node->m_Content = instances;

			for each (GameObject* it in instances)
				it->GetQuadTreeType()->SetNode(node);
				//it->GetQuadTreeType()->GetQuadTreeData().Node = node;
		
			delete node->m_NW;
			delete node->m_NE;
			delete node->m_SW;
			delete node->m_SE;		

			node->m_NW = NULL;
			node->m_NE = NULL;
			node->m_SW = NULL;
			node->m_SE = NULL;
		}
	}
	//Tar bort instansen från noden.
	else
	{
		node->m_Content.erase(remove(node->m_Content.begin(), node->m_Content.end(), instance), node->m_Content.end());
	}
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

	node->m_NW = new Node(node, NW);
	node->m_NE = new Node(node, NE);
	node->m_SW = new Node(node, SW);
	node->m_SE = new Node(node, SE);
}

void QuadTree::InsertToChildren(GameObject* instance, BoundingSphere& boundingSphere, Node* node, ContainmentType containmentType)
{
	if (containmentType == CONTAINS)
	{
		Insert(instance, boundingSphere, node->m_NW, CONTAINS);
		Insert(instance, boundingSphere, node->m_NE, CONTAINS);	
		Insert(instance, boundingSphere, node->m_SE, CONTAINS);	
		Insert(instance, boundingSphere, node->m_SW, CONTAINS);	
	}

	else if (containmentType == INTERSECTS)
	{
		Insert(instance, boundingSphere, node->m_NW, boundingSphere.Contains(node->m_NW->m_BoundingBox));
		Insert(instance, boundingSphere, node->m_NE, boundingSphere.Contains(node->m_NE->m_BoundingBox));
		Insert(instance, boundingSphere, node->m_SE, boundingSphere.Contains(node->m_SE->m_BoundingBox));
		Insert(instance, boundingSphere, node->m_SW, boundingSphere.Contains(node->m_SW->m_BoundingBox));
	}
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

	else if (size < maxNodeSize && node->m_Content.size() != size)
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
	if (node->m_BoundingBox.Contains(GetOldBoundingSphere(instance)) == CONTAINS)
		if (node->m_BoundingBox.Contains(GetCurrentBoundingSphere(instance)) == CONTAINS)
			return false;
	return true;

}

void QuadTree::GetObjectsCollidingWith(GameObject* go, vector<GameObject*> &GameObjects, Node* node, ContainmentType containmentType)
{
	switch (containmentType)
	{
	case DISJOINT:
		return;
		break;
	case INTERSECTS:
		if (node->m_NW == NULL)
		{
			BoundingSphere goSphere = GetCurrentBoundingSphere(go);
			for each (GameObject* go2 in node->m_Content)
			{
				BoundingSphere go2Sphere = GetCurrentBoundingSphere(go);

				if (goSphere.Intersects(go2Sphere))
					GameObjects.push_back(go2);
			}
		}

		else
		{
			BoundingSphere goSphere = GetCurrentBoundingSphere(go);
			GetObjectsCollidingWith(go, GameObjects, node->m_NW, goSphere.Contains(node->m_NW->m_BoundingBox));
			GetObjectsCollidingWith(go, GameObjects, node->m_NE, goSphere.Contains(node->m_NE->m_BoundingBox));
			GetObjectsCollidingWith(go, GameObjects, node->m_SW, goSphere.Contains(node->m_SW->m_BoundingBox));
			GetObjectsCollidingWith(go, GameObjects, node->m_SE, goSphere.Contains(node->m_SE->m_BoundingBox));
		}

		break;
	case CONTAINS:
		GetInstances(GameObjects, node);
		break;
	}

	if (containmentType == DISJOINT)
		return;
}

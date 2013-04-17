#include "QuadTree.h"


QuadTree::QuadTree(BoundingBox world, float minNodeVolume)
{
	m_WorldBounds	= world;
	m_RootNode		= new Node(NULL, world);
	QuadTree::minNodeVolume = minNodeVolume;
}


QuadTree::~QuadTree(void)
{
	m_WorldBounds	= BoundingBox(XMFLOAT3(2000,0,2000), XMFLOAT3(2000, 2000, 2000));
	m_RootNode		= new Node(NULL, m_WorldBounds);
	minNodeVolume	= 1000;
}


void QuadTree::GetIntersectingInstances(BoundingFrustum frustum, vector<ModelInstance*> &instances, Node* node, ContainmentType containmentType)
{	

	switch (containmentType)
	{
	case ContainmentType::DISJOINT:
		return;
	case ContainmentType::CONTAINS:
		if (node->m_NW == NULL)
		{
			//L�gger till instancerna i listan.
			for each (ModelInstance *instance in node->m_Content)			
				instances.push_back(instance);			
		}
		else
		{
			GetIntersectingInstances(frustum, instances, node->m_NW, ContainmentType::CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_NE, ContainmentType::CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_SW, ContainmentType::CONTAINS);
			GetIntersectingInstances(frustum, instances, node->m_SE, ContainmentType::CONTAINS);
		}
		break;
	case ContainmentType::INTERSECTS:
		if (node->m_NW == NULL)
		{
			for each (ModelInstance *instance in node->m_Content)
			{
				//Om instansen kolliderar med furstum s� l�gger vi till den i listan.
				if (frustum.Intersects(instance->GetBoundingSphere()))
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


void QuadTree::Insert(ModelInstance* instance, BoundingSphere& boundingSphere, Node* node, ContainmentType containmentType)
{
	if (containmentType == ContainmentType::DISJOINT)
		return;

	//S�tter in instansen i noden om den �r ett l�v.
	if (node->m_NW == NULL)
	{
		node->m_Content.push_back(instance);
		instance->m_Node = node;

		//F�r m�nga instanser i noden. Skapar barn till noden som vi skickar ner instanserna till.
		float volume = 4 * node->m_BoundingBox.Extents.x * node->m_BoundingBox.Extents.y *node->m_BoundingBox.Extents.z;
		if (node->m_Content.size() > maxNodeSize && volume > minNodeVolume)
		{
			CreateChildNodes(node);

			while (!node->m_Content.empty())
			{
				ModelInstance* temp = node->m_Content.back();
				InsertToChildren(temp, temp->GetBoundingSphere(), node, containmentType);
				node->m_Content.pop_back();
			}			
		}
		
	}
	//S�tter in instansen i barnen
	else
	{
		InsertToChildren(instance, boundingSphere, node, containmentType);
	}
}

void QuadTree::Delete(ModelInstance* instance, Node* node)
{
	if (!node->m_BoundingBox.Intersects(instance->m_OldBoundingSphere))
		return;

	int size = 0;
	//Tar bort instansen fr�n barnen om den h�r noden inte har n�got content.
	if (node->HasChildren())
	{
		Delete(instance, node->m_NW);
		Delete(instance, node->m_NE);
		Delete(instance, node->m_SW);
		Delete(instance, node->m_SE);

		vector<ModelInstance*> instances = vector<ModelInstance*>();
		GetInstances(instances, node);
		sort( instances.begin(), instances.end() );
		instances.erase( unique( instances.begin(), instances.end() ), instances.end() );


		if (instances.size() < maxNodeSize)
		{
			node->m_Content = instances;

			for each (ModelInstance* it in instances)
				it->m_Node = node;
		
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
	//Tar bort instansen fr�n noden.
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

void QuadTree::InsertToChildren(ModelInstance* instance, BoundingSphere& boundingSphere, Node* node, ContainmentType containmentType)
{
	if (containmentType == ContainmentType::CONTAINS)
	{
		Insert(instance, boundingSphere, node->m_NW, ContainmentType::CONTAINS);
		Insert(instance, boundingSphere, node->m_NE, ContainmentType::CONTAINS);	
		Insert(instance, boundingSphere, node->m_SE, ContainmentType::CONTAINS);	
		Insert(instance, boundingSphere, node->m_SW, ContainmentType::CONTAINS);	
	}

	else if (containmentType == ContainmentType::INTERSECTS)
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
		vector<ModelInstance*> instances = vector<ModelInstance*>();
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

void QuadTree::GetInstances(vector<ModelInstance*> &instances, Node* node)
{
	if (!node)
		return;
	for each (ModelInstance *instance in node->m_Content)
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

bool QuadTree::NeedUpdate(ModelInstance* instance)
{		
	Node* node = (Node*)instance->m_Node;	
	if (node->m_BoundingBox.Contains(instance->m_OldBoundingSphere) == ContainmentType::CONTAINS)
		if (node->m_BoundingBox.Contains(instance->GetBoundingSphere()) == ContainmentType::CONTAINS)
			return false;
	return true;

}


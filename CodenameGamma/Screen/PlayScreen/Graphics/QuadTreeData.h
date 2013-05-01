#pragma once
#ifndef QUADTREEDATA_H
#define QUADTREEDATA_H

#include "Model.h"
#include "../../../stdafx.h"

struct QuadTreeData
{
	BoundingSphere Current;
	BoundingSphere Old;
	void* Node;
	
	QuadTreeData()
	{
		Node = NULL;
	}
};

class QuadTreeType
{	
protected:
	QuadTreeData m_QuadTreeData;
	
public:
	QuadTreeData GetQuadTreeData() { return m_QuadTreeData; }
	void  SetOldSphere(BoundingSphere sphere) { m_QuadTreeData.Old = sphere; }
	void  SetOldPoint(XMFLOAT3 point) { m_QuadTreeData.Old = BoundingSphere(point, 0); }
	void  SetNode(void* node) { m_QuadTreeData.Node = node; }
	void* GetNode() { return m_QuadTreeData.Node; }

	virtual void Update() = 0; 
};

class QuadTreeTypeModel : public QuadTreeType
{
	ModelInstance* m_ModelInstance;
public:
	QuadTreeTypeModel(ModelInstance* modelInstance);
	void Update() 
	{ 
		m_QuadTreeData.Current = m_ModelInstance->GetBoundingSphere(); 
	}
};

class QuadTreeTypePoint : public QuadTreeType
{
	XMFLOAT3 *m_Point;
public:
	QuadTreeTypePoint(XMFLOAT3 *point);
	void Update() { m_QuadTreeData.Current = BoundingSphere(*m_Point, 0); }
};

#endif
#include "QuadTreeData.h"

QuadTreeTypeModel::QuadTreeTypeModel(ModelInstance* modelInstance)
{
	m_ModelInstance = modelInstance;
	BoundingSphere sphere = modelInstance->GetBoundingSphere();
	m_QuadTreeData.Current = sphere;
}

QuadTreeTypePoint::QuadTreeTypePoint(XMFLOAT3 *point)
{
	m_Point = point;

	BoundingSphere sphere = BoundingSphere(*m_Point, 0);
	m_QuadTreeData.Current = sphere;
}
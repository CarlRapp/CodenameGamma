#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Graphics\Model.h"
#include "..\..\stdafx.h"
#include "Terrain\Terrain.h"
#include "Graphics\QuadTreeData.h"

using namespace DirectX;
class GameObject
{
	QuadTreeType *m_QuadTreeType;
	ModelInstance *m_ModelInstance;

public:
	XMFLOAT3 m_Position, m_Velocity;


	GameObject(void);
	~GameObject(void);

	bool Update(float deltaTime, Terrain* terrain);
	QuadTreeType* GetQuadTreeType() { return m_QuadTreeType; }
	
	void SetModelInstance(ModelInstance *modelInstance);	
	ModelInstance* GetModelInstance() { return m_ModelInstance; }
};

#endif
#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Graphics\Model.h"
#include "..\..\stdafx.h"
#include "Terrain\Terrain.h"

using namespace DirectX;
class GameObject
{


public:
	XMFLOAT3 m_Position, m_Velocity;
	ModelInstance *m_ModelInstance;

	GameObject(void);
	~GameObject(void);

	bool Update(float deltaTime, Terrain* terrain);
};

#endif
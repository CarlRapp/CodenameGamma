#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Graphics\Model.h"
#include "..\..\stdafx.h"
#include "Terrain\Terrain.h"
#include "Graphics\QuadTreeData.h"
#include "../../MathHelper.h"
using namespace DirectX;

enum GOFloat3Value
{
	Position, Velocity,
	Acceleration
};
enum GOFloat4x4Value
{
	Rotation, Translation,
	Scale, World,
	WorldInverseTranspose
};

class GameObject
{
	QuadTreeType *m_QuadTreeType;
	ModelInstance *m_ModelInstance;

	XMFLOAT3	gPosition;
	XMFLOAT3	gVelocity, gAcceleration;
	XMFLOAT3	gRotationInFloat;

	XMFLOAT4X4	gRotation, gTranslation, gScale, 
				gWorld, gWorldInverseTranspose;


	void	UpdateWorld(bool UpdateInverseTranspose);
public:
	GameObject(void);
	~GameObject(void);

	bool Update(float deltaTime, Terrain* terrain);
	QuadTreeType* GetQuadTreeType() { return m_QuadTreeType; }
	
	void SetModelInstance(ModelInstance *modelInstance);	
	ModelInstance* GetModelInstance() { return m_ModelInstance; }

	XMFLOAT3	GetFloat3Value(GOFloat3Value Value);
	XMFLOAT4X4	GetFloat4x4Value(GOFloat4x4Value Value);

	void	AddRotation(XMFLOAT3 Delta);
	void	SetRotation(XMFLOAT3 Rotation);
	void	LookAt(XMFLOAT3 Position);

	void	SetScale(XMFLOAT3 Scale);

	void	Move(XMFLOAT3 Delta);
	void	MoveTo(XMFLOAT3 Location);

	void	SetVelocity(XMFLOAT3 Velocity);

};

#endif
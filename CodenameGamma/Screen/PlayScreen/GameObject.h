#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Graphics\Model.h"
#include "..\..\stdafx.h"
#include "Terrain\Terrain.h"
#include "Graphics\QuadTreeData.h"
#include "../../MathHelper.h"
#include "Graphics\ModelManager.h"
#include <typeinfo>
#include <functional>
using namespace DirectX;

//typedef std::function<void(Light*)> LightCallback
#define LightCallback std::function<void(Light*)>

struct HitBox
{
	std::string				name;
	BoundingOrientedBox box;

	HitBox(void)
	{
	}

	HitBox(std::string n, BoundingOrientedBox b)
	{
		name = n;
		box  = b;
	}
};

struct CollisionData
{
	HitBox A;
	HitBox B;
	
	void Swap()
	{
		HitBox temp = A;
		A = B;
		B = temp;
	}
};

enum GOState
{
	Alive,
	Idle,
	Dead
};

enum GOTeam
{
	Team1, Team2,
	Team3, Team4,
	Team5, Neutral
};

enum GOFloat3Value
{
	Position, Velocity,
	Acceleration, Rotations,
	Direction
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

	GOState		gState;
	GOTeam		gTeam;
	XMFLOAT3	gPosition;
	XMFLOAT3	gVelocity, gAcceleration;
	XMFLOAT3	gRotationInFloat;
	float		gScaleInFloat;

	XMFLOAT4X4	gRotation, gTranslation, gScale, 
				gWorld, gWorldInverseTranspose;


	void	UpdateWorld(bool UpdateInverseTranspose);

protected:
	LightCallback AddLight;
	LightCallback RemoveLight;
	ModelInstance *m_ModelInstance;

public:
	GameObject(void);
	~GameObject(void);

	virtual bool Update(float deltaTime, Terrain* terrain);
	QuadTreeType* GetQuadTreeType() { return m_QuadTreeType; }
	
	void SetModelInstance(ModelInstance *modelInstance);	
	ModelInstance* GetModelInstance() { return m_ModelInstance; }

	float		GetRadius();
	float		GetScale() { return gScaleInFloat; }
	XMFLOAT3	GetFloat3Value(GOFloat3Value Value);
	XMFLOAT4X4	GetFloat4x4Value(GOFloat4x4Value Value);

	void	SetAddLight(LightCallback callback) { AddLight = callback; }
	void	SetRemoveLight(LightCallback callback) { RemoveLight = callback; }

	void	AddRotation(XMFLOAT3 Delta);
	void	SetRotation(XMFLOAT3 Rotation);
	void	LookAt(XMFLOAT3 Position);

	void	SetScale(float Scale);

	void	Move(XMFLOAT3 Delta);
	void	MoveTo(XMFLOAT3 Location);

	void	SetVelocity(XMFLOAT3 Velocity);

	void	SetState(GOState Value);

	bool	PlayAnimation(string name);

	bool	UsePose(string name);

	string  CurrentAnimationOrPose();
	
	static bool	Intersects(const GameObject* A, const GameObject* B, vector<CollisionData>& CD);

	bool	IsAlive(){ return (gState != Dead); }

	bool	IsEnemy(GameObject* Instance);
	virtual void	SetTeam(GOTeam Value);
	GOTeam	GetTeam();

	virtual	void	CollideWith(GameObject* Instance, vector<CollisionData> CD);
	/*
	template<class T>
	bool IsOfType(GameObject* Instance)
	{
		return (dynamic_cast<T*>(Instance) != 0);
	}
	*/
};

#endif
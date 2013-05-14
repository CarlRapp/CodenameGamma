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
#define GameObjectCallback std::function<void(GameObject*)>

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
	Hidden,	//	When an object wants to be removed from tree, but not from game
	Dead	//	Will remove the object from the game, and hence DELETE it
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


	void		UpdateWorld(bool UpdateInverseTranspose);

	bool		AnimateThisUpdate;

protected:
	LightCallback AddLight;
	LightCallback RemoveLight;
	GameObjectCallback AddGameObject;
	GameObjectCallback RemoveGameObject;
	GameObjectCallback DeleteGameObject;

	ModelInstance *m_ModelInstance;

public:
	GameObject(void);
	virtual ~GameObject(void);

	virtual void Update(float deltaTime, Terrain* terrain);
	QuadTreeType* GetQuadTreeType() { return m_QuadTreeType; }
	
	void SetModelInstance(ModelInstance *modelInstance);	
	ModelInstance* GetModelInstance() { return m_ModelInstance; }

	float		GetRadius();
	float		GetScale() { return gScaleInFloat; }
	XMFLOAT3	GetFloat3Value(GOFloat3Value Value);
	XMFLOAT4X4	GetFloat4x4Value(GOFloat4x4Value Value);

	void	SetAddLight(LightCallback callback) { AddLight = callback; }
	void	SetRemoveLight(LightCallback callback) { RemoveLight = callback; }

	void	SetAddGameObject(GameObjectCallback callback) { AddGameObject = callback; }
	void	SetRemoveGameObject(GameObjectCallback callback) { RemoveGameObject = callback; }
	void	SetDeleteGameObject(GameObjectCallback callback) { DeleteGameObject = callback; }

	void	AddRotation(XMFLOAT3 Delta);
	void	SetRotation(XMFLOAT3 Rotation);
	void	LookAt(XMFLOAT3 Position);

	void	SetScale(float Scale);

	void	Move(XMFLOAT3 Delta);
	void	MoveTo(XMFLOAT3 Location);

	void	SetVelocity(XMFLOAT3 Velocity);

	void	SetState(GOState Value);

	bool	PlayAnimation(string name);
	void	AnimateNextFrame() { AnimateThisUpdate = true; }

	bool	UsePose(string name);

	string  CurrentAnimationOrPose();
	
	//static bool	Intersects(const GameObject* A, const GameObject* B, vector<CollisionData>& CD);

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD) = 0;

	GOState	GetState()	{ return gState; }
	bool	IsAlive()	{ return (gState != Dead) && (gState != Hidden); }

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



	static bool SphereVsSphere(const GameObject* A, const GameObject* B);
	static bool BoxVsBox(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD);
	static bool BoneVsBone(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD);
	static bool AllBonesVsAllBones(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD);


	static bool SphereVsBox(const GameObject* A, const GameObject* B);
	static bool SphereVsBone(const GameObject* A, const GameObject* B);
	
	static bool BoxVsBone(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD);
	static bool BoxVsAllBones(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD);


	//static bool SphereVsAllBones(const GameObject* A, const GameObject* B);	
	//static bool BoneVsAllBones(const GameObject* A, const GameObject* B, vector<CollisionData>& CD);

};

#endif
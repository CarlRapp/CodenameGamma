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
	Dying,	//	When an object is dead but will not be removed from the game
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

	float		gDeathTimer;

protected:

	enum MoveDirection { Forward, Back, Right, Left, None };

	MoveDirection gMoveDirection;

	LightCallback AddLight;
	LightCallback RemoveLight;
	GameObjectCallback SetCallbackFunctions;
	GameObjectCallback AddGameObject;

	ModelInstance *m_ModelInstance;

	float		  gDeathTime;

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

	void		SetWorld(XMFLOAT4X4 translation, XMFLOAT4X4 scale, XMFLOAT4X4 rotation);

	vector<BoundingOrientedBox> GetBoundingBoxes()
	{
		vector<BoundingOrientedBox> boxes;
		if (m_ModelInstance)
		{
			m_ModelInstance->UpdateBoxes();			
			boxes = m_ModelInstance->m_BoneBoxes;
			
			if (boxes.empty())
				boxes.push_back(m_ModelInstance->GetBoundingOrientedBox());
		}
		else
		{
			BoundingBox AABB = BoundingBox(gPosition, XMFLOAT3(0,0,0));
			BoundingOrientedBox OBB;
			BoundingOrientedBox::CreateFromBoundingBox(OBB, AABB);
			boxes.push_back(OBB);
		}
		return boxes;
	}

	BoundingOrientedBox GetBoundingBox()
	{
		if (m_ModelInstance)
			return m_ModelInstance->GetBoundingOrientedBox();
		else
		{
			BoundingBox AABB = BoundingBox(gPosition, XMFLOAT3(0,0,0));
			BoundingOrientedBox OBB;
			BoundingOrientedBox::CreateFromBoundingBox(OBB, AABB);
			return OBB;
		}
	}

	void	SetAddLight(LightCallback callback) { AddLight = callback; }
	void	SetRemoveLight(LightCallback callback) { RemoveLight = callback; }

	void	SetSetCallbackFunctions(GameObjectCallback callback) { SetCallbackFunctions = callback; }
	void	SetAddGameObject(GameObjectCallback callback) { AddGameObject = callback; }

	void	AddRotation(XMFLOAT3 Delta);
	void	SetRotation(XMFLOAT3 Rotation);
	void	LookAt(XMFLOAT3 Position);

	void	SetScale(float Scale);

	void	Move(XMFLOAT3 Delta);
	void	MoveTo(XMFLOAT3 Location);

	virtual	void	SetVelocity(XMFLOAT3 Velocity);

	virtual void	SetState(GOState Value);

	bool	PlayingAnimation(string name);
	bool	LoopAnimation(string name);	
	bool	PlayAnimation(string name);
	bool	LoopAnimationAfter(string current, string next);
	bool	PlayAnimationAfter(string current, string next);
	void	StopAnimation(string name);
	void	StopAllAnimations();

	bool	GetJointPosition(string name, XMFLOAT3& pos);

	bool	SetAnimationSpeed(string name, float speed);
	bool	SetAnimationProgress(string name, float progress);

	float	GetAnimationSpeed(string name);
	float	GetAnimationProgress(string name);

	float	GetAnimationTime(string name);

	void	AnimateNextFrame() { AnimateThisUpdate = true; }

	void	SetTextureIndex(int index) { if (m_ModelInstance) m_ModelInstance->SetTextureIndex(index); }

	void	Kill();

	bool	GotCallbackFunctions() { return (AddLight && RemoveLight && SetCallbackFunctions && AddGameObject); }
	//bool	UsePose(string name);

	//string  CurrentAnimationOrPose();
	
	//static bool	Intersects(const GameObject* A, const GameObject* B, vector<CollisionData>& CD);

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD) = 0;

	GOState	GetState()	{ return gState; }
	bool	IsAlive()	{ return (gState == Alive) || (gState == Idle); }
	bool	WantsRemove() { return (gState == Dead) || (gState == Hidden); }

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
#pragma once
#ifndef ENEMYUNIT_H
#define ENEMYUNIT_H

#include "Unit.h"
#include "PlayerUnit.h"
#include "../../../AI/Patrol/NodeMap.h"

enum EnemyBehaviourState
{
	Roaming,
	Hunting,
	Returning
};

class EnemyUnit : public Unit
{

#define UpdateHuntTime 1.0 / 30.0f

	bool hasTargetPos;
	float updateHuntTimer;

protected:
	NodeMap*			gNodeMap;

	vector<XMFLOAT3>	gPath;
	XMFLOAT3			gTargetPos;

	//vector<PatrolNode*>	gNodePath;
	PatrolNode*			gTargetNode;

	vector<PlayerUnit*>	gTargets;

	PlayerUnit*			gTargetPlayer;

	EnemyBehaviourState	gBehaviourState;

	bool	HasVisionOnTarget;

	//	0 = Health, 1 = Damage, 2 = Speed
	float	gMultipliers[3];

	//void	UpdateWalkBack(float deltaTime, Terrain* terrain);
	//void	UpdatePatrol(float deltaTime);
	void	UpdateHunt(float deltaTime, Terrain* terrain);

	void	FollowPath();
	void	GetNewPath(Terrain* terrain);

	void	ScanForEnemies(Terrain* terrain);

	void SetTargetPos(XMFLOAT3 pos)
	{
		gTargetPos = pos;
		hasTargetPos = true;
	}

	void	Boost()
	{
		//	Update health
		gHealth.second	*=	gMultipliers[0];
		gHealth.first	=	gHealth.second;

		//	Update health
		gHealth.second	*=	gMultipliers[0];

		//	Update speed
		gWalkSpeed	*=		gMultipliers[2];
		gRunSpeed	*=		gMultipliers[2];
	}

public:
	EnemyUnit(void);
	~EnemyUnit(void);

	virtual	void	Update(float deltaTime, Terrain* terrain);

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD);
	void	CollideWith(GameObject* Instance, vector<CollisionData> CD);

	void	SetNodeMap( NodeMap* Instance )
	{
		gNodeMap	=	Instance;
	}

	void	SetTargets( vector<PlayerUnit*> Targets )
	{
		gTargets = Targets;
	}

	void	SetMultipliers( float Health, float Damage, float Speed )
	{
		gMultipliers[0]	=	Health;
		gMultipliers[1]	=	Damage;
		gMultipliers[2]	=	Speed;
	}
	void	Boost( int NumberOfTimes )
	{
		for( int n = 0; n < NumberOfTimes; ++n )
			Boost();
	}
};

#endif
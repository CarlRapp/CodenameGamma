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
	bool Attacking;
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

	float	AttackRange;
	float	AttackMaxRange;

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
};

#endif
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
	bool hasTargetPos;


protected:
	NodeMap*			gNodeMap;

	vector<XMFLOAT3>	gPath;
	XMFLOAT3			gTargetPos;

	//vector<PatrolNode*>	gNodePath;
	PatrolNode*			gTargetNode;

	vector<PlayerUnit*>	gTargets;

	PlayerUnit*			gTargetPlayer;

	EnemyBehaviourState	gBehaviourState;

	//void	UpdateWalkBack(float deltaTime, Terrain* terrain);
	//void	UpdatePatrol(float deltaTime);
	void	UpdateHunt();

	void	FollowPath();
	void	GetNewPath(Terrain* terrain);

	void	ScanForEnemies();

	void SetTargetPos(XMFLOAT3 pos)
	{
		gTargetPos = pos;
		hasTargetPos = true;
	}

public:
	EnemyUnit(void);
	~EnemyUnit(void);

	void	Update(float deltaTime, Terrain* terrain);

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
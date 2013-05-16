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
protected:
	NodeMap*	gNodeMap;

	vector<XMFLOAT2>	gPath;
	XMFLOAT3			gTargetPos;

	vector<PatrolNode*>	gNodePath;
	PatrolNode*			gTargetNode;

	PlayerUnit*			gTargetPlayer;

	EnemyBehaviourState	gBehaviourState;

	void	UpdateWalkBack(float deltaTime, Terrain* terrain);
	void	UpdatePatrol(float deltaTime);
	void	UpdateHunt(float deltaTime);

public:
	EnemyUnit(void);
	~EnemyUnit(void);

	void	Update(float deltaTime, Terrain* terrain);

	void	SetNodeMap( NodeMap* Instance )
	{
		gNodeMap	=	Instance;
	}

	void	SetTarget( PlayerUnit* Target )
	{
		gTargetPlayer = Target;
	}
};

#endif
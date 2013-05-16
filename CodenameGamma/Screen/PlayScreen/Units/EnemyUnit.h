#pragma once
#ifndef ENEMYUNIT_H
#define ENEMYUNIT_H

#include "Unit.h"
#include "PlayerUnit.h"
#include "../../../AI/Patrol/NodeMap.h"

class EnemyUnit : public Unit
{
protected:
	NodeMap*	gNodeMap;

	vector<PatrolNode*>	gNodePath;
	PatrolNode*			gTargetNode;
	PlayerUnit*			gTargetPlayer;

	void	UpdatePatrol(float deltaTime);

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
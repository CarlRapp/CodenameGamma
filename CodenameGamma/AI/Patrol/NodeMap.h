#pragma once
#ifndef NODEMAP_H
#define NODEMAP_H

#include "../../stdafx.h"
using namespace std;

struct PatrolNode
{
	XMFLOAT2			Position;
	vector<PatrolNode*>	AdjacentNodes;

	PatrolNode()
	{
		AdjacentNodes	=	vector<PatrolNode*>();
	}

	PatrolNode*	GetRandomNode( )
	{
		if ( AdjacentNodes.size() == 0 )
			return 0;

		return AdjacentNodes[ (int)MathHelper::RandF( 0, AdjacentNodes.size() - 1 ) ];
	}

	bool	CanReach( PatrolNode* Node )
	{
		for each( PatrolNode* tNode in AdjacentNodes )
			if( tNode == Node )
				return true;

		return false;
	}
};

class NodeMap
{
private:
	vector<PatrolNode*>	gNodes;

public:
	NodeMap( void );
	~NodeMap( void );

	PatrolNode*	GetRandomNode( void );
	void		AddNode( PatrolNode* Node );

	void		SetNodeAdjacent( PatrolNode* NodeA, PatrolNode* NodeB );
	PatrolNode*	GetClosestNode( XMFLOAT3 Position );
};
#endif
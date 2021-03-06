#pragma once
#ifndef NODEMAP_H
#define NODEMAP_H

#include "../../stdafx.h"
using namespace std;

struct PatrolNode
{
	XMFLOAT2			Position;
	vector<PatrolNode*>	AdjacentNodes;
	vector<float>		AdjacentNodeDistances;

	PatrolNode()
	{
		AdjacentNodes			=	vector<PatrolNode*>();
		AdjacentNodeDistances	= 	vector<float>();
	}
	PatrolNode( XMFLOAT2 Pos )
	{
		Position		=	Pos;
		AdjacentNodes	=	vector<PatrolNode*>();
		AdjacentNodeDistances	= 	vector<float>();
	}

	PatrolNode*	GetRandomNode( )
	{
		if ( AdjacentNodes.size() == 0 )
			return 0;

		return AdjacentNodes[ (int)MathHelper::RandF( 0, AdjacentNodes.size() ) ];
	}

	bool	CanReach( PatrolNode* Node )
	{
		for each( PatrolNode* tNode in AdjacentNodes )
			if( tNode == Node )
				return true;

		return false;
	}

	int	NumberOfAdjacents() { return AdjacentNodes.size(); }
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

	bool		BuildPath( XMFLOAT3 PositionA, XMFLOAT3 PositionB, vector<XMFLOAT3>& path );
};
#endif
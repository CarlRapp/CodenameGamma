#include "NodeMap.h"

NodeMap::NodeMap()
{
	gNodes	=	vector<PatrolNode*>();
}
NodeMap::~NodeMap()
{
	for( int i = gNodes.size() - 1; i >= 0; --i )
		delete	gNodes[i];
}

void NodeMap::AddNode( PatrolNode* Node )
{
	for each( PatrolNode* tNode in gNodes )
		if( tNode == Node )
			return;

	gNodes.push_back( Node );
}

PatrolNode*	NodeMap::GetRandomNode()
{
	if( gNodes.size() == 0)
		return 0;

	return gNodes[ (int)MathHelper::RandF( 0, gNodes.size() ) ];
}

void NodeMap::SetNodeAdjacent( PatrolNode* NodeA, PatrolNode* NodeB )
{
	//	See if NodeA already has
	//	NodeB added to its Adjacent list
	if( !NodeA->CanReach( NodeB ) )
		NodeA->AdjacentNodes.push_back( NodeB );

	//	Do the same for NodeB
	if( !NodeB->CanReach( NodeA ) )
		NodeB->AdjacentNodes.push_back( NodeA );
}

PatrolNode*	NodeMap::GetClosestNode( XMFLOAT3 Position )
{
	PatrolNode*	Result	=	gNodes[0];
	float		fLength;
	XMStoreFloat(
		&fLength,
		XMVector3Length( XMLoadFloat3( &Position ) - XMLoadFloat3( &XMFLOAT3( gNodes[0]->Position.x, 0, gNodes[0]->Position.y ) ) )
	);

	for each( PatrolNode* Node in gNodes )
	{
		float	tLength;
		XMStoreFloat(
			&tLength,
			XMVector3Length( XMLoadFloat3( &Position ) - XMLoadFloat3( &XMFLOAT3( Node->Position.x, 0, Node->Position.y ) ) )
		);

		if ( tLength < fLength )
		{
			fLength	=	tLength;
			Result	=	Node;
		}
	}


	return Result;
}

bool NodeMap::BuildPath( XMFLOAT3 PositionA, XMFLOAT3 PositionB, vector<XMFLOAT3>& path )
{
	path.clear();

	vector<PatrolNode*>	visitedNodes	=	vector<PatrolNode*>();

	visitedNodes.push_back( GetClosestNode( PositionA ) );
	PatrolNode*	lastVisited	=	visitedNodes[0];
	PatrolNode*	goalNode	=	GetClosestNode( PositionB );

	if( lastVisited == goalNode )
		return false;

	for( int i = 0; i < 25; ++i )
	{
		PatrolNode*	tNode	=	lastVisited->GetRandomNode();
		
		//	Lower the risk of getting stuck in a dead end
		if ( tNode->NumberOfAdjacents() == 1 && tNode != goalNode )
			continue;

		bool	Exists	=	false;
		for each( PatrolNode* Node in visitedNodes )
			if ( Node == tNode )
			{
				Exists	=	true;
				break;
			}

		if( !Exists )
		{
			lastVisited	=	tNode;
			visitedNodes.push_back( tNode );
		}

		if( tNode == goalNode )
			break;
	}
	
	for( int i = visitedNodes.size() - 1; i >= 0; --i )
	{
		XMFLOAT2 pos2 = visitedNodes[i]->Position;
		XMFLOAT3 pos3 = XMFLOAT3(pos2.x, 0, pos2.y);
		path.push_back( pos3 );
	}

	visitedNodes.clear();
	return !path.empty();
}
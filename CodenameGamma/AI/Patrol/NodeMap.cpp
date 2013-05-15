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

	return gNodes[ (int)MathHelper::RandF( 0, gNodes.size() ) - 1 ];
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
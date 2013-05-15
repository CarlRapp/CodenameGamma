#include "EnemyUnit.h"

EnemyUnit::EnemyUnit()
{
	gTargetNode		=	0;
	gTargetPlayer	=	0;
}
EnemyUnit::~EnemyUnit()
{

}

void EnemyUnit::Update(float DeltaTime, Terrain* terrain)
{
	UpdatePatrol( DeltaTime );



	Unit::Update(DeltaTime, terrain);
}

void EnemyUnit::UpdatePatrol(float DeltaTime)
{
	//	If the unit is chasing a target,
	//	cancel this method
	if( gTargetPlayer != 0)
		return;

	//	If it doesnt have a node get the
	//	node closest to its current position
	if( gTargetNode == 0 )
	{
		PatrolNode*	tNode	=	gNodeMap->GetClosestNode( GetFloat3Value( Position ) );

		if( tNode == 0 )
			return;

		gTargetNode	=	tNode;
	}

	//	Save some useful fields
	//	that will be used throughout
	//	the method
	XMFLOAT3	newVelocity, tPosition	=	XMFLOAT3( gTargetNode->Position.x, 0, gTargetNode->Position.y );

	//	Check if the unit is at its
	//	target node position
	float	tLength;
	XMStoreFloat(
		&tLength,
		XMVector3Length( XMLoadFloat3( &tPosition ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);

	//	If the unit is withing 50cm
	//	from its target node make it
	//	pick a new one
	if( tLength < 0.5f * UnitsPerMeter )
	{
		gTargetNode	=	gTargetNode->GetRandomNode();
		tPosition	=	XMFLOAT3( gTargetNode->Position.x, 0, gTargetNode->Position.y );
	}

	XMStoreFloat3(
		&newVelocity,
		3 * UnitsPerMeter * XMVector3Normalize( XMLoadFloat3( &tPosition ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);

	SetVelocity( newVelocity );
}
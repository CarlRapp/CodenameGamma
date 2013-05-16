#include "EnemyUnit.h"

EnemyUnit::EnemyUnit()
{
	gTargetNode		=	0;
	gTargetPlayer	=	0;
	gBehaviourState	=	Roaming;
}
EnemyUnit::~EnemyUnit()
{

}

void EnemyUnit::Update(float DeltaTime, Terrain* terrain)
{
	switch( gBehaviourState )
	{
		case Roaming:
			UpdatePatrol( DeltaTime );
			break;

		case Hunting:
			UpdateHunt( DeltaTime );
			break;
	}



	Unit::Update(DeltaTime, terrain);
}

void EnemyUnit::UpdatePatrol(float DeltaTime)
{
	//	If the unit is chasing a target,
	//	cancel this method
	if( gTargetPlayer == 0 )
		return;

	//	If it doesnt have a node get the
	//	node closest to its current position
	if( gTargetNode == 0 )
	{
		//	Create a path from the enemy
		//	to its target, via nodes.
		gNodePath	=	gNodeMap->BuildPath( 
							GetFloat3Value( Position ), 
							gTargetPlayer->GetFloat3Value( Position ) 
						);

		//	Size == 0 means that there
		//	was no path, i.e. the start
		//	and goal node is the same node
		if( gNodePath.size() == 0 )
		{

			//	FOR NOW JUST MAKE THE UNIT STOP!!!
			//	FOR NOW JUST MAKE THE UNIT STOP!!!
			//	FOR NOW JUST MAKE THE UNIT STOP!!!
			//	FOR NOW JUST MAKE THE UNIT STOP!!!
			SetVelocity( XMFLOAT3( 0, 0, 0 ) );

			return;
		}

		//	Pop the last node from
		//	the list
		gTargetNode	=	gNodePath.back();
		gNodePath.pop_back();
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
		//	If there are no more nodes to 
		//	follow, null the target node
		if ( gNodePath.size() == 0 )
		{
			gTargetNode	=	0;
			return;
		}

		//	Get the next node to walk to
		gTargetNode	=	gNodePath.back();
		gNodePath.pop_back();

		tPosition	=	XMFLOAT3( gTargetNode->Position.x, 0, gTargetNode->Position.y );
	}

	//	Calculate the new velocity.
	XMStoreFloat3(
		&newVelocity,
		3 * UnitsPerMeter * XMVector3Normalize( XMLoadFloat3( &tPosition ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);

	SetVelocity( newVelocity );

	LookAt(
		tPosition
	);


	//	Placeholder for entering HUNT behaviour
	XMStoreFloat(
		&tLength,
		XMVector3Length( XMLoadFloat3( &gTargetPlayer->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);
	if( tLength < 5 * UnitsPerMeter )
		gBehaviourState	=	Hunting;
}

void EnemyUnit::UpdateHunt(float deltaTime)
{
	//	Calculate the new velocity.
	XMFLOAT3	newVelocity;
	XMStoreFloat3(
		&newVelocity,
		5 * UnitsPerMeter * XMVector3Normalize( XMLoadFloat3( &gTargetPlayer->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);

	SetVelocity( newVelocity );
	LookAt( gTargetPlayer->GetFloat3Value( Position ) );

	float tLength;
	XMStoreFloat(
		&tLength,
		XMVector3Length( XMLoadFloat3( &gTargetPlayer->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);
	if( tLength > 10 * UnitsPerMeter )
		gBehaviourState	=	Roaming;
}
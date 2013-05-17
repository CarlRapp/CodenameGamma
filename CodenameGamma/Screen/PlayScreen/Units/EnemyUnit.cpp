#include "EnemyUnit.h"

EnemyUnit::EnemyUnit()
{
	gTargetNode		= 0;
	gTargetPlayer	= 0;
	gBehaviourState	= Returning;
	hasTargetPos	= false;
}
EnemyUnit::~EnemyUnit()
{

}

void EnemyUnit::Update(float DeltaTime, Terrain* terrain)
{
	if ( gBehaviourState == Hunting)
	{
		UpdateHunt();
	}
	else
	{
		if (!hasTargetPos)
			GetNewPath(terrain);

		FollowPath();
		ScanForEnemies();
	}

	/*
	switch( gBehaviourState )
	{
		case Roaming:
			UpdatePatrol( DeltaTime );
			break;

		case Hunting:
			UpdateHunt( DeltaTime );
			break;

		case Returning:
			UpdateWalkBack( DeltaTime, terrain );
			break;
	}
	*/
	Unit::Update(DeltaTime, terrain);
}

void EnemyUnit::GetNewPath(Terrain* terrain)
{
	XMFLOAT3 startPos = GetFloat3Value( Position );
	XMFLOAT2 temp;
	XMFLOAT3 endPos;

	bool foundPath = false;
	switch (gBehaviourState)
	{
	case Roaming:		
		temp	= gNodeMap->GetRandomNode()->Position;
		endPos = XMFLOAT3(temp.x, 0, temp.y);
		//cout << "New Roaming path to " << endPos.x << " - " << endPos.z << endl;
		foundPath = gNodeMap->BuildPath(startPos, endPos, gPath);		
		break;
	case Returning:		
		temp	= gNodeMap->GetClosestNode(startPos)->Position;
		endPos = XMFLOAT3(temp.x, 0, temp.y);
		//cout << "New Returning path to " << endPos.x << " - " << endPos.z << endl;
		foundPath = terrain->FindPath(startPos, endPos, gPath);
		break;
	}

	if (foundPath && !gPath.empty())
	{
		gTargetPos = gPath.back();
		gPath.pop_back();
		hasTargetPos = true;
	}
}

void EnemyUnit::FollowPath()
{
	//	Check if the unit is at its
	//	target node position
	float	tLength;
	XMStoreFloat(&tLength, XMVector3Length( XMLoadFloat3( &gTargetPos ) - XMLoadFloat3( &GetFloat3Value( Position ) ) ));

	//	If the unit is withing 50cm
	//	from its target node make it
	//	pick a new one
	if( tLength < 0.5f * UnitsPerMeter )
	{
		//	If there are no more nodes to 
		//	follow, null the target node
		if ( gPath.empty() )
		{
			hasTargetPos	= false;
			gBehaviourState	= Roaming;
			return;
		}

		//	Get the next node to walk to
		gTargetPos = gPath.back();
		gPath.pop_back();

		//cout << "Targetpos: " << gTargetPos.x << " - " << gTargetPos.z << endl;
	}

	//	Calculate the new velocity.
	XMFLOAT3	newVelocity;
	XMStoreFloat3(&newVelocity, 3 * UnitsPerMeter * XMVector3Normalize( XMLoadFloat3( &gTargetPos ) - XMLoadFloat3( &GetFloat3Value( Position ) ) ) );

	SetVelocity( newVelocity );

	LookAt(gTargetPos);
}

void EnemyUnit::UpdateHunt()
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
		gBehaviourState	= Returning;
}

void EnemyUnit::ScanForEnemies()
{
	for each (PlayerUnit* target in gTargets)
	{
		float tLength;
		XMStoreFloat(&tLength, XMVector3Length( XMLoadFloat3( &target->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) ));
		if( tLength < 5 * UnitsPerMeter )
		{
			hasTargetPos	= false;
			gTargetPlayer	= target;
			gBehaviourState	= Hunting;
		}
	}
}

/*
void EnemyUnit::UpdateWalkBack(float deltaTime, Terrain* terrain)
{
	if( gTargetPos.x == -9999 )
	{
		XMFLOAT3 pos3 = GetFloat3Value( Position );
		XMFLOAT2 pos2 = XMFLOAT2(pos3.x, pos3.z);
		if (!terrain->FindPath(pos2, gNodeMap->GetClosestNode(pos3)->Position, gPath))
		{
			gBehaviourState	= Roaming;
			return;
		}

		XMFLOAT2 newTarget = gPath.front();
		gPath.erase(gPath.begin());

		gTargetPos = XMFLOAT3(newTarget.x, 0, newTarget.y);

		cout << "Targetpos: " << gTargetPos.x << " - " << gTargetPos.z << endl;
	}


	//	Save some useful fields
	//	that will be used throughout
	//	the method
	XMFLOAT3	newVelocity;

	//	Check if the unit is at its
	//	target node position
	float	tLength;
	XMStoreFloat(
		&tLength,
		XMVector3Length( XMLoadFloat3( &gTargetPos ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);

	//	If the unit is withing 50cm
	//	from its target node make it
	//	pick a new one
	if( tLength < 0.5f * UnitsPerMeter )
	{
		//	If there are no more nodes to 
		//	follow, null the target node
		if ( gPath.empty() )
		{
			gTargetPos.x = -9999;
			gBehaviourState	= Roaming;
			return;
		}

		//	Get the next node to walk to
		XMFLOAT2 newTarget = gPath.front();
		gPath.erase(gPath.begin());

		gTargetPos = XMFLOAT3(newTarget.x, 0, newTarget.y);
		cout << "Targetpos: " << gTargetPos.x << " - " << gTargetPos.z << endl;
	}

	//	Calculate the new velocity.
	XMStoreFloat3(
		&newVelocity,
		3 * UnitsPerMeter * XMVector3Normalize( XMLoadFloat3( &gTargetPos ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);

	SetVelocity( newVelocity );

	LookAt(
		gTargetPos
	);


	if( gTargetPlayer == 0 )
		return;
	//	Placeholder for entering HUNT behaviour
	XMStoreFloat(
		&tLength,
		XMVector3Length( XMLoadFloat3( &gTargetPlayer->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);
	if( tLength < 5 * UnitsPerMeter )
	{
		gTargetPos.x = -9999;
		gBehaviourState	=	Hunting;
	}
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
*/

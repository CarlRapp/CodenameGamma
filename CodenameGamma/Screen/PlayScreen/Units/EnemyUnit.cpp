#include "EnemyUnit.h"

EnemyUnit::EnemyUnit()
{
	gTargetNode		= 0;
	gTargetPlayer	= 0;
	gBehaviourState	= Returning;
	hasTargetPos	= false;
	HasVisionOnTarget = false;
	updateHuntTimer = 0.0f;

	gMultipliers[0]	=	1.0f;
	gMultipliers[1]	=	1.0f;
	gMultipliers[2]	=	1.0f;

	AttackRange = 1.0f;
	AttackMaxRange = 1.0f;
	Attacking = false;

}
EnemyUnit::~EnemyUnit()
{

}

void EnemyUnit::Update(float DeltaTime, Terrain* terrain)
{
	Unit::Update(DeltaTime, terrain);

	if (!IsAlive())
		return;

	if ( gBehaviourState == Hunting)
	{
		UpdateHunt(DeltaTime, terrain);
	}
	else
	{
		if (!hasTargetPos)
			GetNewPath(terrain);

		FollowPath();
		ScanForEnemies(terrain);
	}

	if (GetWeapon() != NULL)
	{
		GetWeapon()->Update( DeltaTime, terrain );

		if (!GetWeapon()->GotCallbackFunctions())
			SetCallbackFunctions(GetWeapon());
	}

	if ( gTargetPlayer == 0 || !HasVisionOnTarget || gBehaviourState != Hunting )
	{
		Attacking = false;
		if (gWeaponState != Unit::Hold)
			SetWeaponState(Unit::Hold);
		return;
	}

	float	distance;

	XMStoreFloat(&distance, XMVector3Length( XMLoadFloat3( &gTargetPlayer->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) ) );

	if( (Attacking && distance > AttackMaxRange * UnitsPerMeter) || (!Attacking && distance > AttackRange * UnitsPerMeter))
	{
		Attacking = false;
		if (gWeaponState != Unit::Hold)
			SetWeaponState(Unit::Hold);
		return;
	}

	SetVelocity( XMFLOAT3( 0, 0, 0 ) );

	if (gWeaponState != Unit::Aim)
		SetWeaponState(Unit::Aim);

	FireWeapon(gTargetPlayer);
	Attacking = true;
}

void EnemyUnit::GetNewPath(Terrain* terrain)
{
	XMFLOAT3 startPos = GetFloat3Value( Position );
	XMFLOAT2 temp;
	XMFLOAT3 endPos;

	bool foundPath = false;

	if (gBehaviourState == Roaming)
	{
		if (gTargets.empty())
			temp	= gNodeMap->GetRandomNode()->Position;

		XMVECTOR startPosV = XMLoadFloat3( &startPos );

		float closesDistance = INFINITE;
		for (int i = 0; i < gTargets.size(); ++i)
		{
			if (gTargets[i]->IsAlive())
			{
				XMFLOAT3 targetPos	= gTargets[i]->GetFloat3Value( Position );
				XMVECTOR targetPosV	= XMLoadFloat3( &targetPos );

				float distance = XMVectorGetX( XMVector3Length(targetPosV - startPosV) );

				if ( distance < closesDistance )
				{
					closesDistance = distance;
					temp = XMFLOAT2(targetPos.x, targetPos.z);
				}
			}
		}
		
		endPos = XMFLOAT3(temp.x, 0, temp.y);


		PatrolNode* CA = gNodeMap->GetClosestNode(startPos);
		PatrolNode* CB = gNodeMap->GetClosestNode(endPos);

		XMVECTOR A = XMLoadFloat2( &CA->Position );
		XMVECTOR B = XMLoadFloat2( &CB->Position );

		if (XMVector2Equal(A, B))
		{
			foundPath = terrain->FindPath(startPos, endPos, gPath);
			roamingInNodes = false;
		}
		else if (roamingInNodes)
		{
			foundPath = gNodeMap->BuildPath(startPos, endPos, gPath);
		}
		else
		{
			XMVECTOR target = XMLoadFloat2(&temp);

			float targetToNodeDist = XMVectorGetX( XMVector2Length(B - target) );

			if (closesDistance < targetToNodeDist)
				foundPath = terrain->FindPath(startPos, endPos, gPath);

			else
			{
				endPos = XMFLOAT3(CA->Position.x, 0, CA->Position.y);
				foundPath = terrain->FindPath(startPos, endPos, gPath);
				roamingInNodes = true;
			}
		}
	}

	else if (gBehaviourState == Returning)
	{
		temp	= gNodeMap->GetClosestNode(startPos)->Position;
		endPos = XMFLOAT3(temp.x, 0, temp.y);
		foundPath = terrain->FindPath(startPos, endPos, gPath);
		roamingInNodes = true;
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

	}

	//	Calculate the new velocity.
	XMFLOAT3	newVelocity;
	XMStoreFloat3(&newVelocity, gWalkSpeed * UnitsPerMeter * XMVector3Normalize( XMLoadFloat3( &gTargetPos ) - XMLoadFloat3( &GetFloat3Value( Position ) ) ) );

	SetVelocity( newVelocity );

	LookAtXZ(gTargetPos);
}

void EnemyUnit::UpdateHunt(float deltaTime, Terrain* terrain)
{
	if (!gTargetPlayer->IsAlive())
	{
		HasVisionOnTarget = false;
		gBehaviourState	= Roaming;
		roamingInNodes = false;
		return;
	}

	updateHuntTimer += deltaTime;
	if (updateHuntTimer < UpdateHuntTime)
		return;

	updateHuntTimer = 0.0f;

	HasVisionOnTarget = false;
	XMFLOAT3 target = gTargetPlayer->GetFloat3Value( Position );
	if (!terrain->IsShortestPathFree( GetFloat3Value( Position ), target ))
	{
		vector<XMFLOAT3> trails = gTargetPlayer->GetTrails();
		bool foundTrail = false;
		for (int i = 0; i < trails.size(); ++i)
		{
			target = trails[i];


			//std::cout << "Trying trail " << target.x << ", " << target.z << endl;

			//if (terrain->IsPathFree( GetFloat3Value( Position ), target, 1.0f))
			if (terrain->IsShortestPathFree( GetFloat3Value( Position ), target ))
			{
				//std::cout << "Following trail " << i  << " of " << trails.size() << " trails." << endl;
				foundTrail = true;
				break;
			}
		}

		if (!foundTrail)
		{
			//std::cout << "Lost trail - trailsize: " << trails.size() << endl;
			gBehaviourState	= Returning;
			return;
		}
	}	
	else
		HasVisionOnTarget = true;

	//terrain->FindPath(GetFloat3Value( Position ), target, gPath);
	//target = gPath.back();

	//	Calculate the new velocity.

	float tLength;
	XMStoreFloat(&tLength, XMVector3Length( XMLoadFloat3( &target ) - XMLoadFloat3( &GetFloat3Value( Position ) ) ));

	if( tLength > 20 * UnitsPerMeter )
	{
		gBehaviourState	= Returning;
		HasVisionOnTarget = false;
		return;
	}

	LookAtXZ( target );

	if (Attacking)
		return;

	XMFLOAT3	newVelocity;
	XMStoreFloat3(&newVelocity, gRunSpeed * UnitsPerMeter * XMVector3Normalize( XMLoadFloat3( &target ) - XMLoadFloat3( &GetFloat3Value( Position ) ) ));

	SetVelocity( newVelocity );
	

	
}

void EnemyUnit::ScanForEnemies(Terrain* terrain)
{
	for each (PlayerUnit* target in gTargets)
	{
		float tLength;
		XMStoreFloat(&tLength, XMVector3Length( XMLoadFloat3( &target->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) ));
		if( tLength < 5 * UnitsPerMeter )
		{
			if (terrain->IsShortestPathFree( GetFloat3Value( Position ), target->GetFloat3Value( Position ) ))
			{
				XMVECTOR lookDir	= XMLoadFloat3(&GetFloat3Value( Direction ));
				XMVECTOR targetDir	= XMLoadFloat3(&target->GetFloat3Value( Position )) - XMLoadFloat3(&GetFloat3Value( Position ));				
				float angle = XMVectorGetX(XMVector3AngleBetweenVectors(lookDir, targetDir));

				if (angle < 0.25 * PI)
				{
					hasTargetPos	= false;
					gTargetPlayer	= target;
					gBehaviourState	= Hunting;
					gPath.clear();
					return;
				}
			}			
		}
	}
}

void EnemyUnit::Hunt(PlayerUnit* target)
{
	hasTargetPos	= false;
	gTargetPlayer	= target;
	gBehaviourState	= Hunting;
	gPath.clear();
}

bool EnemyUnit::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	return false;
	/*
	if ( IsOfType<EnemyUnit>(B) )
		return BoxVsBox(this, B, CD, false);
	return false;
	*/
}

void EnemyUnit::CollideWith(GameObject* Instance, vector<CollisionData> CD)
{
	if ( IsOfType<EnemyUnit>(Instance))
	{
		//om "this" är bakom Instance och vinkel mellan dem inte skiljer mer än 0.5f * PI
		//då låter vi "this" stå still i 1.0f sekunder. detta låter Instance flytta på sig innan "this" går vidare.
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

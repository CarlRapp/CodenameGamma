#include "CannedFood.h"


CannedFood::CannedFood()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CannedFood" ) );
	SetTeam( Neutral );
	SetScale(10);
}

CannedFood::~CannedFood()
{

}

bool CannedFood::Update(float DeltaTime, Terrain* terrain)
{
	AddRotation( XMFLOAT3( 0, DeltaTime, 0 ) );


	return Item::Update(DeltaTime, terrain);
}


void CannedFood::OnPickUp(Unit* Instance)
{
	if ( !IsOfType<PlayerUnit>(Instance) )
		return;

	PlayerUnit*	pUnit	=	(PlayerUnit*)Instance;

	
	SetState( Dead );
}
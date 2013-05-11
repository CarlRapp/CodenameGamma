#include "CannedFood.h"


CannedFood::CannedFood()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CannedFood" ) );
}

CannedFood::~CannedFood()
{

}

bool CannedFood::Update(float DeltaTime, Terrain* terrain)
{
	return Item::Update(DeltaTime, terrain);
}


void CannedFood::OnPickUp(Unit* Instance)
{
	if ( !IsOfType<PlayerUnit>(Instance) )
		return;

	PlayerUnit*	pUnit	=	(PlayerUnit*)Instance;
	pUnit->Eat( 10 );
	
	SetState( Dead );
}
#include "TownHall.h"

TownHall::TownHall()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TownHall" ) );
	SetScale( XMFLOAT3( 15, 15, 15 ) );
}

TownHall::~TownHall()
{

}
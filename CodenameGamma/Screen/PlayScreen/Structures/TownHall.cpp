#include "TownHall.h"

TownHall::TownHall()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TownHall" ) );
	SetScale( 15 );
}

TownHall::~TownHall()
{

}
#include "TownHall.h"

TownHall::TownHall()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TownHall" ) );
}

TownHall::~TownHall()
{

}
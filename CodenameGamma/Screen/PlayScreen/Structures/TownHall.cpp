#include "TownHall.h"

TownHall::TownHall()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TownHall" ) );
	PlayAnimation("StartPose");
	//PlayAnimation("ALL");
}

TownHall::~TownHall()
{

}
#include "TownHall.h"

TownHall::TownHall()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TownHall" ) );
	UsePose("StartPose");
	//PlayAnimation("ALL");
}

TownHall::~TownHall()
{

}
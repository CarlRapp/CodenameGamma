#include "TownHall.h"

TownHall::TownHall()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TownHall" ) );
	PlayAnimation("StartPose");
	
	gVegModel = "TownHall_VEG";
}

TownHall::~TownHall()
{

}
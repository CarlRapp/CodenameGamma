#include "HotDogStand.h"

HotDogStand::HotDogStand()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "HotDogStand" ) );

	gVegModel = "HotDogStand_VEG";
}

HotDogStand::~HotDogStand()
{

}
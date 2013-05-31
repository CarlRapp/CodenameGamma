#include "PickupTruck.h"

PickupTruck::PickupTruck()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "PickUpTruck" ) );
	
	gVegModel = "PickUpTruck_VEG";
}

PickupTruck::~PickupTruck()
{

}
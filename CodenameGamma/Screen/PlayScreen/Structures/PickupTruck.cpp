#include "PickupTruck.h"

PickupTruck::PickupTruck()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "PickUpTruck" ) );
}

PickupTruck::~PickupTruck()
{

}
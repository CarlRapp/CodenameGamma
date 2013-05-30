#include "ApartmentBuilding.h"

ApartmentBuilding::ApartmentBuilding()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "ApartmentBuilding" ) );

	gVegModel = "ApartmentBuilding_VEG";
}

ApartmentBuilding::~ApartmentBuilding()
{

}
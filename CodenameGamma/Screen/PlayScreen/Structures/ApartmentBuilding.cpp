#include "ApartmentBuilding.h"

ApartmentBuilding::ApartmentBuilding()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "ApartmentBuilding" ) );
}

ApartmentBuilding::~ApartmentBuilding()
{

}
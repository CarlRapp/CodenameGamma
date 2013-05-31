#include "FactoryBuilding.h"

FactoryBuilding::FactoryBuilding()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "FactoryBuilding" ) );

	gVegModel = "FactoryBuilding_VEG";
}

FactoryBuilding::~FactoryBuilding()
{

}
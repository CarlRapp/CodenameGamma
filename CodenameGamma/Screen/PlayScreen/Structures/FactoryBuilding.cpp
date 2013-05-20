#include "FactoryBuilding.h"

FactoryBuilding::FactoryBuilding()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "FactoryBuilding" ) );
}

FactoryBuilding::~FactoryBuilding()
{

}
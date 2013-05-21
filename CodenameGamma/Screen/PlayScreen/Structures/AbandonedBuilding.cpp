#include "AbandonedBuilding.h"

AbandonedBuilding::AbandonedBuilding()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "AbandonedBuilding" ) );
}

AbandonedBuilding::~AbandonedBuilding()
{

}
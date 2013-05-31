#include "AbandonedBuilding.h"

AbandonedBuilding::AbandonedBuilding()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "AbandonedBuilding" ) );

	gVegModel = "AbandonedBuilding_VEG";
}

AbandonedBuilding::~AbandonedBuilding()
{

}
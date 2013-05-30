#include "CrowdBarrier.h"

CrowdBarrier::CrowdBarrier()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CrowdBarrier" ) );

	gVegModel = "CrowdBarrier_VEG";
}

CrowdBarrier::~CrowdBarrier()
{

}
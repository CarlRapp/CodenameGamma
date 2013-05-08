#include "CrowdBarrier.h"

CrowdBarrier::CrowdBarrier()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CrowdBarrier" ) );
}

CrowdBarrier::~CrowdBarrier()
{

}
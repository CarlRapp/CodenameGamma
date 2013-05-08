#include "CrowdBarrier.h"

CrowdBarrier::CrowdBarrier()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CrowdBarrier" ) );
	SetScale( 7 );
}

CrowdBarrier::~CrowdBarrier()
{

}
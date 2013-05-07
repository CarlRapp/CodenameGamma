#include "CrowdBarrier.h"

CrowdBarrier::CrowdBarrier()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CrowdBarrier" ) );
	SetScale( XMFLOAT3( 10, 5, 10 ) );
}

CrowdBarrier::~CrowdBarrier()
{

}
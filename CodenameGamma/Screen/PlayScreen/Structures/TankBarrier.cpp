#include "TankBarrier.h"

TankBarrier::TankBarrier()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TankBarrier" ) );
}

TankBarrier::~TankBarrier()
{

}
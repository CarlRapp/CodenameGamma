#include "TankBarrier.h"

TankBarrier::TankBarrier()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TankBarrier" ) );
	
	gVegModel = "TankBarrier_VEG";
}

TankBarrier::~TankBarrier()
{

}
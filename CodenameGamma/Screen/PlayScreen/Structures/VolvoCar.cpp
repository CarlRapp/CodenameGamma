#include "VolvoCar.h"

VolvoCar::VolvoCar()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "VolvoCar" ) );
	
	gVegModel = "VolvoCar_VEG";
}

VolvoCar::~VolvoCar()
{

}
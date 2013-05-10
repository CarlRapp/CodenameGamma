#include "VolvoCar.h"

VolvoCar::VolvoCar()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "VolvoCar" ) );
}

VolvoCar::~VolvoCar()
{

}
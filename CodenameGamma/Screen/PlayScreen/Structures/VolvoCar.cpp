#include "VolvoCar.h"

VolvoCar::VolvoCar()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "VolvoCar" ) );
	SetScale( 6 );
}

VolvoCar::~VolvoCar()
{

}
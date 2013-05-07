#include "VolvoCar.h"

VolvoCar::VolvoCar()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "VolvoCar" ) );
	SetScale( XMFLOAT3( 6, 6, 6 ) );
}

VolvoCar::~VolvoCar()
{

}
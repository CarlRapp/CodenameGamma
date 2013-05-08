#include "UnitCube.h"

UnitCube::UnitCube()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "UnitCube" ) );
	SetScale( XMFLOAT3( 1, 1, 1 ) );
}

UnitCube::~UnitCube()
{

}
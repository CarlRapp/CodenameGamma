#include "UnitCube.h"

UnitCube::UnitCube()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "UnitCube" ) );
}

UnitCube::~UnitCube()
{

}
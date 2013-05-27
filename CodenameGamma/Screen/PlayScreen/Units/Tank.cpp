#include "Tank.h"

Tank::Tank()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Tank" ) );
	SetWeaponState(Hold);

	gWalkSpeed = 1.5f;
	gRunSpeed  = 3.0f;

	gDeathTime = 5.0f;
}

Tank::~Tank()
{

}
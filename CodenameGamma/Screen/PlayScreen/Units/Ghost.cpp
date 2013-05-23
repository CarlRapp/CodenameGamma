#include "Ghost.h"

Ghost::Ghost()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Ghost" ) );
	SetWeaponState(Hold);

	gWalkSpeed = 3.0f;
	gRunSpeed  = 5.0f;

}

Ghost::~Ghost()
{

}
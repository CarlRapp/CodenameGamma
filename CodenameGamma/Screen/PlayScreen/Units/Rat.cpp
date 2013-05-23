#include "Rat.h"

Rat::Rat()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Rat" ) );
	SetWeaponState(Hold);

	SetScale(2.5f);

	gWalkSpeed = 3.0f;
	gRunSpeed  = 5.0f;

	gDeathTime = 5.0f;

}

Rat::~Rat()
{

}
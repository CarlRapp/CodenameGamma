#include "Rat.h"

Rat::Rat()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CrazyBitch" ) );
	SetWeaponState(Hold);


	gWalkSpeed = 3.0f;
	gRunSpeed  = 5.0f;

}

Rat::~Rat()
{

}
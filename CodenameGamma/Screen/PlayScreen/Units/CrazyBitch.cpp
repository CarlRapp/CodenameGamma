#include "CrazyBitch.h"

CrazyBitch::CrazyBitch()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CrazyBitch" ) );
	SetWeaponState(Hold);

	gWalkSpeed = 5.0f;
	gRunSpeed  = 8.0f;

	gDeathTime = 10.0f;
}

CrazyBitch::~CrazyBitch()
{

}
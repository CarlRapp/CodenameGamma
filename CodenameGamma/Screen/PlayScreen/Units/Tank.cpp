#include "Tank.h"
#include "../Weapons/TankAttack.h"

Tank::Tank()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Tank" ) );
	SetWeaponState(Hold);

	SetHealth( UnitHealth( 50.0f, 50.0f ) );

	gWalkSpeed = 1.5f;
	gRunSpeed  = 3.0f;

	gDeathTime = 5.0f;

	AttackRange = 1.5f;
	AttackMaxRange = 2.0f;
	
	gCurrentWeapon	=	new TankAttack();
	gPrimaryWeapon	=	gCurrentWeapon;
}

Tank::~Tank()
{

}
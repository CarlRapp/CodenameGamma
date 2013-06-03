#include "Tank.h"
#include "../Weapons/TankAttack.h"

Tank::Tank()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Tank" ) );
	SetWeaponState(Hold);

	SetHealth( UnitHealth( 150.0f, 150.0f ) );

	gWalkSpeed = 1.8f;
	gRunSpeed  = 3.6f;

	SetScale(1.5f);

	gDeathTime = 5.0f;

	AttackRange = 1.5f;
	AttackMaxRange = 2.0f;
	
	gCurrentWeapon	=	new TankAttack();
	gPrimaryWeapon	=	gCurrentWeapon;
}

Tank::~Tank()
{

}
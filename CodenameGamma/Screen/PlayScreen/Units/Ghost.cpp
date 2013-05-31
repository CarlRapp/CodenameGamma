#include "Ghost.h"
#include "../Weapons/GhostAttack.h"

Ghost::Ghost()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Ghost" ) );
	SetWeaponState(Hold);

	SetHealth( UnitHealth( 50.0f, 50.0f ) );

	gWalkSpeed = 3.0f;
	gRunSpeed  = 5.0f;

	gDeathTime = 5.0f;

	gCurrentWeapon	=	new GhostAttack();
	gPrimaryWeapon	=	gCurrentWeapon;

	AttackRange = 4.5f;
	AttackMaxRange = 6.0f;
}

Ghost::~Ghost()
{

}

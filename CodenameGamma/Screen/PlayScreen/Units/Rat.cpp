#include "Rat.h"
#include "../Weapons/RatAttack.h"

Rat::Rat()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Rat" ) );
	SetWeaponState(Hold);

	SetHealth( UnitHealth( 50.0f, 50.0f ) );

	SetScale(2.5f);

	gWalkSpeed = 3.0f;
	gRunSpeed  = 5.0f;

	gDeathTime = 5.0f;

	SetWeapon( new RatAttack() );

	AttackRange = 1.5f;
	AttackMaxRange = 2.0f;
}

Rat::~Rat()
{

}

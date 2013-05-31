#include "TankAttack.h"
#include "../Units/Unit.h"

TankAttack::TankAttack()
{
	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(1, 1);
	gReloadTime	=	WeaponReloadTime(0.0f, 4.0f);

	gWeaponAnimations.Shoot				= "Shoot";
}

TankAttack::~TankAttack()
{

}

bool TankAttack::Fire( GameObject* Owner, GameObject* Target, float DamageMul )
{
	if ( CanFire() )
	{
		gClip.first	=	0;

		if ( IsOfType<Unit>(Target) )
			((Unit*)Target)->Hurt( 1.5f * DamageMul );
		return true;
	}

	return false;
}
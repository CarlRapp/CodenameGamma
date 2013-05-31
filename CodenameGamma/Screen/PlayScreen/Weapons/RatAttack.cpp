#include "RatAttack.h"
#include "../Units/Unit.h"

RatAttack::RatAttack()
{
	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(1, 1);
	gReloadTime	=	WeaponReloadTime(0.0f, 4.0f);
	gAmmo		=	-1;
}

RatAttack::~RatAttack()
{

}

bool RatAttack::Fire( GameObject* Owner, GameObject* Target, float DamageMul )
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
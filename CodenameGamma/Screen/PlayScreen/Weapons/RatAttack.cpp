#include "RatAttack.h"
#include "../Units/Unit.h"

RatAttack::RatAttack()
{
	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(1, 1);
	gReloadTime	=	WeaponReloadTime(0.0f, 2.0f);
	gAmmo		=	-1;
	gWeaponAnimations.Aim				= "Aim";
	gWeaponAnimations.Shoot				= "Shoot";
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
			((Unit*)Target)->Hurt( 2.5f * DamageMul );

		SoundManager::GetInstance()->Play( "Rat_Attack", SFX, false );
		return true;
	}

	return false;
}
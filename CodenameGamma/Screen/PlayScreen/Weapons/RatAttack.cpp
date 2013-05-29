#include "RatAttack.h"

RatAttack::RatAttack()
{
	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(1, 1);
	gReloadTime	=	WeaponReloadTime(0.0f, 4.0f);

	gWeaponAnimations.Aim				= "PistolAim";
	gWeaponAnimations.Draw				= "PistolDraw";
	gWeaponAnimations.DrawReloadPutAway = "PistolDrawReloadPutAway";
	gWeaponAnimations.PutAway			= "PistolPutAway";
	gWeaponAnimations.Shoot				= "PistolShoot";
	gWeaponAnimations.Reload		= "PistolReload";
	gWeaponAnimations.UpperWalk			= "PistolUpperWalk";
	gWeaponAnimations.UpperRun			= "PistolUpperRun";
	gWeaponAnimations.UpperStand		= "PistolUpperStand";
}

RatAttack::~RatAttack()
{

}

void RatAttack::ReloadCountdown(float DeltaTime)
{
	if ( gState == Reloading )
	{
		gReloadTime.first	-=	DeltaTime;

		if ( gReloadTime.first <= 0 )
		{
			gClip.first			=	gClip.second;
			gState				=	Ready;
			gReloadTime.first	=	0;
		}
	}
}

bool RatAttack::Fire( GameObject* Instance, float DamageMul )
{
	if ( CanFire() )
	{
		gClip.first	=	0;

		return true;
	}

	return false;
}
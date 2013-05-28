#include "Pistol.h"
#include "../../ScreenEnums.h"

Pistol::Pistol()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Glock" ) );

	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(9, 9);
	gReloadTime	=	WeaponReloadTime(0.0f, 2.5f);

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

Pistol::~Pistol()
{

}

bool Pistol::CanFire()
{
	return (gClip.first > 0 && gCooldown.first <= 0);
}

bool Pistol::Fire( GameObject* Instance, float DamageMul )
{
	if ( CanFire() )
	{
		Bullet*	tBullet	=	new Bullet();
		tBullet->MultiplyDamage( DamageMul );

		float	tRotationY	=	GetFloat3Value( Rotations ).y;

		XMFLOAT3	tVelocity	=	XMFLOAT3(0, 0, 0);
		tVelocity.x	=	-tBullet->GetSpeed() * cos( tRotationY - PI * 0.5f );
		tVelocity.z	=	tBullet->GetSpeed() * sin( tRotationY - PI * 0.5f );

		tBullet->SetRotation( XMFLOAT3( 0, tRotationY, 0 ) );

		XMFLOAT3 pipePos;

		if (GetJointPosition("Pipe", pipePos))
			tBullet->MoveTo( pipePos );
		else
			tBullet->MoveTo( GetFloat3Value( Position ) );

		tBullet->SetVelocity( tVelocity );
		tBullet->SetTeam( GetTeam() );
		tBullet->SetOwner( Instance );

		SoundManager::GetInstance()->Play("Pistol", SFX);

		gCooldown.first	=	gCooldown.second;
		--gClip.first;

		AddGameObject( tBullet );

		return true;
	}
	else if ( gClip.first == 0 && gCooldown.first <= 0)
	{
		SoundManager::GetInstance()->Play("EmptyClip", SFX);
		gCooldown.first	=	gCooldown.second;
	}

	return false;
}
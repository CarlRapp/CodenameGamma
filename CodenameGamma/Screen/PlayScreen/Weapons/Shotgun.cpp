#include "Shotgun.h"
#include "../../ScreenEnums.h"

Shotgun::Shotgun()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Shotgun" ) );

	gCooldown	=	WeaponCooldown(0.0f, 1.0f);
	gClip		=	WeaponClip(8, 8);

	gReloadTime	=	WeaponReloadTime(0.0f, 4.0f);

	gWeaponAnimations.Aim				= "WeaponAim";
	gWeaponAnimations.Draw				= "WeaponDraw";
	gWeaponAnimations.DrawReloadPutAway = "ShotgunDrawReloadPutAway";
	gWeaponAnimations.PutAway			= "WeaponPutAway";
	gWeaponAnimations.Shoot				= "ShotgunShoot";
	gWeaponAnimations.Reload			= "ShotgunReload";
	gWeaponAnimations.UpperWalk			= "WeaponUpperWalk";
	gWeaponAnimations.UpperRun			= "WeaponUpperRun";
	gWeaponAnimations.UpperStand		= "WeaponUpperStand";

	gReloadSound = "Reload";
}

Shotgun::~Shotgun()
{

}

bool Shotgun::CanFire()
{
	return (gClip.first > 0 && gCooldown.first <= 0);
}

bool Shotgun::Fire( GameObject* Owner, GameObject* Target, float DamageMul )
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
		tBullet->MoveTo( GetFloat3Value( Position ) );
		tBullet->SetVelocity( tVelocity );
		tBullet->SetTeam( GetTeam() );
		tBullet->SetOwner( Owner );

		SoundManager::GetInstance()->Play("Shotgun_Fire", SFX);

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
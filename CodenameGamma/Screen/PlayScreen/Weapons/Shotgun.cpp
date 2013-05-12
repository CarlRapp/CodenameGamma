#include "Shotgun.h"
#include "../../ScreenEnums.h"

Shotgun::Shotgun()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Shotgun" ) );

	gCooldown	=	WeaponCooldown(0.0f, 0.4f);
	gClip		=	WeaponClip(8, 8);

	gReloadTime	=	WeaponReloadTime(0.0f, 4.0f);
}

Shotgun::~Shotgun()
{

}

bool Shotgun::CanFire()
{
	return (gClip.first > 0 && gCooldown.first <= 0);
}

vector<Projectile*> Shotgun::Fire()
{
	vector<Projectile*>	tBullets	=	vector<Projectile*>();

	if ( CanFire() )
	{
		Bullet*	tBullet	=	new Bullet();		

		float	tRotationY	=	GetFloat3Value( Rotations ).y;

		XMFLOAT3	tVelocity	=	XMFLOAT3(0, 0, 0);
		tVelocity.x	=	-tBullet->GetSpeed() * cos( tRotationY - PI * 0.5f );
		tVelocity.z	=	tBullet->GetSpeed() * sin( tRotationY - PI * 0.5f );

		tBullet->SetRotation( XMFLOAT3( 0, tRotationY, 0 ) );
		tBullet->MoveTo( GetFloat3Value( Position ) );
		tBullet->SetVelocity( tVelocity );
		tBullet->SetTeam( GetTeam() );

		SoundManager::GetInstance()->Play("Shotgun_Fire");

		gCooldown.first	=	gCooldown.second;
		--gClip.first;

		if ( gClip.first == 0 )
			Reload();

		tBullets.push_back(tBullet);
	}
	else if ( gClip.first == 0 && gCooldown.first <= 0)
	{
		SoundManager::GetInstance()->Play("EmptyClip");
		gCooldown.first	=	gCooldown.second;
	}

	return tBullets;
}
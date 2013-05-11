#include "Pistol.h"
#include "../../ScreenEnums.h"

Pistol::Pistol()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Glock" ) );

	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(5, 9);
	gReloadTime	=	WeaponReloadTime(0.0f, 2.5f);
}

Pistol::~Pistol()
{

}

bool Pistol::CanFire()
{
	return (gClip.first > 0 && gCooldown.first <= 0);
}

vector<Projectile*> Pistol::Fire()
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

		SoundManager::GetInstance()->Play("Pistol");

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
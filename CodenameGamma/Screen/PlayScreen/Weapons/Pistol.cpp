#include "Pistol.h"

Pistol::Pistol()
{
	gCooldown	=	WeaponCooldown(0.0f, 0.5f);
}

Pistol::~Pistol()
{

}

vector<Projectile*> Pistol::Fire()
{
	vector<Projectile*>	tBullets	=	vector<Projectile*>();
	

	for ( int i = 0; i < 7; ++i )
	{
		Bullet*	tBullet	=	new Bullet();
		tBullets.push_back(tBullet);

		float temp	=	PI * 0.125f * 0.333333f * (i - 3);

		XMFLOAT3	tVelocity	=	XMFLOAT3(0, 0, 0);
		tVelocity.x	=	-200.0f * cos( GetFloat3Value( Rotations ).y - PI*0.50f - temp);
		tVelocity.z	=	200.0f * sin( GetFloat3Value( Rotations ).y - PI*0.50f - temp);

		tBullet->MoveTo( GetFloat3Value( Position ) );
		tBullet->SetVelocity( tVelocity );
		tBullet->SetTeam( GetTeam() );
		tBullet->SetScale( XMFLOAT3( 2, 2, 2 ) );
	}

	gCooldown.first	=	gCooldown.second;
	return tBullets;
}
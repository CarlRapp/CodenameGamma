#include "Weapon.h"

Weapon::Weapon(void)
{
	gCooldown	=	WeaponCooldown( 0.0f, 0.0f );
	gClip		=	WeaponClip( 0, 0 );
	gReloadTime	=	WeaponReloadTime( 0.0f, 0.0f );

	gState		=	Ready;
}

Weapon::~Weapon(void)
{

}

void Weapon::LowerCooldown(float DeltaTime)
{
	if ( gCooldown.first > 0 )
	{
		gCooldown.first	-=	DeltaTime;

		gCooldown.first	=	( gCooldown.first <= 0 ) ? 0 : gCooldown.first;
	}
}

void Weapon::ReloadCountdown(float DeltaTime)
{
	if ( gState == Reloading )
	{
		gReloadTime.first	-=	DeltaTime;

		if ( gReloadTime.first <= 0 )
		{
			gClip.first			=	gClip.second;
			gState				=	Ready;
			gReloadTime.first	=	0;

			SoundManager::GetInstance()->Play("Reload");
		}
	}
}

void Weapon::Update(float deltaTime, Terrain* terrain)
{
	LowerCooldown(deltaTime);

	ReloadCountdown(deltaTime);
}

bool Weapon::Fire()
{
	return false;
}

void Weapon::Reload()
{
	if ( gState == Reloading )
		return;
	
	gClip.first			=	0;
	gReloadTime.first	=	gReloadTime.second;
	gState				=	Reloading;
}

bool Weapon::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	return false;
}
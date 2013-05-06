#include "Weapon.h"

Weapon::Weapon(void)
{
	gCooldown	=	WeaponCooldown(0.0f, 0.0f);
}

Weapon::~Weapon(void)
{

}

void Weapon::LowerCooldown(float DeltaTime)
{
	if ( !CanFire() )
	{
		gCooldown.first	-=	DeltaTime;

		gCooldown.first	=	( gCooldown.first < 0 ) ? 0 : gCooldown.first;
	}
}

bool Weapon::Update(float deltaTime, Terrain* terrain)
{
	bool updated = GameObject::Update(deltaTime, terrain);
	LowerCooldown(deltaTime);

	return updated;
}

vector<Projectile*> Weapon::Fire()
{
	gCooldown.first	=	gCooldown.second;

	return vector<Projectile*>();
}
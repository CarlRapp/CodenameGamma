#include "Projectile.h"

Projectile::Projectile(void)
{
	gDamage		=	ProjectileDamage(0.0f, 0.0f);
	gLifeSpan	=	0.0f;

}

Projectile::~Projectile(void)
{

}

bool Projectile::Update(float DeltaTime, Terrain* terrain)
{
	gLifeSpan	-=	DeltaTime;
	if ( gLifeSpan <= 0 )
		SetState( Dead );

	return GameObject::Update(DeltaTime, terrain);
}
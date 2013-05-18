#include "Projectile.h"
#include "../../Units/Unit.h"

Projectile::Projectile(void)
{
	gDamage		=	0.0f;
	gLifeSpan	=	0.0f;
	gOwner		=	0;
}

Projectile::~Projectile(void)
{

}

void Projectile::Update(float DeltaTime, Terrain* terrain)
{
	gLifeSpan	-=	DeltaTime;
	if ( gLifeSpan <= 0 )
		SetState( Dead );

	GameObject::Update(DeltaTime, terrain);
}

bool Projectile::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	return false;
}

void Projectile::SetOwner( GameObject* Instance )
{
	if( !IsOfType<Unit>(Instance) )
	{
		gOwner	=	0;
		return;
	}

	gOwner	=	Instance;
}
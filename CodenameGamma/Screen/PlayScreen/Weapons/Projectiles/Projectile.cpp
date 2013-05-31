#include "Projectile.h"
#include "../../Units/Unit.h"
#include "../../Units/PlayerUnit.h"
#include "../../Structures/StructureList.h"

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
	if (IsEnemy(B))
	{
		
		if ( IsOfType<Unit>(B) )
			return	BoxVsBone(this, B, CD, false);

		if ( IsOfType<Structure>(B) )
			return	SphereVsBone(this, B);
	}

	return false;
}

void Projectile::CollideWith(GameObject* Instance, vector<CollisionData> CD)
{
	if( !IsAlive() )
		return;

	if ( IsOfType<Unit>(Instance) && IsEnemy( Instance ) )
	{
		Unit*	tUnit	=	((Unit*)Instance);

		tUnit->Hurt( gDamage );
		
		if( IsOfType<PlayerUnit>( gOwner ) )
		{
			((PlayerUnit*)gOwner)->GetPlayerScore()->PlayScore	+=	10 * gDamage;

			if( !tUnit->IsAlive() )
				((PlayerUnit*)gOwner)->GetPlayerScore()->PlayKillCount	+=	1;
		}

		SetState(Dead);	
	}

	if ( IsOfType<Structure>(Instance) )
		SetState(Dead);	
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

TargetProjectile::TargetProjectile(void)
{
	gTarget = 0;
}

TargetProjectile::~TargetProjectile(void)
{

}

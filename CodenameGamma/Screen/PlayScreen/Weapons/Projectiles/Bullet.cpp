#include "Bullet.h"
#include "../../Units/Unit.h"
#include "../../Units/PlayerUnit.h"
#include "../../Structures/StructureList.h"

Bullet::Bullet()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Bullet" ) );
	gDamage	=	10.0f;

	gLifeSpan	=	1.5f;
}
Bullet::~Bullet()
{

}

bool Bullet::Intersects(GameObject* B, vector<CollisionData>& CD)
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

void Bullet::CollideWith(GameObject* Instance, vector<CollisionData> CD)
{	
	if( !IsAlive() )
		return;

	if ( IsOfType<Unit>(Instance) )
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

float Bullet::GetSpeed()
{
	return 700.0f;
}
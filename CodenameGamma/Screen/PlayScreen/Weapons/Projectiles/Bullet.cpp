#include "Bullet.h"
#include "../../Units/Unit.h"
#include "../../Structures/StructureList.h"

Bullet::Bullet()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Bullet" ) );
	gDamage	=	ProjectileDamage(1.0f, 1.0f);

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
			return BoxVsBone(this, B, CD, false);

		if ( IsOfType<Structure>(B) )
			return SphereVsBone(this, B);
	}
	return false;
}

void Bullet::CollideWith(GameObject* Instance, vector<CollisionData> CD)
{	
	if ( IsOfType<Unit>(Instance) )
		Instance->SetState(Dead);

	if ( IsOfType<Structure>(Instance) )
		SetState(Dead);	
}

float Bullet::GetSpeed()
{
	return 700.0f;
}
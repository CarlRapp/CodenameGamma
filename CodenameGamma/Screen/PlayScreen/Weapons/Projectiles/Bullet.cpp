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

void Bullet::CollideWith(GameObject* Instance)
{

	if (IsEnemy(Instance))
	{
		if ( IsOfType<Unit>(Instance) )
			Instance->SetState(Dead);

		if ( IsOfType<Structure>(Instance) )
			SetState(Dead);
	}
}

float Bullet::GetSpeed()
{
	return 700.0f;
}
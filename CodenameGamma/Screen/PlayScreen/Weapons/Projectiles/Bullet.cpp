#include "Bullet.h"

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
		if (strcmp(typeid(*Instance).name(), "class Unit") == 0)
			Instance->SetState(Dead);
	}
}
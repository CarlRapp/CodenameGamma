#include "Bullet.h"

Bullet::Bullet()
{
	gDamage	=	ProjectileDamage(1.0f, 1.0f);

	gLifeSpan	=	1.5f;
}
Bullet::~Bullet()
{

}

void Bullet::CollideWith(GameObject* Instance)
{
}
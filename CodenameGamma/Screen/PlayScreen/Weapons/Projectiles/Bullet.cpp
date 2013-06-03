#include "Bullet.h"

Bullet::Bullet()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Bullet" ) );
	gDamage	=	10.0f;

	gLifeSpan	=	1.5f;
}
Bullet::~Bullet()
{

}

#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../../GameObject.h"

using namespace DirectX;

class Projectile : public GameObject
{
protected:
	GameObject*	gOwner;
	float		gLifeSpan;
	float		gDamage;

public:
	Projectile(void);
	virtual ~Projectile(void);

	virtual	void	Update(float DeltaTime, Terrain* terrain);
	virtual	void	CollideWith(GameObject* Instance, vector<CollisionData> CD)	=	0;

	virtual bool	Intersects(GameObject* B, vector<CollisionData>& CD);

	virtual	float	GetSpeed(){	return 0.0f; }

	virtual	void	SetOwner( GameObject* Instance );

	virtual void	MultiplyDamage( float Amount )
	{
		gDamage	*=	Amount;
	};
};

#endif
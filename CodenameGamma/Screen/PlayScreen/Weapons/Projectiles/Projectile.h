#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../../GameObject.h"

using namespace DirectX;

class Projectile : public GameObject
{
	vector<GameObject*> gUnitsHit;

protected:
	GameObject*	gOwner;
	float		gLifeSpan;
	float		gDamage;

	int			gHP;

public:
	Projectile(void);
	virtual ~Projectile(void);

	virtual	void	Update(float DeltaTime, Terrain* terrain);

	virtual	void	CollideWith(GameObject* Instance, vector<CollisionData> CD);
	virtual bool	Intersects(GameObject* B, vector<CollisionData>& CD);

	virtual	float	GetSpeed(){	return 0.0f; }

	virtual	void	SetOwner( GameObject* Instance );

	void			SetHP( int HP ) { gHP = HP; }

	void			SetDamage(float damage) { gDamage = damage; }
	void			SetLifeSpan(float lifeSpan) { gLifeSpan = lifeSpan; }

	virtual void	MultiplyDamage( float Amount )
	{
		gDamage	*=	Amount;
	};
};

class TargetProjectile : public Projectile
{
protected:
		GameObject* gTarget;	

public:
		TargetProjectile(void);
		virtual ~TargetProjectile(void);

		void		SetTarget( GameObject* Target ) { gTarget = Target; }
};

#endif
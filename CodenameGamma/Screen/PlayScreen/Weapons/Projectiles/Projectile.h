#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../../GameObject.h"

using namespace DirectX;

//	Is arranged like <Min, Max>
typedef pair<float, float>	ProjectileDamage;

class Projectile : public GameObject
{
protected:
	float				gLifeSpan;
	ProjectileDamage	gDamage;

public:
	Projectile(void);
	~Projectile(void);

	virtual	void	Update(float DeltaTime, Terrain* terrain);
	virtual	void	CollideWith(GameObject* Instance, vector<CollisionData> CD)	=	0;

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD);

	virtual	float	GetSpeed(){	return 0.0f; }
};

#endif
#pragma once
#ifndef ITEM_H
#define ITEM_H

#include "../GameObject.h"
#include "../Units/Unit.h"
#include "../Units/PlayerUnit.h"
#include "../Graphics/LightHelper.h"

using namespace DirectX;

class Item : public GameObject
{
protected:
	XMFLOAT3	gOffset;
	float		gTimeSpan;
	float		gCooldown;

	PointLight*	gPointLight;
	virtual	void	OnPickUp(Unit* Instance);

public:
	Item(void);
	~Item(void);

	virtual	void	Update(float DeltaTime, Terrain* terrain);

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD);

	void	CollideWith(GameObject* Instance, vector<CollisionData> CD);
};

#endif
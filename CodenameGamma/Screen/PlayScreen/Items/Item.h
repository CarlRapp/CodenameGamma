#pragma once
#ifndef ITEM_H
#define ITEM_H

#include "../GameObject.h"
#include "../Units/Unit.h"
#include "../Units/PlayerUnit.h"

using namespace DirectX;

class Item : public GameObject
{
protected:
	virtual	void	OnPickUp(Unit* Instance);

public:
	Item(void);
	~Item(void);

	virtual	void	Update(float DeltaTime, Terrain* terrain);

	void	CollideWith(GameObject* Instance, vector<CollisionData> CD);
};

#endif
#pragma once
#ifndef CANNEDFOOD_H
#define CANNEDFOOD_H

#include "Item.h"
#include "../Units/Unit.h"

using namespace DirectX;

class CannedFood : public Item
{
public:
	CannedFood(void);
	~CannedFood(void);

	bool	Update(float DeltaTime, Terrain* terrain);
	void	OnPickUp(Unit* Instance);
};

#endif
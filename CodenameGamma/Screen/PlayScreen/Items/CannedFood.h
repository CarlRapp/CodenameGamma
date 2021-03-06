#pragma once
#ifndef CANNEDFOOD_H
#define CANNEDFOOD_H

#include "Item.h"


using namespace DirectX;

class CannedFood : public Item
{
protected:
	void	OnPickUp(Unit* Instance);

public:
	CannedFood(void);
	~CannedFood(void);

	void	Update(float DeltaTime, Terrain* terrain);
};

#endif
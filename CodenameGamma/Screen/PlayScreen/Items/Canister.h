#pragma once
#ifndef CANISTER_H
#define CANISTER_H

#include "Item.h"

using namespace DirectX;

class Canister : public Item
{
protected:
	void	OnPickUp(Unit* Instance);

public:
	Canister(void);
	~Canister(void);

	void	Update(float DeltaTime, Terrain* terrain);
	
};

#endif
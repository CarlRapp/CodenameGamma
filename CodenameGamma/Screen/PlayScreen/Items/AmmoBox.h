#pragma once
#ifndef AMMOBOX_H
#define AMMOBOX_H

#include "Item.h"


using namespace DirectX;

class AmmoBox : public Item
{
protected:
	void	OnPickUp(Unit* Instance);

public:
	AmmoBox(void);
	~AmmoBox(void);

	void	Update(float DeltaTime, Terrain* terrain);
};

#endif
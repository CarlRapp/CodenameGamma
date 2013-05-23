#pragma once
#ifndef MEDIPACK_H
#define MEDIPACK_H

#include "Item.h"


using namespace DirectX;

class MediPack : public Item
{
protected:
	void	OnPickUp(Unit* Instance);

public:
	MediPack(void);
	~MediPack(void);

	void	Update(float DeltaTime, Terrain* terrain);
};

#endif
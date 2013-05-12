#pragma once
#ifndef CANNEDFOOD_H
#define CANNEDFOOD_H

#include "Item.h"
#include "../Units/Unit.h"
#include "../Graphics/LightHelper.h"

using namespace DirectX;

class CannedFood : public Item
{
private:
	XMFLOAT3	gOffset;
	float		gTimeSpan;

	PointLight*	gPointLight;


public:
	CannedFood(void);
	~CannedFood(void);

	bool	Update(float DeltaTime, Terrain* terrain);
	void	OnPickUp(Unit* Instance);
};

#endif
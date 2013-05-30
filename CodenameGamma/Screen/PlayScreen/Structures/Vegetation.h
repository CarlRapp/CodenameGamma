#pragma once
#ifndef VEGETATION_H
#define VEGETATION_H

#include "../GameObject.h"

class Vegetation : public GameObject
{
public:
	Vegetation(void);
	Vegetation(string VegModel);
	~Vegetation(void);

	void	Update(float deltaTime, Terrain* terrain);

	bool	Intersects(GameObject* B, vector<CollisionData>& CD);
};

#endif
#pragma once
#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "../GameObject.h"
#include "Vegetation.h"

using namespace DirectX;

class Structure : public GameObject
{
	Vegetation* gVegetation;

protected:

	string gVegModel;

public:
	Structure(void);
	~Structure(void);

	void Update(float deltaTime, Terrain* terrain);

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD);
	void	CollideWith(GameObject* Instance, vector<CollisionData> CD);

	void	SetState(GOState Value);

	void	SetOvergrown(bool value);
};

#endif
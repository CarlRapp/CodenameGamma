#pragma once
#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "../GameObject.h"

using namespace DirectX;

class Structure : public GameObject
{
public:
	Structure(void);
	~Structure(void);

	virtual bool Intersects(GameObject* B, vector<CollisionData>& CD);
	void	CollideWith(GameObject* Instance, vector<CollisionData> CD);
};

#endif
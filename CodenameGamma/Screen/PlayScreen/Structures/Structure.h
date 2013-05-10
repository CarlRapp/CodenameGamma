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

	void	CollideWith(GameObject* Instance);
};

#endif
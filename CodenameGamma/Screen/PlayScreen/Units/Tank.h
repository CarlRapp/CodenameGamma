#pragma once
#ifndef TANK_H
#define TANK_H

#include "EnemyUnit.h"

using namespace DirectX;

class Tank : public EnemyUnit
{
public:
	Tank(void);
	~Tank(void);
};

#endif
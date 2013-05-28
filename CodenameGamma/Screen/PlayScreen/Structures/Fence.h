#pragma once
#ifndef FENCE_H
#define FENCE_H

#include "Structure.h"

using namespace DirectX;

class Fence : public Structure
{
public:
	Fence(void);
	~Fence(void);
};

#endif
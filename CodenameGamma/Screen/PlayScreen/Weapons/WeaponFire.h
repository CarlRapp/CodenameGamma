#pragma once
#ifndef WEAPONFIRE_H
#define WEAPONFIRE_H

#include "../GameObject.h"

class WeaponFire : public GameObject
{
	float gTimer;
	float gTimerStop;

	float gMinScale;
	float gMaxScale;

	GameObject* gOwner;

public:
	WeaponFire(void);
	WeaponFire(float timer, float minScale, float maxScale, GameObject* owner);
	~WeaponFire(void);

	void Update(float deltaTime, Terrain* terrain);

	bool Intersects(GameObject* B, vector<CollisionData>& CD) { return false; }
};

#endif

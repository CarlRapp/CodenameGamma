#pragma once
#ifndef PLAYERUNIT_H
#define PLAYERUNIT_H

#include "Unit.h"


#define TRAIL_INTERVAL 0.1f
#define TRAIL_LIFETIME 5.0f

//	Is arranged like <Current, Max>
typedef pair<float, float>	UnitHunger;
typedef pair<float, float>	UnitThirst;

struct Trail
{
	XMFLOAT3 position;
	
	float time;
	float stopTime;

	Trail() : time(0.0f), stopTime(0.0f) { }

	Trail(XMFLOAT3 p, float t, float st) : position(p), time(t), stopTime(st) { }

	void Update(float deltaTime) { time += deltaTime; }
	bool IsAlive() { return time < stopTime; }

	bool operator==(Trail b)
	{
		XMVECTOR A = XMLoadFloat3(&position);
		XMVECTOR B = XMLoadFloat3(&b.position);
		return XMVector3Equal(A, B) && time == b.time;
	}

};

class PlayerUnit : public Unit
{
	vector<Trail> gTrails;

	float trailTimer;

	void UpdateTrail(float deltaTime);

protected:
	UnitHunger	gHunger;
	UnitThirst	gThirst;

	void	UpdateMeters(float DeltaTime);
public:
	PlayerUnit(void);
	~PlayerUnit(void);

	void	Update(float deltaTime, Terrain* terrain);

	UnitHunger	GetHungerMeter()	{ return gHunger; }
	UnitThirst	GetThirstMeter()	{ return gThirst; }

	void	Hurt(float Damage);

	void	Eat(float Value)
	{
		gHunger.first	+=	Value;
		gHunger.first	=	(gHunger.first > gHunger.second) ? gHunger.second : gHunger.first;
	}
	void	Drink(float Value)
	{
		gThirst.first	+=	Value;
		gThirst.first	=	(gThirst.first > gThirst.second) ? gThirst.second : gThirst.first;
	}

	void	SetVelocity(XMFLOAT3 Velocity);

	vector<XMFLOAT3> GetTrails() 
	{ 
		vector<XMFLOAT3> trails;

		for (int i = gTrails.size() - 1; i >= 0; --i)
			trails.push_back(gTrails[i].position);

		return trails;
	}

};

#endif
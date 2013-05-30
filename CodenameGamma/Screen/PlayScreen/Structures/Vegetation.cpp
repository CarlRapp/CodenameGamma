#include "Vegetation.h"


Vegetation::Vegetation(void)
{
	SetTeam( Neutral );	
}

Vegetation::Vegetation(string VegModel)
{
	SetTeam( Neutral );
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( VegModel ) );
}

Vegetation::~Vegetation(void)
{
}

void Vegetation::Update(float deltaTime, Terrain* terrain)
{
	return;
}

bool Vegetation::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	return false;
}

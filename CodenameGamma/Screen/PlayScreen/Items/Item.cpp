#include "Item.h"

Item::Item()
{
	SetTeam( Neutral );
}

Item::~Item()
{

}

void Item::Update(float DeltaTime, Terrain* terrain)
{
	GameObject::Update(DeltaTime, terrain);
}

bool Item::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	if ( IsOfType<Unit>(B) )
		return BoxVsBone(this, B, CD, false);
	return false;
}

void Item::CollideWith(GameObject* Instance, vector<CollisionData> CD)
{
	if ( !IsOfType<Unit>(Instance) )
		return;


	OnPickUp( ((Unit*)Instance) );
}

void Item::OnPickUp(Unit* Instance)
{

}
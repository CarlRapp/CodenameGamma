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

void Item::CollideWith(GameObject* Instance, vector<CollisionData> CD)
{
	if ( !IsOfType<Unit>(Instance) )
		return;


	OnPickUp( ((Unit*)Instance) );
}

void Item::OnPickUp(Unit* Instance)
{

}
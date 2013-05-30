#include "Structure.h"
#include "../Units/Unit.h"
#include "../Units/EnemyUnit.h"
#include "UnitCube.h"

Structure::Structure()
{
	gVegModel = "";
	SetTeam( Neutral );

	gVegetation = NULL;
}

Structure::~Structure()
{
}

void Structure::Update(float deltaTime, Terrain* terrain)
{
	GameObject::Update(deltaTime, terrain);

	if (gVegetation)
		gVegetation->SetWorld( GetFloat4x4Value( Translation ), GetFloat4x4Value( Scale ), GetFloat4x4Value( Rotation ) );
}

bool Structure::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	if ( IsOfType<Unit>(B) && !IsOfType<EnemyUnit>(B) )
		return BoxVsAllBones(B, this, CD, true);
	return false;
}

void Structure::CollideWith(GameObject* Instance, vector<CollisionData> CD)
{
	if ( IsOfType<Unit>(Instance) && !IsOfType<EnemyUnit>(Instance) )
	{
		for each (CollisionData cd in CD)
		{
			float Radius = Instance->GetModelInstance()->GetSmallestRaduisInBox();
			XMFLOAT3	CenterPos	=	Instance->GetFloat3Value( Position );

			MathHelper::CollisionResult		Result	=	MathHelper::CheckCollision(cd.A.box, BoundingSphere(CenterPos, Radius));

			if ( Result.IsColliding )
			{
				XMFLOAT3	deltaMove;
				XMStoreFloat3(
					&deltaMove, 
					Result.PenetrationLength * XMLoadFloat3( &Result.Normal )
				);

				Instance->Move( deltaMove );
			}
		}
		/*
		//cout << "Stucture hit " << CD.B.name << endl;

		//XMFLOAT3	Extents		=	Instance->GetModelInstance()->GetBoundingOrientedBox().Extents;
		//float		Radius		=	( Extents.x > Extents.z ) ? Extents.z : Extents.x;
		float Radius = Instance->GetModelInstance()->GetSmallestRaduisInBox();
		XMFLOAT3	CenterPos	=	Instance->GetFloat3Value( Position );

		MathHelper::CollisionResult		Result	=	MathHelper::CheckCollision(GetModelInstance()->GetBoundingOrientedBox(), BoundingSphere(CenterPos, Radius));
		//MathHelper::CollisionResult		Result	=	MathHelper::CheckCollision(CD.A.box, BoundingSphere(CenterPos, Radius));

		if ( Result.IsColliding )
		{
			XMFLOAT3	deltaMove;
			XMStoreFloat3(
				&deltaMove, 
				Result.PenetrationLength * XMLoadFloat3( &Result.Normal )
			);

			Instance->Move( deltaMove );
		}
		*/
	}
}

void Structure::SetState(GOState Value)
{
	GameObject::SetState( Value );

	if (gVegetation)
		gVegetation->SetState( Value );
}

void Structure::SetOvergrown(bool value)
{
	if (value)
	{
		if (!gVegetation)
		{
			if (gVegModel != "")
			{
				gVegetation = new Vegetation( gVegModel );
				AddGameObject( gVegetation );
			}
		}
	}
	else
	{
		if (gVegetation)
		{
			gVegetation->SetState( Dead );
			gVegetation = NULL;
		}
	}
}
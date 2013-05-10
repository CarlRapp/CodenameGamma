#include "Structure.h"
#include "../Units/Unit.h"
#include "UnitCube.h"

Structure::Structure()
{

}

Structure::~Structure()
{

}

void Structure::CollideWith(GameObject* Instance)
{
	if ( IsOfType<Unit>(Instance) )
	{
		XMFLOAT3	Extents		=	Instance->GetModelInstance()->GetBoundingOrientedBox().Extents;
		float		Radius		=	( Extents.x > Extents.z ) ? Extents.z : Extents.x;
		XMFLOAT3	CenterPos	=	Instance->GetFloat3Value( Position );

		MathHelper::CollisionResult		Result	=	MathHelper::CheckCollision(GetModelInstance()->GetBoundingOrientedBox(), BoundingSphere(CenterPos, Radius));

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
}
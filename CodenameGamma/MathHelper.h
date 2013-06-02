#pragma once
#ifndef MATHHELPERR_H
#define MATHHELPERR_H

#include <Windows.h>
//#include <D3DX10math.h>

//#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace DirectX;

class MathHelper
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	 
	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b-a)*t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x); 
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	static XMFLOAT4 GetRotationQuaternion(XMFLOAT3 v1, XMFLOAT3 v2)
	{
		XMVECTOR quat = XMQuaternionIdentity();

		XMVECTOR V1 = XMLoadFloat3(&v1);
		XMVECTOR V2 = XMLoadFloat3(&v2);

		XMVECTOR angleV = XMVector3AngleBetweenVectors(V1, V2);
		float angle;
		XMStoreFloat(&angle, angleV);

		if (angle != 0)
		{
			XMVECTOR axis = XMVector3Cross(V1, V2);
			quat = XMQuaternionRotationAxis(axis, angle);
		}

		XMFLOAT4 result;
		XMStoreFloat4(&result, quat);

		return result;
	}

	static BoundingFrustum GenerateBoundingFrustum(CXMMATRIX View, CXMMATRIX Proj)
	{
		XMVECTOR det		= XMMatrixDeterminant(View);
		XMMATRIX invView	= XMMatrixInverse(&det, View);

		BoundingFrustum output = BoundingFrustum(Proj);
		output.Transform(output, invView);

		return output;
	}

	static bool BoundingSphereEqual(BoundingSphere A, BoundingSphere B)
	{
		XMVECTOR AC = XMLoadFloat3(&A.Center);
		XMVECTOR BC = XMLoadFloat3(&B.Center);

		return XMVector3Equal(AC, BC) && A.Radius == B.Radius;
	}

	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	static XMFLOAT3 GetClosestSidePoint(BoundingOrientedBox &OBB, XMFLOAT3 &Pos);
	/*
	static D3DXMATRIX InverseTranspose(const D3DXMATRIX& M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
	
		D3DXMATRIX A = M;
		A._31 = 0.0f;
		A._32 = 0.0f;
		A._33 = 0.0f;
		A._34 = 1.0f;
		
		float det = D3DXMatrixDeterminant(&A);

		D3DXMatrixInverse(&A, &det, &A);
		D3DXMatrixTranspose(&A, &A);
		
		return A;
	}
	
	
	static XMMATRIX D3DXMatrixToXMMatrix(const D3DXMATRIX& M)
	{
		return DirectX::XMMATRIX(	M._11, M._12, M._13, M._14,
									M._21, M._22, M._23, M._24,
									M._31, M._32, M._33, M._34,
									M._41, M._42, M._43, M._44);
	}
	*/
	
	//static D3DXVECTOR3 RandUnitVec3();
	//static D3DXVECTOR3 RandHemisphereUnitVec3(D3DXVECTOR3 n);
	

	static const float Infinity;
	static const float Pi;


	struct CollisionResult
	{
		CollisionResult()
		{
			IsColliding	=	false;
		}
		bool		IsColliding;
		XMFLOAT3	Normal;
		float		PenetrationLength;
	};

	static CollisionResult CheckCollision(BoundingOrientedBox OBB, BoundingSphere Sphere)
	{
		//	Create our initial result that will
		//	be returned
		CollisionResult	Result	=	CollisionResult();

		//	
		//	First we need to make this collision
		//	into a Sphere VS AAB instead of Sphere vs OBB
		//

		//	Transform the quaternion
		//	into a matrix
		XMMATRIX	rot	=	XMMatrixRotationQuaternion( XMLoadFloat4( &OBB.Orientation ) );

		//	Get the determinant from
		//	the rotation matrix and
		//	inverse it
		XMVECTOR	det =	XMMatrixDeterminant( rot );
					rot	=	XMMatrixInverse( &det, rot);

		//	Get the make a temporary
		//	scale and translation
		//	and use it to make a 
		//	transformation matrix
		XMVECTOR	scale	=	XMLoadFloat3(&XMFLOAT3(1,1,1));
		XMVECTOR	quat	=	XMQuaternionRotationMatrix(rot);
		XMVECTOR	trans	=	XMLoadFloat3(&XMFLOAT3(0, 0, 0));
 
		//	Create the transformation
		//	matrix from our rotation
		XMMATRIX transf = XMMatrixAffineTransformation( scale, XMLoadFloat3(&OBB.Center), quat, trans );


		//	Now save our transformed
		//	sphere and AAB
		BoundingSphere TransformedSphere;
		Sphere.Transform(TransformedSphere, transf);
		BoundingBox	AAB	=	BoundingBox(OBB.Center, OBB.Extents);

		//	Clamp the both shapes to the
		//	ground
		AAB.Center.y				=	0;
		TransformedSphere.Center.y	=	0;

		//	We now have our transformed 
		//	objects so now we can do
		//	the collision calculations

		//	First get the vector that
		//	is from the center of the
		//	box to the center of the sphere
		XMFLOAT3	distanceVector, closestVector;
		XMStoreFloat3(
			&distanceVector, 
			XMLoadFloat3( &TransformedSphere.Center ) - XMLoadFloat3( &AAB.Center )
		);

		//	Set this as our closest vector
		//	between the two objects
		closestVector	=	distanceVector;

		//	Now clamp the values to be on
		//	the box bounds
		closestVector.x	=	MathHelper::Clamp( closestVector.x, -AAB.Extents.x, AAB.Extents.x );
		closestVector.z	=	MathHelper::Clamp( closestVector.z, -AAB.Extents.z, AAB.Extents.z );

		bool	inside	=	false;

		//	If closestVector still is
		//	same it means that we are inside
		//	the box and we now need to
		//	calculate how to resolve this collision
		if ( closestVector.x  == distanceVector.x && closestVector.z == distanceVector.z )
		{
			inside	=	true;

			if ( abs( distanceVector.x ) > abs( distanceVector.z ) )
			{
				if ( closestVector.x > 0 ) 
					closestVector.x	=	AAB.Extents.x;
				else
					closestVector.x	=	-AAB.Extents.x;
			}
			else
			{
				if ( closestVector.z > 0 ) 
					closestVector.z	=	AAB.Extents.z;
				else
					closestVector.z	=	-AAB.Extents.z;
			}
		}

		//	Now calculate the collision
		//	normal.
		XMFLOAT3	Normal;
		XMStoreFloat3(
			&Normal, 
			XMLoadFloat3( &distanceVector ) - XMLoadFloat3( &closestVector )
		);

		//	Calculate the length
		//	of the intersection
		float	d;
		XMStoreFloat(
			&d, 
			XMVector3Length( XMLoadFloat3( &Normal ) )
		);
		d	*=	d;

		//	We might have missed so we need
		//	to check if we still are within
		//	the bounds of our sphere
		if ( d > TransformedSphere.Radius * TransformedSphere.Radius && !inside )
		{
			Result.IsColliding	=	false;
			return Result;
		}

		//	At this point we
		//	are sure that there
		//	is a collision, now
		//	calculate the resolving
		//	vector
		d	=	sqrt( d );
		if ( inside )
		{
			XMStoreFloat3(
				&Normal, 
				-1 * XMLoadFloat3( &Normal )
			);
		}
		else
		{
			XMStoreFloat3(
				&Normal, 
				XMLoadFloat3( &Normal )
			);
		}
		
		//	Now rotate the normal back to
		//	the "original space".
		XMVECTOR NormalV = XMLoadFloat3(&Normal);

		XMMATRIX tRot = XMMatrixRotationQuaternion(XMLoadFloat4(&OBB.Orientation));

		NormalV = XMVector3TransformCoord(NormalV, tRot);
		XMStoreFloat3(
			&Normal, 
			XMVector3Normalize( NormalV )
		);
		
		Normal.y					=	0;
		Result.PenetrationLength	=	abs( d - TransformedSphere.Radius );
		Result.Normal				=	Normal;
		Result.IsColliding			=	true;
		return Result;
	}
};

/*
	Funktion som beräknar den närmsta
	punkten på en lådas 6 plan och 
	returnerar informationen.
struct CSPResult
{
	CSPResult(void){}

	float		Length;
	XMFLOAT3	Position;
	XMFLOAT3	PlaneNormal;
};

XMFLOAT3 GetClosestPointOnPlane(XMFLOAT4 &Plane, XMFLOAT3 &Pos)
{
	XMFLOAT3	tPointOnPlane;
	XMFLOAT3	tNormal	=	XMFLOAT3( Plane.x, Plane.y, Plane.z );
	XMVECTOR	pA, pB, tResult;
	pA	=	XMLoadFloat3( &Pos );
	pB	=	XMLoadFloat3( &tNormal );
	pB	+=	pA;
	
	tResult	=	XMPlaneIntersectLine(
					XMLoadFloat4( &Plane ),
					pA,
					pB
				);

	XMStoreFloat3( &tPointOnPlane, tResult ); 

	return tPointOnPlane;
}

CSPResult GetClosestSidePoint(BoundingOrientedBox &OBB, XMFLOAT3 &Pos)
{
			
	//		IMPORTANT
	//	All the normals will
	//	be facing outwards 
	//	relative to the box
	
	XMFLOAT3 cornersW[OBB.CORNER_COUNT];
	OBB.GetCorners(&cornersW[0]);

	//	Needed variables for 
	//	this method
	XMFLOAT3			fResult, fNormal;
	float				fLength;
	XMVECTOR			tempPlane;
	XMFLOAT4			tempAffine;
	vector<XMFLOAT4>	tPlanes	=	vector<XMFLOAT4>();

	//	"Back"
	tempPlane	=	XMPlaneFromPoints( XMLoadFloat3( &cornersW[4] ), XMLoadFloat3( &cornersW[7] ), XMLoadFloat3( &cornersW[5] ) );
	XMStoreFloat4( &tempAffine, tempPlane );
	tPlanes.push_back( tempAffine );

	//	"Right"
	tempPlane	=	XMPlaneFromPoints( XMLoadFloat3( &cornersW[5] ), XMLoadFloat3( &cornersW[6] ), XMLoadFloat3( &cornersW[1] ) );
	XMStoreFloat4( &tempAffine, tempPlane );
	tPlanes.push_back( tempAffine );

	//	"Front"
	tempPlane	=	XMPlaneFromPoints( XMLoadFloat3( &cornersW[1] ), XMLoadFloat3( &cornersW[2] ), XMLoadFloat3( &cornersW[0] ) );
	XMStoreFloat4( &tempAffine, tempPlane );
	tPlanes.push_back( tempAffine );

	//	"Left" 
	tempPlane	=	XMPlaneFromPoints( XMLoadFloat3( &cornersW[0] ), XMLoadFloat3( &cornersW[3] ), XMLoadFloat3( &cornersW[4] ) );
	XMStoreFloat4( &tempAffine, tempPlane );
	tPlanes.push_back( tempAffine );

	//	"Up"
	tempPlane	=	XMPlaneFromPoints( XMLoadFloat3( &cornersW[3] ), XMLoadFloat3( &cornersW[2] ), XMLoadFloat3( &cornersW[7] ) );
	XMStoreFloat4( &tempAffine, tempPlane );
	//tPlanes.push_back( tempAffine );

	//	"Down"
	tempPlane	=	XMPlaneFromPoints( XMLoadFloat3( &cornersW[4] ), XMLoadFloat3( &cornersW[5] ), XMLoadFloat3( &cornersW[0] ) );
	XMStoreFloat4( &tempAffine, tempPlane );
	//tPlanes.push_back( tempAffine );

	//	Just set the start value to be the 
	//	first value from the list
	XMFLOAT3	tResult;
	float		tLength;

	fResult	=	GetClosestPointOnPlane( tPlanes[0], Pos );
	fNormal	=	XMFLOAT3( tPlanes[0].x, tPlanes[0].y, tPlanes[0].z );
	XMStoreFloat(
		&fLength, 
		XMVector3Length( XMLoadFloat3( &Pos ) - XMLoadFloat3( &fResult ) )
	);

	//	Loop through each plane
	for each ( XMFLOAT4 tAffineForm in tPlanes )
	{
		XMFLOAT3	tResult	=	GetClosestPointOnPlane( tAffineForm, Pos );

		//	Get the length of
		//	the vector
		XMStoreFloat(
			&tLength, 
			XMVector3Length( XMLoadFloat3( &Pos ) - XMLoadFloat3( &tResult ) )
		);

		if ( abs(tLength) < abs(fLength) )
		{
			fResult	=	tResult;
			fLength	=	tLength;
			fNormal	=	XMFLOAT3( tAffineForm.x, tAffineForm.y, tAffineForm.z );
		}
	}

	CSPResult	Result	=	CSPResult();
	Result.Length		=	fLength;
	Result.Position		=	fResult;
	Result.PlaneNormal	=	fNormal;

	return Result;
}
*/
#endif

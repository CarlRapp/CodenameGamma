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


};

#endif

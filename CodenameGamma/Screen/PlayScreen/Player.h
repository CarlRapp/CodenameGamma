#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "..\..\Input\Controller.h"
#include "Camera.h"

class Player
{
private:
	Camera		*m_Camera;
	Controller	*m_Controller;

	void UpdateCamera(float deltaTime);

public:
	Player(void);
	Player(int index);
	~Player(void);

	void Update(float deltaTime);

	void SetScreen(int X, int Y, int Width, int Height) 
	{ 
		m_Camera->SetViewPort(X, Y, Width, Height); 
		m_Camera->SetAspectRatio( (float)Width / (float)Height );
	}

	Camera*		GetCamera()		{ return m_Camera; }
	Controller*	GetController()	{ return m_Controller; }
};

#endif
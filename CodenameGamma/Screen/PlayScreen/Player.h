#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "../../Input/InputManager.h"
#include "../../Input/InputBinds.h"
#include "../Score/Score.h"
#include "Units/PlayerUnit.h";
#include "Camera.h"

class Player
{
private:
	PlayerScore	*gPlayerScore;
	PlayerUnit	*m_Unit;
	Camera		*m_Camera;
	Controller	*m_Controller;
	UINT		m_PlayerIndex;

	void	UpdateCamera(float deltaTime);

	bool	IsButtonState( XboxBinds Key, InputState State );
	bool	IsButtonState( KeyboardBinds Key, InputState State );
	bool	IsButtonState( MouseBinds Key, InputState State );

public:
	Player(void);
	Player(int index);
	~Player(void);

	void	Update(float deltaTime);

	void SetScreen(int X, int Y, int Width, int Height) 
	{ 
		m_Camera->SetViewPort(X, Y, Width, Height); 
		m_Camera->SetAspectRatio( (float)Width / (float)Height );
	}

	Camera*		GetCamera()		{ return m_Camera; }
	Controller*	GetController()	{ return m_Controller; }
	PlayerUnit*	GetUnit()		{ return m_Unit; }

	void		SetUnit(PlayerUnit* Instance);

	PlayerScore*	GetPlayerScore()
	{ 
		return gPlayerScore; 
	}

	bool IsAlive() 
	{ 
		if (m_Unit) 
			return m_Unit->IsAlive();
		return false;
	}
	
	int	GetIndex(){ return m_PlayerIndex; }
};

#endif
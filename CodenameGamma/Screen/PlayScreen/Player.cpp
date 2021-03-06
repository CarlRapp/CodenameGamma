#include "Player.h"


Player::Player(void)
{
	m_Controller	=	InputManager::GetInstance()->GetController(0);
	m_Camera		=	new Camera();
	m_Unit			=	NULL;
	m_PlayerIndex	=	0;
	gPlayerScore	=	new PlayerScore();

	gMasterOfUnit	=	false;
}

Player::Player(int index)
{
	m_Controller	=	InputManager::GetInstance()->GetController(index);
	m_Camera		=	new Camera();
	m_Unit			=	NULL;
	m_PlayerIndex	=	index;
	gPlayerScore	=	new PlayerScore( index );

	m_Camera->SetPosition(2000, 100, 500);
}

Player::~Player(void)
{
	delete m_Camera;
}

void Player::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

	if ( m_Unit != 0 && gMasterOfUnit)
	{
		//XBOX-CONTROL
		XMFLOAT3	newVel	=	XMFLOAT3(0, 0, 0);
		XMFLOAT3	tPosition	=	m_Unit->GetFloat3Value(Position);

		XMFLOAT2	rStickDir		=	m_Controller->GetStickDirection( RIGHT );

		if ( m_Controller->GetStickLength( RIGHT ) > 0.3f && m_Unit->IsAlive())
			m_Unit->LookAtXZ(XMFLOAT3(tPosition.x + rStickDir.x, 0, tPosition.z + rStickDir.y));

		XMFLOAT2	lStickDir		=	m_Controller->GetStickDirection( LEFT );
		float		lStickLength	=	m_Controller->GetStickLength( LEFT ) * m_Unit->GetWalkSpeed() * UnitsPerMeter;

		if (IsButtonState( XAim, UP ) && m_Unit->CanRun())
		{
			float walkSpeed = m_Unit->GetWalkSpeed();
			float runSpeed = m_Unit->GetRunSpeed();

			lStickLength				*=	1.0f + ( (runSpeed - walkSpeed) / walkSpeed) * m_Controller->GetTriggerValue( RIGHT );
		}

		newVel	=	XMFLOAT3(lStickDir.x * lStickLength, 0, lStickDir.y * lStickLength);

		if (m_Unit->Crouching())
			m_Unit->SetVelocity(XMFLOAT3(0, 0, 0));
		else
			m_Unit->SetVelocity(newVel);
		

		switch (m_Controller->GetButtonState( (Xbox_Button)XAim ) )
		{
		case PRESSED:
			m_Unit->SetWeaponState(Unit::Aim);
			break;
		case DOWN:
			if ( IsButtonState( XShoot, DOWN ) )
			{
				if( m_Unit->FireWeapon(NULL) )
					InputManager::GetInstance()->GetController( m_PlayerIndex )->Vibrate( 1.0f, 1.0f, 0.1f );
			}
			break;
		case RELEASED:
			m_Unit->SetWeaponState(Unit::Hold);
			break;
		case UP:
			XMVECTOR lStickDirV = XMLoadFloat2(&lStickDir);
			float length = XMVectorGetX( XMVector2Length(lStickDirV) );

			if (m_Unit->IsAlive() && (length > 0.05f && m_Controller->GetTriggerValue( RIGHT ) > 0.3f || m_Unit->IsRunning()))
			{
				m_Unit->LookAtXZ(XMFLOAT3(tPosition.x + lStickDir.x, 0, tPosition.z + lStickDir.y));
			}
			break;
		}

		if ( IsButtonState( XCrouch, PRESSED) )
			m_Unit->SwitchCrouchStand();

		if ( IsButtonState( XReload, PRESSED ) )
			m_Unit->ReloadWeapon();

		if ( IsButtonState( XDrop, PRESSED ) )
			m_Unit->DropWeapon();

		if ( IsButtonState( XChangeWeapon, PRESSED ) )
			m_Unit->ChangeWeapon();
	}

	if ( m_Unit != 0 )
	{
		XMFLOAT3 tPosition = m_Unit->GetFloat3Value(Position);
		m_Camera->SetPosition(tPosition.x, tPosition.y + 300, tPosition.z - 100);
		m_Camera->SetLookAt(tPosition);
	}
}

void Player::UpdateCamera(float deltaTime)
{
	//Move Camera
	XMVECTOR cameraPosV = XMLoadFloat3(&m_Camera->GetPosition());
	XMVECTOR cameraFor = XMLoadFloat3(&m_Camera->GetForward());
	XMVECTOR cameraRig = XMLoadFloat3(&m_Camera->GetRight());

	XMFLOAT2 stickleft = m_Controller->GetStickDirection(LEFT);

	cameraPosV += stickleft.y * cameraFor * deltaTime * 300;
	cameraPosV += stickleft.x * cameraRig * deltaTime * 300;

	XMFLOAT3 cameraPos;
	XMStoreFloat3(&cameraPos, cameraPosV);

	m_Camera->SetPosition(cameraPos);

	//Change Look-Direction
	XMFLOAT2 stickright = m_Controller->GetStickDirection(RIGHT);

	float dx = stickright.x * deltaTime * 4;
	float dy = stickright.y * deltaTime * 4;

	m_Camera->Rotate(-dy, dx);
}

void Player::SetUnit(PlayerUnit* Instance)
{
	m_Unit			=	Instance;
	gMasterOfUnit	= true;
}

void Player::Spectate(Player* player)
{
	if (!player)
		return;
	m_Unit			= player->GetUnit();
	m_SpectateIndex = player->GetIndex();
	gMasterOfUnit	= false;
}

#pragma region Player controller methods
bool Player::IsButtonState( XboxBinds Key, InputState State )
{
	return m_Controller->GetButtonState( (Xbox_Button)Key ) == State;
}
bool Player::IsButtonState( KeyboardBinds Key, InputState State )
{
	return m_PlayerIndex == 0 && InputManager::GetInstance()->GetKeyboard()->GetKeyState( Key ) == State;
}
bool Player::IsButtonState( MouseBinds Key, InputState State )
{
	return m_PlayerIndex == 0 && InputManager::GetInstance()->GetMouse()->GetButtonState( (MouseButton)Key ) == State;
}
#pragma endregion
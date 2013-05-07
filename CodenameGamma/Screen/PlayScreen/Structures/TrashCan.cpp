#include "TrashCan.h"

TrashCan::TrashCan()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TrashCan" ) );
	SetScale( XMFLOAT3( 2, 2, 2 ) );
}

TrashCan::~TrashCan()
{

}
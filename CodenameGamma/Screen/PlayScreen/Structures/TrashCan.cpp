#include "TrashCan.h"

TrashCan::TrashCan()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TrashCan" ) );
}

TrashCan::~TrashCan()
{

}
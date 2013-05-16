#include "Rat.h"

Rat::Rat()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "CrazyBitch" ) );
}

Rat::~Rat()
{

}
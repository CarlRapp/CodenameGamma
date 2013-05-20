#include "Lamp.h"

Lamp::Lamp()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Lamp" ) );
}

Lamp::~Lamp()
{

}
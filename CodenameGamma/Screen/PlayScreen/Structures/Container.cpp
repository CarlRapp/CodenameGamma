#include "Container.h"

Container::Container()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Container" ) );
}

Container::~Container()
{

}
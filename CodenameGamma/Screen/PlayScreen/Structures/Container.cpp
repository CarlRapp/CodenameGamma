#include "Container.h"

Container::Container()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Container" ) );

	gVegModel = "Container_VEG";
}

Container::~Container()
{

}
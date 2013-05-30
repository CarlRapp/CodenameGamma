#include "Lamp.h"

Lamp::Lamp()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Lamp" ) );
	
	gVegModel = "Lamp_VEG";
}

Lamp::~Lamp()
{

}
#include "AstonMartin.h"

AstonMartin::AstonMartin()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "AstonMartin" ) );
	
	gVegModel = "AstonMartin_VEG";
}

AstonMartin::~AstonMartin()
{

}
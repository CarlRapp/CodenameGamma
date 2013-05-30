#include "UtilityPole.h"

UtilityPole::UtilityPole()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "UtilityPole" ) );
	
	gVegModel = "UtilityPole_VEG";
}

UtilityPole::~UtilityPole()
{

}
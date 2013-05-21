#include "UtilityPole.h"

UtilityPole::UtilityPole()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "UtilityPole" ) );
}

UtilityPole::~UtilityPole()
{

}
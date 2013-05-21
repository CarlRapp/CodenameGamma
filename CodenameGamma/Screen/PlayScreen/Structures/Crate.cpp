#include "Crate.h"

Crate::Crate()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Crate" ) );
}

Crate::~Crate()
{

}
#include "SmallStore.h"

SmallStore::SmallStore()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SmallStore" ) );
}

SmallStore::~SmallStore()
{

}
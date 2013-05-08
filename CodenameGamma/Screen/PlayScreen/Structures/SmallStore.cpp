#include "SmallStore.h"

SmallStore::SmallStore()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SmallStore" ) );
	SetScale( 8 );
}

SmallStore::~SmallStore()
{

}
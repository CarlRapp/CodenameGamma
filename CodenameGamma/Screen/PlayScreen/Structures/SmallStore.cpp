#include "SmallStore.h"

SmallStore::SmallStore()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SmallStore" ) );

	gVegModel = "SmallStore_VEG";
}

SmallStore::~SmallStore()
{

}
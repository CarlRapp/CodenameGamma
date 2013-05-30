#include "MarketStore.h"

MarketStore::MarketStore()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "MarketStore" ) );

	gVegModel = "MarketStore_VEG";
}

MarketStore::~MarketStore()
{

}
#include "MarketStore.h"

MarketStore::MarketStore()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "MarketStore" ) );
}

MarketStore::~MarketStore()
{

}
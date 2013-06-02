#include "BarbedWire.h"

BarbedWire::BarbedWire()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "BarbedWire" ) );

	gVegModel = "BarbedWire_VEG";
}

BarbedWire::~BarbedWire()
{

}
#include "ApartmentComplex.h"

ApartmentComplex::ApartmentComplex()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "ApartmentComplex" ) );
}

ApartmentComplex::~ApartmentComplex()
{

}
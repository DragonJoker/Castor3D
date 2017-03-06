#include "ComRasteriserState.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED_STATE = cuT( "The blend state must be initialised" );

	CRasteriserState::CRasteriserState()
	{
	}

	CRasteriserState::~CRasteriserState()
	{
	}
}

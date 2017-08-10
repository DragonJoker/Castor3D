#include "ComRasteriserState.hpp"
#include "ComLogger.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED_STATE = cuT( "The blend state must be initialised" );

	CRasteriserState::CRasteriserState()
	{
	}

	CRasteriserState::~CRasteriserState()
	{
	}
}

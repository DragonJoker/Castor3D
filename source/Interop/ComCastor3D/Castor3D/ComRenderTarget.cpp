#include "ComCastor3D/Castor3D/ComRenderTarget.hpp"

namespace CastorCom
{
	STDMETHODIMP CRenderTarget::Initialise()noexcept
	{
		if ( m_internal )
			return dispatchUninitialised( _T( "Initialise" ) );

		return convert( c3dRenderTarget_initialise( m_internal ) );
	}

	STDMETHODIMP CRenderTarget::Cleanup()noexcept
	{
		if ( m_internal )
			return dispatchUninitialised( _T( "Cleanup" ) );

		return convert( c3dRenderTarget_cleanup( m_internal ) );
	}
}

#include "ComCastor3D/CastorUtils/ComPixelBuffer.hpp"

namespace CastorCom
{
	STDMETHODIMP CPixelBuffer::Create( /*[in]*/ ISize * size, /*[in]*/ ePIXEL_FORMAT format )noexcept
	{
		if ( !size )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dPixelBuffer_create( &static_cast< CSize * >( size )->getInternal()
			, C3D_PIXEL_FORMAT( format )
			, &m_internal ) );
	}
}

#include "ComCastor3D/CastorUtils/ComImage.hpp"
#include "ComCastor3D/CastorUtils/ComRgbaColour.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	STDMETHODIMP CImage::Resample( /*[in]*/ ISize * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "Resample" ) );

		return convert( c3dImage_resample( m_internal
			, &static_cast< CSize * >( val )->getInternal() ) );
	}

	STDMETHODIMP CImage::Fill( /*[in]*/ IRgbaColour * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "Fill" ) );

		return convert( c3dImage_fill( m_internal
			, &static_cast< CRgbaColour * >( val )->getInternal() ) );
	}
}

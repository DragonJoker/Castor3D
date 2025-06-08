#include "ComCastor3D/CastorUtils/ComFont.hpp"
#include "ComCastor3D/CastorUtils/ComGlyph.hpp"

namespace CastorCom
{
	STDMETHODIMP CFont::GetGlyph( /*[in]*/ WORD glyph, /*[out, retval]*/ IGlyph ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetGlyph" ) );
		if ( CGlyph::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dFont_getGlyph( m_internal
			, glyph
			, &static_cast< CGlyph * >( *pRet )->getInternal() ) );
	}
}

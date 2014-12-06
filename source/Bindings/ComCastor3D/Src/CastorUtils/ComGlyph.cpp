#include "ComGlyph.hpp"

namespace CastorCom
{
	CGlyph::CGlyph()
	{
	}

	CGlyph::~CGlyph()
	{
	}

	STDMETHODIMP CGlyph::AdjustPosition( /* [in] */ UINT x, /* [in] */ UINT y )
	{
		HRESULT hr = E_POINTER;

		if ( m_glyph )
		{
			m_glyph->AdjustPosition( x, y );
			hr = S_OK;
		}

		return hr;
	}
}

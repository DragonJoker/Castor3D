#include "ComFont.hpp"
#include "ComUtils.hpp"
#include "ComGlyph.hpp"
#include "ComEngine.hpp"

namespace CastorCom
{
	static const TCHAR * ERROR_WRONG_FILE_NAME = _T( "The given file doesn't exist" );
	static const TCHAR * ERROR_FONT_EXISTS = _T( "The given font name already exists" );
	static const TCHAR * ERROR_UNINITIALISED_FONT = _T( "The font must be initialised" );

	CFont::CFont()
	{
	}

	CFont::~CFont()
	{
	}

	STDMETHODIMP CFont::LoadFromFile( /* [in] */ IEngine * engine, /* [in] */ BSTR path, /* [in] */ BSTR name, /* [in] */ UINT height )
	{
		HRESULT hr = E_POINTER;

		if ( engine && !m_font )
		{
			CEngine * l_engn = static_cast< CEngine * >( engine );
			castor3d::Engine * l_engine = l_engn->getInternal();
			castor::String l_name = FromBstr( name );
			castor::Path l_path{ FromBstr( path ) };
			castor::Path l_pathFont = l_path;

			if ( !castor::File::fileExists( l_pathFont ) )
			{
				l_pathFont = l_engine->getDataDirectory() / l_path;
			}

			m_font = l_engine->getFontCache().add( l_name, height, l_pathFont );

			if ( !m_font )
			{
				hr = CComError::DispatchError( E_FAIL, IID_IFont, cuT( "LoadFromFile" ), ERROR_WRONG_FILE_NAME, 0, NULL );
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IFont, cuT( "LoadFromFile" ), ERROR_FONT_EXISTS, 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CFont::getGlyph( /* [in] */ WORD glyph, /* [out, retval] */ IGlyph ** pGlyph )
	{
		HRESULT hr = E_POINTER;

		if ( m_font )
		{
			hr = S_OK;
			std::map< WORD, IGlyph * >::iterator l_it = m_glyphs.find( glyph );

			if ( l_it == m_glyphs.end() )
			{
				CGlyph::CreateInstance( pGlyph );
			}
			else
			{
				*pGlyph = l_it->second;
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IFont, cuT( "getGlyph" ), ERROR_UNINITIALISED_FONT, 0, NULL );
		}

		return hr;
	}
}

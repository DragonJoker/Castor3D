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
			Castor3D::Engine * l_engine = l_engn->GetInternal();
			Castor::String l_name = FromBstr( name );
			Castor::Path l_path{ FromBstr( path ) };
			Castor::Path l_pathFont = l_path;

			if ( !Castor::File::FileExists( l_pathFont ) )
			{
				l_pathFont = l_engine->GetDataDirectory() / l_path;
			}

			m_font = l_engine->GetFontCache().Add( l_name, height, l_pathFont );

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

	STDMETHODIMP CFont::GetGlyph( /* [in] */ WORD glyph, /* [out, retval] */ IGlyph ** pGlyph )
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
			hr = CComError::DispatchError( E_FAIL, IID_IFont, cuT( "GetGlyph" ), ERROR_UNINITIALISED_FONT, 0, NULL );
		}

		return hr;
	}
}

#include "ComCastor3D/CastorUtils/ComFont.hpp"
#include "ComCastor3D/CastorUtils/ComGlyph.hpp"
#include "ComCastor3D/Castor3D/ComEngine.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	namespace font
	{
		static const tstring ERROR_WRONG_FILE_NAME = _T( "The given file doesn't exist" );
		static const tstring ERROR_FONT_EXISTS = _T( "The given font name already exists" );
		static const tstring ERROR_UNINITIALISED_FONT = _T( "The font must be initialised" );
	}

	STDMETHODIMP CFont::LoadFromFile( /* [in] */ IEngine * engine, /* [in] */ BSTR path, /* [in] */ BSTR name, /* [in] */ UINT height )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( engine && !m_internal )
		{
			CEngine * l_engn = static_cast< CEngine * >( engine );
			castor3d::Engine * l_engine = l_engn->getInternal();
			castor::String l_name = fromBstr( name );
			castor::Path l_path{ fromBstr( path ) };
			castor::Path l_pathFont = l_path;

			if ( !castor::File::fileExists( l_pathFont ) )
			{
				l_pathFont = l_engine->getDataDirectory() / l_path;
			}

			m_internal = l_engine->addNewFont( l_name, height, l_pathFont );

			if ( !m_internal )
			{
				hr = CComError::dispatchError( E_FAIL, IID_IFont, _T( "LoadFromFile" ), font::ERROR_WRONG_FILE_NAME.c_str(), 0, nullptr );
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IFont, _T( "LoadFromFile" ), font::ERROR_FONT_EXISTS.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CFont::GetGlyph( /* [in] */ WORD glyph, /* [out, retval] */ IGlyph ** pGlyph )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = S_OK;
			std::map< WORD, IGlyph * >::iterator l_it = m_glyphs.find( glyph );

			if ( l_it == m_glyphs.end() )
			{
				hr = CGlyph::CreateInstance( pGlyph );
			}
			else
			{
				*pGlyph = l_it->second;
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IFont, _T( "GetGlyph" ), font::ERROR_UNINITIALISED_FONT.c_str(), 0, nullptr );
		}

		return hr;
	}
}

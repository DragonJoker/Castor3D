#include "ComFont.hpp"
#include "ComUtils.hpp"
#include "ComGlyph.hpp"
#include "ComEngine.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_WRONG_FILE_NAME = cuT( "The given file doesn't exist" );
	static const Castor::String ERROR_IMAGE_EXISTS = cuT( "The given font name already exists" );

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
			Castor3D::FontCollection & l_mgr = l_engine->GetFontManager();
			Castor::String l_name = FromBstr( name );
			Castor::Path l_path = FromBstr( path );
			m_font = l_mgr.find( l_name );
			Castor::Path l_pathFont = l_path;

			if ( !Castor::File::FileExists( l_pathFont ) )
			{
				l_pathFont = l_engine->GetDataPath() / l_path;
			}

			if ( Castor::File::FileExists( l_pathFont ) )
			{
				if ( !m_font )
				{
					m_font = std::make_shared< Castor::Font >( l_pathFont, l_name, height );
				}
				else
				{
					Castor::Font::BinaryLoader()( *m_font, l_pathFont, height );
				}

				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,						// This represents the error
						 IID_IFont,						// This is the GUID of component throwing error
						 cuT( "LoadFromFile" ),			// This is generally displayed as the title
						 ERROR_WRONG_FILE_NAME.c_str(),	// This is the description
						 0,								// This is the context in the help file
						 NULL );
			}
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

		return hr;
	}
}

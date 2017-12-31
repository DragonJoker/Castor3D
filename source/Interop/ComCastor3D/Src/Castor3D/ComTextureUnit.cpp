#include "ComTextureUnit.hpp"
#include "ComUtils.hpp"

#include <Render/RenderSystem.hpp>

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The texture unit must be initialised" );

	CTextureUnit::CTextureUnit()
	{
	}

	CTextureUnit::~CTextureUnit()
	{
	}

	STDMETHODIMP CTextureUnit::LoadTexture( /* [in] */ BSTR path )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto l_texture = m_internal->getEngine()->getRenderSystem()->createTexture( castor3d::TextureType::eTwoDimensions
				, castor3d::AccessType::eRead
				, castor3d::AccessType::eRead );
			castor::Path l_path{ fromBstr( path ) };
			l_texture->getImage().initialiseSource(l_path.getPath(), l_path.getFileName( true ) );
			m_internal->setTexture( l_texture );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_ITextureUnit,				// This is the GUID of PixelComponents throwing error
					 cuT( "LoadTexture" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

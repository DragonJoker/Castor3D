#include "ComTextureUnit.hpp"
#include "ComUtils.hpp"

#include <Render/RenderSystem.hpp>

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The texture unit must be initialised" );

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
			auto l_texture = m_internal->GetEngine()->GetRenderSystem()->CreateTexture( Castor3D::TextureType::eTwoDimensions
				, Castor3D::AccessType::eRead
				, Castor3D::AccessType::eRead );
			Castor::Path l_path{ FromBstr( path ) };
			l_texture->GetImage().InitialiseSource(l_path.GetPath(), l_path.GetFileName( true ) );
			m_internal->SetTexture( l_texture );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ITextureUnit,				// This is the GUID of component throwing error
					 cuT( "LoadTexture" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

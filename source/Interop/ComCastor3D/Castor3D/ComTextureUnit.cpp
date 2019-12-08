#include "ComCastor3D/Castor3D/ComTextureUnit.hpp"
#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"
#include "ComCastor3D/ComUtils.hpp"

#include <Castor3D/Render/RenderSystem.hpp>

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The texture unit must be initialised" );

	CTextureUnit::CTextureUnit()
	{
	}

	CTextureUnit::~CTextureUnit()
	{
	}

	STDMETHODIMP CTextureUnit::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialise();
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,						// This represents the error
				IID_ITextureUnit,				// This is the GUID of PixelComponents throwing error
				_T( "Initialise" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureUnit::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->cleanup();
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,						// This represents the error
				IID_ITextureUnit,				// This is the GUID of PixelComponents throwing error
				_T( "Cleanup" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureUnit::LoadTexture( /* [in] */ BSTR path )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			ashes::ImageCreateInfo imageInfo
			{
				0u,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_UNDEFINED,
				{ 1u, 1u, 1u },
				0u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
			};
			auto texture = std::make_shared< castor3d::TextureLayout >( *m_internal->getEngine()->getRenderSystem()
				, imageInfo
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "ComStaticImageView" );
			castor::Path filePath{ fromBstr( path ) };
			texture->setSource( filePath.getPath()
				, filePath.getFileName( true ) );
			m_internal->setTexture( texture );

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,							// This represents the error
				IID_ITextureUnit,				// This is the GUID of PixelComponents throwing error
				_T( "LoadTexture" ),			// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}
}

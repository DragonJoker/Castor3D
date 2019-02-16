#include "ComCastor3D/Castor3D/ComTextureUnit.hpp"
#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"
#include "ComCastor3D/ComUtils.hpp"

#include <Castor3D/Render/RenderSystem.hpp>

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The texture unit must be initialised" );

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
					 cuT( "Initialise" ),			// This is generally displayed as the title
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
					 cuT( "Cleanup" ),				// This is generally displayed as the title
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
				ashes::TextureType::e2D,
				ashes::Format::eUndefined,
				{ 1u, 1u, 1u },
				0u,
				1u,
				ashes::SampleCountFlag::e1,
				ashes::ImageTiling::eOptimal,
				ashes::ImageUsageFlag::eSampled | ashes::ImageUsageFlag::eTransferDst
			};
			auto texture = std::make_shared< castor3d::TextureLayout >( *m_internal->getEngine()->getRenderSystem()
				, imageInfo
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			castor::Path filePath{ fromBstr( path ) };
			texture->setSource( filePath.getPath()
				, filePath.getFileName( true )
				, castor3d::ImageComponents::eAll );
			m_internal->setTexture( texture );

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

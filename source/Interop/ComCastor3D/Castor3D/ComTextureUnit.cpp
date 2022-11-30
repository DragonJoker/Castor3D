#include "ComCastor3D/Castor3D/ComTextureUnit.hpp"
#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"
#include "ComCastor3D/ComUtils.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>

namespace CastorCom
{
	namespace texunit
	{
		static const tstring ERROR_UNINITIALISED = _T( "The texture unit must be initialised" );
	}

	STDMETHODIMP CTextureUnit::Initialise()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			auto & device = *m_internal->getOwner()->getRenderDevice();
			auto queueData = device.graphicsData();
			m_internal->initialise( device, *queueData );
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,						// This represents the error
				IID_ITextureUnit,				// This is the GUID of PixelComponents throwing error
				_T( "Initialise" ),			// This is generally displayed as the title
				texunit::ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTextureUnit::Cleanup()noexcept
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
				texunit::ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}
}

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
}

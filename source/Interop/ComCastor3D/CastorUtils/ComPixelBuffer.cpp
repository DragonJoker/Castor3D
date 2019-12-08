#include "ComCastor3D/CastorUtils/ComPixelBuffer.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED_BUFFER = _T( "The buffer must be initialised" );
	static const tstring ERROR_INITIALISED_BUFFER = _T( "The buffer is already initialised" );

	CPixelBuffer::CPixelBuffer()
	{
	}

	CPixelBuffer::~CPixelBuffer()
	{
	}

	STDMETHODIMP CPixelBuffer::Flip()
	{
		HRESULT hr = E_POINTER;

		if ( m_buffer )
		{
			m_buffer->flip();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,								// This represents the error
					 IID_IPixelBuffer,						// This is the GUID of PixelComponents throwing error
					 _T( "Flip" ),							// This is generally displayed as the title
					 ERROR_UNINITIALISED_BUFFER.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPixelBuffer::Mirror()
	{
		HRESULT hr = E_POINTER;

		if ( m_buffer )
		{
			m_buffer->mirror();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,								// This represents the error
					 IID_IPixelBuffer,						// This is the GUID of PixelComponents throwing error
					 _T( "Mirror" ),						// This is generally displayed as the title
					 ERROR_UNINITIALISED_BUFFER.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPixelBuffer::Initialise( /* [in] */ ISize * size, /* [in] */ ePIXEL_FORMAT format )
	{
		HRESULT hr = E_FAIL;

		if ( !m_buffer )
		{
			m_buffer = castor::PxBufferBase::create( *static_cast< CSize * >( size ), castor::PixelFormat( format ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,							// This represents the error
					 IID_IPixelBuffer,					// This is the GUID of PixelComponents throwing error
					 _T( "Initialise" ),				// This is generally displayed as the title
					 ERROR_INITIALISED_BUFFER.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

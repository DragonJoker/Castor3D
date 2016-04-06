#include "ComStaticTexture.hpp"
#include "ComLogger.hpp"
#include "ComSize.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The texture must be initialised" );

	CStaticTexture::CStaticTexture()
	{
	}

	CStaticTexture::~CStaticTexture()
	{
	}

	STDMETHODIMP CStaticTexture::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = m_internal->Initialise() ? S_OK : E_FAIL;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IStaticTexture,			// This is the GUID of component throwing error
					 cuT( "Initialise" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CStaticTexture::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IStaticTexture,			// This is the GUID of component throwing error
					 cuT( "Cleanup" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CStaticTexture::SetImage( /* [in] */ IPixelBuffer * buffer )
	{
		HRESULT hr = S_OK;

		if ( m_internal )
		{
			m_internal->SetImage( static_cast< CPixelBuffer * >( buffer )->GetInternal() );
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IStaticTexture,			// This is the GUID of component throwing error
					 cuT( "SetImage" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CStaticTexture::Set3DImage( /* [in] */ ISize * size, /* [in] */ unsigned int depth, /* [in] */ IPixelBuffer * buffer )
	{
		HRESULT hr = S_OK;

		if ( m_internal )
		{
			Castor::Size sz = *static_cast< CSize * >( size );
			m_internal->SetImage( Castor::Point3ui( sz.width(), sz.height(), depth ), static_cast< CPixelBuffer * >( buffer )->GetInternal() );
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IStaticTexture,			// This is the GUID of component throwing error
					 cuT( "Set3DImage" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

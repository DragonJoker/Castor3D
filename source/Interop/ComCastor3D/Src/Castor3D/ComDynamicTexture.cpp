#include "ComDynamicTexture.hpp"
#include "ComLogger.hpp"
#include "ComSize.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The texture must be initialised" );

	CDynamicTexture::CDynamicTexture()
	{
	}

	CDynamicTexture::~CDynamicTexture()
	{
	}

	STDMETHODIMP CDynamicTexture::Initialise()
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
					 IID_IDynamicTexture,			// This is the GUID of component throwing error
					 cuT( "Initialise" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CDynamicTexture::Cleanup()
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
					 IID_IDynamicTexture,			// This is the GUID of component throwing error
					 cuT( "Cleanup" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CDynamicTexture::Set2DImage( /* [in] */ ISize * size, /* [in] */ ePIXEL_FORMAT fmt )
	{
		HRESULT hr = S_OK;

		if ( m_internal )
		{
			m_internal->SetImage( *static_cast< CSize * >( size ), Castor::ePIXEL_FORMAT( fmt ) );
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IDynamicTexture,			// This is the GUID of component throwing error
					 cuT( "Set2DImage" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CDynamicTexture::Set3DImage( /* [in] */ ISize * size, /* [in] */ unsigned int depth, /* [in] */ ePIXEL_FORMAT fmt )
	{
		HRESULT hr = S_OK;

		if ( m_internal )
		{
			Castor::Size sz = *static_cast< CSize * >( size );
			m_internal->SetImage( Castor::Point3ui( sz.width(), sz.height(), depth ), Castor::ePIXEL_FORMAT( fmt ) );
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IDynamicTexture,			// This is the GUID of component throwing error
					 cuT( "Set3DImage" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CDynamicTexture::Resize2D( /* [in] */ ISize * size )
	{
		HRESULT hr = S_OK;

		if ( m_internal )
		{
			m_internal->Resize( *static_cast< CSize * >( size ) );
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IDynamicTexture,			// This is the GUID of component throwing error
					 cuT( "Resize2D" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CDynamicTexture::Resize3D( /* [in] */ ISize * size, /* [in] */unsigned int depth )
	{
		HRESULT hr = S_OK;

		if ( m_internal )
		{
			Castor::Size sz = *static_cast< CSize * >( size );
			m_internal->Resize( Castor::Point3ui( sz.width(), sz.height(), depth ) );
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IDynamicTexture,			// This is the GUID of component throwing error
					 cuT( "Resize3D" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

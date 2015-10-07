#include "ComRenderWindow.hpp"

#include <PlatformWindowHandle.hpp>

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The render window must be initialised" );

	CRenderWindow::CRenderWindow()
	{
	}

	CRenderWindow::~CRenderWindow()
	{
	}

	STDMETHODIMP CRenderWindow::Initialise( /* [in] */ LPVOID val, /* [out, retval] */ VARIANT_BOOL * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				try
				{
					*pVal = m_internal->Initialise( Castor3D::WindowHandle( std::make_shared< Castor3D::IMswWindowHandle >( HWND( val ) ) ) ) ? VARIANT_TRUE : VARIANT_FALSE;
					hr = S_OK;
				}
				catch ( Castor::Exception & p_exc )
				{
					Castor::Logger::LogError( p_exc.GetFullDescription() );
				}
				catch ( std::exception & p_exc )
				{
					Castor::Logger::LogError( p_exc.what() );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IRenderWindow,				// This is the GUID of component throwing error
					 cuT( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::Cleanup()
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
					 IID_IRenderWindow,				// This is the GUID of component throwing error
					 cuT( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::Resize( /* [in] */ ISize * size )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Resize( *static_cast< CSize * >( size ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IRenderWindow,				// This is the GUID of component throwing error
					 cuT( "Source" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

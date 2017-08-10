#include "ComMaterial.hpp"
#include "ComPass.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The material must be initialised" );
	static const castor::String ERROR_OUT_OF_BOUND_INDEX = cuT( "The given index was out of bounds" );

	CMaterial::CMaterial()
	{
	}

	CMaterial::~CMaterial()
	{
	}

	STDMETHODIMP CMaterial::initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = S_OK;
			m_internal->initialise();
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "Initialise" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMaterial::cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "Cleanup" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMaterial::createPass( /* [out, retval] */ IPass ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CPass::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CPass * >( *pVal )->setInternal( m_internal->createPass() );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "createPass" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMaterial::getPass( /* [in] */ unsigned int val, /* [out, retval] */ IPass ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CPass::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					try
					{
						static_cast< CPass * >( *pVal )->setInternal( m_internal->getPass( val ) );
					}
					catch ( std::exception & )
					{
						hr = CComError::DispatchError(
								 E_FAIL,							// This represents the error
								 IID_IMaterial,						// This is the GUID of PixelComponents throwing error
								 cuT( "getPass" ),					// This is generally displayed as the title
								 ERROR_OUT_OF_BOUND_INDEX.c_str(),	// This is the description
								 0,									// This is the context in the help file
								 NULL );
					}
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "getPass" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMaterial::destroyPass( /* [in] */ unsigned int val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			try
			{
				m_internal->destroyPass( val );
				hr = S_OK;
			}
			catch ( std::exception & )
			{
				hr = CComError::DispatchError(
						 E_FAIL,							// This represents the error
						 IID_IMaterial,						// This is the GUID of PixelComponents throwing error
						 cuT( "destroyPass" ),				// This is generally displayed as the title
						 ERROR_OUT_OF_BOUND_INDEX.c_str(),	// This is the description
						 0,									// This is the context in the help file
						 NULL );
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "destroyPass" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

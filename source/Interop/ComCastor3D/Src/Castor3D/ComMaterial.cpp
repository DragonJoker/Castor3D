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

	STDMETHODIMP CMaterial::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = S_OK;
			m_internal->initialise();
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "Initialise" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CMaterial::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "Cleanup" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CMaterial::CreatePass( /* [out, retval] */ IPass ** pVal )
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
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "CreatePass" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CMaterial::GetPass( /* [in] */ unsigned int val, /* [out, retval] */ IPass ** pVal )
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
						hr = CComError::dispatchError(
								 E_FAIL,							// This represents the error
								 IID_IMaterial,						// This is the GUID of PixelComponents throwing error
								 cuT( "GetPass" ),					// This is generally displayed as the title
								 ERROR_OUT_OF_BOUND_INDEX.c_str(),	// This is the description
								 0,									// This is the context in the help file
								 nullptr );
					}
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "GetPass" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CMaterial::DestroyPass( /* [in] */ unsigned int val )
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
				hr = CComError::dispatchError(
						 E_FAIL,							// This represents the error
						 IID_IMaterial,						// This is the GUID of PixelComponents throwing error
						 cuT( "DestroyPass" ),				// This is generally displayed as the title
						 ERROR_OUT_OF_BOUND_INDEX.c_str(),	// This is the description
						 0,									// This is the context in the help file
						 nullptr );
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMaterial,					// This is the GUID of PixelComponents throwing error
					 cuT( "DestroyPass" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

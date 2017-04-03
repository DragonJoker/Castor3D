#include "ComLegacyPass.hpp"
#include "ComLogger.hpp"
#include "ComSize.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The pass must be initialised" );

	CLegacyPass::CLegacyPass()
	{
	}

	CLegacyPass::~CLegacyPass()
	{
	}

	STDMETHODIMP CLegacyPass::CreateTextureUnit( /* [out, retval] */ ITextureUnit ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IPass,						// This is the GUID of component throwing error
					 cuT( "CreateTextureUnit" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CLegacyPass::GetTextureUnitByIndex( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IPass,							// This is the GUID of component throwing error
					 cuT( "GetTextureUnitByIndex" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),		// This is the description
					 0,									// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CLegacyPass::DestroyTextureUnit( /* [in] */ ITextureUnit * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IPass,						// This is the GUID of component throwing error
					 cuT( "DestroyTextureUnit" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CLegacyPass::GetTextureUnitByChannel( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IPass,							// This is the GUID of component throwing error
					 cuT( "GetTextureUnitByChannel" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),		// This is the description
					 0,									// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

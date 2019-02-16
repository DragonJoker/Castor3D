#include "ComCastor3D/Castor3D/ComPass.hpp"
#include "ComCastor3D/CastorUtils/ComLogger.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The pass must be initialised" );

	CPass::CPass()
	{
	}

	CPass::~CPass()
	{
	}

	STDMETHODIMP CPass::CreateTextureUnit( /* [out, retval] */ ITextureUnit ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IPass,						// This is the GUID of PixelComponents throwing error
					 cuT( "CreateTextureUnit" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPass::GetTextureUnitByIndex( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,							// This represents the error
					 IID_IPass,							// This is the GUID of PixelComponents throwing error
					 cuT( "GetTextureUnitByIndex" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),		// This is the description
					 0,									// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPass::DestroyTextureUnit( /* [in] */ ITextureUnit * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IPass,						// This is the GUID of PixelComponents throwing error
					 cuT( "DestroyTextureUnit" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPass::GetTextureUnitByChannel( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,							// This represents the error
					 IID_IPass,							// This is the GUID of PixelComponents throwing error
					 cuT( "GetTextureUnitByChannel" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),		// This is the description
					 0,									// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

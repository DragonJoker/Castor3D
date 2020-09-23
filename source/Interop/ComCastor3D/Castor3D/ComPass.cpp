#include "ComCastor3D/Castor3D/ComPass.hpp"
#include "ComCastor3D/CastorUtils/ComLogger.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The pass must be initialised" );

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
					 _T( "CreateTextureUnit" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
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
					 _T( "DestroyTextureUnit" ),	// This is generally displayed as the title
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
					 _T( "GetTextureUnitByIndex" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),		// This is the description
					 0,									// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPass::GetTextureUnitByFlag( /* [in] */ eTEXTURE_FLAG flag, /* [out, retval] */ ITextureUnit ** pVal )
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
					 _T( "GetTextureUnitByChannel" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),		// This is the description
					 0,									// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

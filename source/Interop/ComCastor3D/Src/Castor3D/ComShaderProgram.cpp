#include "ComShaderProgram.hpp"
#include "ComLogger.hpp"
#include "ComSize.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The shader program must be initialised" );

	CShaderProgram::CShaderProgram()
	{
	}

	CShaderProgram::~CShaderProgram()
	{
	}

	STDMETHODIMP CShaderProgram::get_File( /* [in] */ eSHADER_TYPE target, /* [out, retval] */ BSTR * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				*pVal = toBstr( m_internal->getFile( castor3d::ShaderType( target ) ) );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of PixelComponents throwing error
					 cuT( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::put_File( /* [in] */ eSHADER_TYPE target, /* [in] */ BSTR val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setFile( castor3d::ShaderType ( target ), castor::Path{ fromBstr( val ) } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of PixelComponents throwing error
					 cuT( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::get_Source( /* [in] */ eSHADER_TYPE target, /* [out, retval] */ BSTR * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				*pVal = toBstr( m_internal->getSource( castor3d::ShaderType( target ) ) );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of PixelComponents throwing error
					 cuT( "Source" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::put_Source( /* [in] */ eSHADER_TYPE target, /* [in] */ BSTR val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setSource( castor3d::ShaderType( target ), fromBstr( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of PixelComponents throwing error
					 cuT( "Source" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialise();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of PixelComponents throwing error
					 cuT( "Initialise" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::Cleanup()
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
					 IID_IShaderProgram,			// This is the GUID of PixelComponents throwing error
					 cuT( "Cleanup" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::CreateObject( /* [in] */ eSHADER_TYPE val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->createObject( castor3d::ShaderType( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of PixelComponents throwing error
					 cuT( "createObject" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

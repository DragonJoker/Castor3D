#include "ComShaderProgram.hpp"
#include "ComLogger.hpp"
#include "ComSize.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The shader program must be initialised" );

	CShaderProgram::CShaderProgram()
	{
	}

	CShaderProgram::~CShaderProgram()
	{
	}

	STDMETHODIMP CShaderProgram::get_File( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [out, retval] */ BSTR * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				*pVal = ToBstr( m_internal->GetFile( Castor3D::eSHADER_TYPE( target ), Castor3D::eSHADER_MODEL( model ) ) );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of component throwing error
					 cuT( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::put_File( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [in] */ BSTR val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->SetFile( Castor3D::eSHADER_TYPE( target ), Castor3D::eSHADER_MODEL( model ), Castor::Path{ FromBstr( val ) } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of component throwing error
					 cuT( "File" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::get_Source( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [out, retval] */ BSTR * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				*pVal = ToBstr( m_internal->GetSource( Castor3D::eSHADER_TYPE( target ), Castor3D::eSHADER_MODEL( model ) ) );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of component throwing error
					 cuT( "Source" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::put_Source( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [in] */ BSTR val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->SetSource( Castor3D::eSHADER_TYPE( target ), Castor3D::eSHADER_MODEL( model ), FromBstr( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of component throwing error
					 cuT( "Source" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::Initialise()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Initialise();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of component throwing error
					 cuT( "Initialise" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::Cleanup()
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
					 IID_IShaderProgram,			// This is the GUID of component throwing error
					 cuT( "Cleanup" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CShaderProgram::CreateObject( /* [in] */ eSHADER_TYPE val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->CreateObject( Castor3D::eSHADER_TYPE( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IShaderProgram,			// This is the GUID of component throwing error
					 cuT( "CreateObject" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

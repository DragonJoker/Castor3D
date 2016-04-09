#include "ComMesh.hpp"
#include "ComSubmesh.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The mesh must be initialised" );

	CMesh::CMesh()
	{
	}

	CMesh::~CMesh()
	{
	}

	STDMETHODIMP CMesh::GetSubmesh( /* [in] */ unsigned int val, /* [out, retval] */ ISubmesh ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSubmesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSubmesh * >( *pVal )->SetInternal( m_internal->GetSubmesh( val ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of component throwing error
					 cuT( "GetSubmesh" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMesh::CreateSubmesh( /* [out, retval] */ ISubmesh ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSubmesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSubmesh * >( *pVal )->SetInternal( m_internal->CreateSubmesh() );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of component throwing error
					 cuT( "CreateSubmesh" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMesh::DeleteSubmesh( /* [in] */ ISubmesh * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->DeleteSubmesh( static_cast< CSubmesh * >( val )->GetInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of component throwing error
					 cuT( "DeleteSubmesh" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

#include "ComMesh.hpp"
#include "ComSubmesh.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The mesh must be initialised" );

	CMesh::CMesh()
	{
	}

	CMesh::~CMesh()
	{
	}

	STDMETHODIMP CMesh::getSubmesh( /* [in] */ unsigned int val, /* [out, retval] */ ISubmesh ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSubmesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSubmesh * >( *pVal )->setInternal( m_internal->getSubmesh( val ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 cuT( "getSubmesh" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMesh::createSubmesh( /* [out, retval] */ ISubmesh ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSubmesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSubmesh * >( *pVal )->setInternal( m_internal->createSubmesh() );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 cuT( "createSubmesh" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CMesh::deleteSubmesh( /* [in] */ ISubmesh * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->deleteSubmesh( static_cast< CSubmesh * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 cuT( "deleteSubmesh" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

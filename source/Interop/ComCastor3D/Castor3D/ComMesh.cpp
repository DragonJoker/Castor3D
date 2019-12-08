#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComSubmesh.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The mesh must be initialised" );

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
					static_cast< CSubmesh * >( *pVal )->setInternal( m_internal->getSubmesh( val ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 _T( "GetSubmesh" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
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
					static_cast< CSubmesh * >( *pVal )->setInternal( m_internal->createSubmesh() );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 _T( "CreateSubmesh" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CMesh::DeleteSubmesh( /* [in] */ ISubmesh * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->deleteSubmesh( static_cast< CSubmesh * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 _T( "DeleteSubmesh" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

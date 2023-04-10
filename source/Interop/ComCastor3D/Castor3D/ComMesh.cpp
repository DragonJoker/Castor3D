#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComSubmesh.hpp"

namespace CastorCom
{
	namespace mesh
	{
		static const tstring ERROR_UNINITIALISED = _T( "The mesh must be initialised" );
	}

	STDMETHODIMP CMesh::GetSubmesh( /* [in] */ unsigned int val, /* [out, retval] */ ISubmesh ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( getMesh() )
		{
			if ( pVal )
			{
				hr = CSubmesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSubmesh * >( *pVal )->setInternal( getMesh()->getSubmesh( val ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 _T( "GetSubmesh" ),			// This is generally displayed as the title
					 mesh::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CMesh::CreateSubmesh( /* [out, retval] */ ISubmesh ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( getMesh() )
		{
			if ( pVal )
			{
				hr = CSubmesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSubmesh * >( *pVal )->setInternal( getMesh()->createSubmesh() );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 _T( "CreateSubmesh" ),		// This is generally displayed as the title
					 mesh::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CMesh::DeleteSubmesh( /* [in] */ ISubmesh * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( getMesh() )
		{
			auto submesh = static_cast< CSubmesh * >( val )->getInternal();
			static_cast< CSubmesh * >( val )->setInternal( nullptr );
			m_internal->deleteSubmesh( submesh );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IMesh,						// This is the GUID of PixelComponents throwing error
					 _T( "DeleteSubmesh" ),			// This is generally displayed as the title
				mesh::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

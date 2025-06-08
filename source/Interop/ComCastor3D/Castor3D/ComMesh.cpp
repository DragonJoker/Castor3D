#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/Castor3D/ComSubmesh.hpp"

namespace CastorCom
{
	STDMETHODIMP CMesh::Create( /*[in]*/ IScene * scene, /*[in]*/ BSTR type, /*[in]*/ BSTR name )noexcept
	{
		if ( !scene || !name )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dMesh_create( static_cast< CScene * >( scene )->getInternal()
			, bstrToString( type ).c_str()
			, bstrToString( name ).c_str()
			, &m_internal ) );
	}

	STDMETHODIMP CMesh::GetSubmesh( /*[in]*/ UINT val, /*[out, retval]*/ ISubmesh ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetSubmesh" ) );
		if ( CSubmesh::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dMesh_getSubmesh( m_internal
			, val
			, &static_cast< CSubmesh * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CMesh::CreateSubmesh( /*[out, retval]*/ ISubmesh ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateSubmesh" ) );
		if ( CSubmesh::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dMesh_createSubmesh( m_internal
			, &static_cast< CSubmesh * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CMesh::RemoveSubmesh( /*[in]*/ ISubmesh * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveSubmesh" ) );

		return convert( c3dMesh_removeSubmesh( m_internal, static_cast< CSubmesh * >( val )->getInternal() ) );
	}
}

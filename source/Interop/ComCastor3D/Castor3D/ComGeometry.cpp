#include "ComCastor3D/Castor3D/ComGeometry.hpp"
#include "ComCastor3D/Castor3D/ComMaterial.hpp"
#include "ComCastor3D/Castor3D/ComSubmesh.hpp"

namespace CastorCom
{
	STDMETHODIMP CGeometry::Create( /*[in]*/ IScene * scene, /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ IMesh * mesh )noexcept
	{
		if ( !scene || !name || !parent )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dGeometry_create( static_cast< CScene * >( scene )->getInternal()
			, bstrToString( name ).c_str()
			, static_cast< CMesh * >( mesh )->getInternal()
			, static_cast< CSceneNode * >( parent )->getInternal()
			, &m_internal ) );
	}

	STDMETHODIMP CGeometry::AttachTo( /*[in]*/ ISceneNode * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AttachTo" ) );

		return convert( c3dGeometry_attachTo( m_internal
			, static_cast< CSceneNode * >( val )->getInternal() ) );
	}

	STDMETHODIMP CGeometry::Detach()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Detach" ) );

		return convert( c3dGeometry_detach( m_internal ) );
	}

	STDMETHODIMP CGeometry::GetMaterial( /*[in]*/ ISubmesh * submesh, /*[out, retval]*/ IMaterial ** pVal )noexcept
	{
		if ( !submesh || !pVal )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetMaterial" ) );
		if ( CMaterial::CreateInstance( pVal ) != S_OK )
			return E_FAIL;

		return convert( c3dGeometry_getMaterial( m_internal
			, static_cast< CSubmesh * >( submesh )->getInternal()
			, &static_cast< CMaterial * >( *pVal )->getInternal() ) );
	}

	STDMETHODIMP CGeometry::SetMaterial( /*[in]*/ ISubmesh * submesh, /*[in]*/ IMaterial * val )noexcept
	{
		if ( !submesh || !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "SetMaterial" ) );

		return convert( c3dGeometry_setMaterial( m_internal
			, static_cast< CSubmesh * >( submesh )->getInternal()
			, static_cast< CMaterial * >( val )->getInternal() ) );
	}
}

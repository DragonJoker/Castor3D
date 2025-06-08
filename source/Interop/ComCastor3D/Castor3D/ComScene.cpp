#include "ComCastor3D/Castor3D/ComScene.hpp"

#include "ComCastor3D/Castor3D/ComCamera.hpp"
#include "ComCastor3D/Castor3D/ComEngine.hpp"
#include "ComCastor3D/Castor3D/ComGeometry.hpp"
#include "ComCastor3D/Castor3D/ComLight.hpp"
#include "ComCastor3D/Castor3D/ComLightGroup.hpp"
#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"

namespace CastorCom
{
	STDMETHODIMP CScene::AddNode( /*[in]*/ ISceneNode * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddNode" ) );

		return convert( c3dScene_addNode( m_internal, static_cast< CSceneNode * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::AddGeometry( /*[in]*/ IGeometry * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddGeometry" ) );

		return convert( c3dScene_addGeometry( m_internal, static_cast< CGeometry * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::AddCamera( /*[in]*/ ICamera * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddCamera" ) );

		return convert( c3dScene_addCamera( m_internal, static_cast< CCamera * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::AddLight( /*[in]*/ ILight * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddLight" ) );

		return convert( c3dScene_addLight( m_internal, static_cast< CLight * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::AddLightGroup( /*[in]*/ ILightGroup * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddLightGroup" ) );

		return convert( c3dScene_addLightGroup( m_internal, static_cast< CLightGroup * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::AddMesh( /*[in]*/ IMesh * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddMesh" ) );

		return convert( c3dScene_addMesh( m_internal, static_cast< CMesh * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::GetNode( /*[in]*/ BSTR name, /*[out, retval]*/ ISceneNode ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetNode" ) );
		if ( CSceneNode::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_getNode( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CSceneNode * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::GetGeometry( /*[in]*/ BSTR name, /*[out, retval]*/ IGeometry ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetGeometry" ) );
		if ( CGeometry::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_getGeometry( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CGeometry * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::GetCamera( /*[in]*/ BSTR name, /*[out, retval]*/ ICamera ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetCamera" ) );
		if ( CCamera::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_getCamera( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CCamera * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::GetLight( /*[in]*/ BSTR name, /*[out, retval]*/ ILight ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetLight" ) );
		if ( CLight::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_getLight( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CLight * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::GetLightGroup( /*[in]*/ BSTR name, /*[out, retval]*/ ILightGroup ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetLightGroup" ) );
		if ( CLightGroup::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_getLightGroup( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CLightGroup * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::GetMesh( /*[in]*/ BSTR name, /*[out, retval]*/ IMesh ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetMesh" ) );
		if ( CMesh::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_getMesh( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CMesh * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::RemoveNode( /*[in]*/ ISceneNode * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveNode" ) );

		return convert( c3dScene_removeNode( m_internal, static_cast< CSceneNode * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::RemoveGeometry( /*[in]*/ IGeometry * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveGeometry" ) );

		return convert( c3dScene_removeGeometry( m_internal, static_cast< CGeometry * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::RemoveCamera( /*[in]*/ ICamera * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveCamera" ) );

		return convert( c3dScene_removeCamera( m_internal, static_cast< CCamera * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::RemoveLight( /*[in]*/ ILight * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveLight" ) );

		return convert( c3dScene_removeLight( m_internal, static_cast< CLight * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::RemoveLightGroup( /*[in]*/ ILightGroup * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveLightGroup" ) );

		return convert( c3dScene_removeLightGroup( m_internal, static_cast< CLightGroup * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::RemoveMesh( /*[in]*/ IMesh * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveMesh" ) );

		return convert( c3dScene_removeMesh( m_internal, static_cast< CMesh * >( val )->getInternal() ) );
	}

	STDMETHODIMP CScene::CreateNode( /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[out, retval]*/ ISceneNode ** pRet )noexcept
	{
		if ( !pRet || !name )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateNode" ) );
		if ( CSceneNode::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_createNode( m_internal
			, bstrToString( name ).c_str()
			, parent ? static_cast< CSceneNode * >( parent )->getInternal() : nullptr
			, &static_cast< CSceneNode * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::CreateGeometry( /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ IMesh * mesh, /*[out, retval]*/ IGeometry ** pRet )noexcept
	{
		if ( !pRet || !name || !parent )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateGeometry" ) );
		if ( CGeometry::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_createGeometry( m_internal
			, bstrToString( name ).c_str()
			, static_cast< CMesh * >( mesh )->getInternal()
			, static_cast< CSceneNode * >( parent )->getInternal()
			, &static_cast< CGeometry * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::CreateCamera( /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ UINT width, /*[in]*/ UINT height, /*[out, retval]*/ ICamera ** pRet )noexcept
	{
		if ( !pRet || !name || !parent )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateCamera" ) );
		if ( CCamera::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_createCamera( m_internal
			, bstrToString( name ).c_str()
			, width, height
			, static_cast< CSceneNode * >( parent )->getInternal()
			, &static_cast< CCamera * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::CreateLight( /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ eLIGHT_TYPE type, /*[out, retval]*/ ILight ** pRet )noexcept
	{
		if ( !pRet || !name || !parent )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateLight" ) );
		if ( CLight::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_createLight( m_internal
			, bstrToString( name ).c_str()
			, static_cast< CSceneNode * >( parent )->getInternal()
			, details::parameterCast< C3D_LIGHT_TYPE >( type )
			, &static_cast< CLight * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::CreateLightGroup( /*[in]*/ BSTR name, /*[in]*/ eLIGHT_TYPE type, /*[out, retval]*/ ILightGroup ** pRet )noexcept
	{
		if ( !pRet || !name )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateLightGroup" ) );
		if ( CLightGroup::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_createLightGroup( m_internal
			, bstrToString( name ).c_str()
			, details::parameterCast< C3D_LIGHT_TYPE >( type )
			, &static_cast< CLightGroup * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CScene::CreateMesh( /*[in]*/ BSTR name, /*[in]*/ BSTR type, /*[out, retval]*/ IMesh ** pRet )noexcept
	{
		if ( !pRet || !name )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateMesh" ) );
		if ( CMesh::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dScene_createMesh( m_internal
			, bstrToString( type ).c_str()
			, bstrToString( name ).c_str()
			, &static_cast< CMesh * >( *pRet )->getInternal() ) );
	}
}

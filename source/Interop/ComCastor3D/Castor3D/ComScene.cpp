#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/Castor3D/ComGeometry.hpp"
#include "ComCastor3D/Castor3D/ComCamera.hpp"
#include "ComCastor3D/Castor3D/ComLight.hpp"
#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"

#include <Castor3D/Cache/BillboardCache.hpp>
#include <Castor3D/Cache/CameraCache.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Cache/WindowCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/Viewport.hpp>

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The scene must be initialised" );

	CScene::CScene()
	{
	}

	CScene::~CScene()
	{
	}

	STDMETHODIMP CScene::ClearScene()
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
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "ClearScene" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateSceneNode( /* [in] */ BSTR name, /* [in] */ ISceneNode * parent, /* [out, retval] */ ISceneNode ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSceneNode::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSceneNode * >( *pVal )->setInternal( m_internal->getSceneNodeCache().add( fromBstr( name ), static_cast< CSceneNode * >( parent )->getInternal() ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "CreateSceneNode" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateGeometry( /* [in] */ BSTR name, /* [out, retval] */ IGeometry ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CGeometry::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CGeometry * >( *pVal )->setInternal( m_internal->getGeometryCache().add( fromBstr( name ), nullptr, nullptr ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "CreateGeometry" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateCamera( /* [in] */ BSTR name, /* [in] */ int ww, /* [in] */ int wh, /* [in] */ ISceneNode * node, /* [out, retval] */ ICamera ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CCamera::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					castor3d::Viewport l_viewport{ *getInternal()->getEngine() };
					l_viewport.setPerspective( castor::Angle::fromDegrees( 120 ), 4.0f / 3.0f, 0.1f, 1000.0f );
					l_viewport.resize( castor::Size( ww, wh ) );
					auto l_camera = m_internal->getCameraCache().add( fromBstr( name )
						, node ? static_cast< CSceneNode * >( node )->getInternal() : nullptr
						, std::move( l_viewport ) );
					static_cast< CCamera * >( *pVal )->setInternal( l_camera );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "CreateCamera" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateLight( /* [in] */ BSTR name, /* [in] */ ISceneNode * node, /* [in] */ eLIGHT_TYPE type, /* [out, retval] */ ILight ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CLight::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CLight * >( *pVal )->setInternal( m_internal->getLightCache().add( fromBstr( name ), node ? static_cast< CSceneNode * >( node )->getInternal() : nullptr, castor3d::LightType( type ) ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "CreateLight" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateMesh( /* [in] */ BSTR type, /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CMesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					auto l_mesh = m_internal->getMeshCache().add( fromBstr( name ) );
					m_internal->getEngine()->getMeshFactory().create( fromBstr( name ) )->generate( *l_mesh, castor3d::Parameters{} );
					static_cast< CMesh * >( *pVal )->setInternal( l_mesh );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "CreateMesh" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetNode( /* [in] */ BSTR name, /* [out, retval] */ ISceneNode ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSceneNode::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSceneNode * >( *pVal )->setInternal( m_internal->getSceneNodeCache().find( fromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "GetNode" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetGeometry( /* [in] */ BSTR name, /* [out, retval] */ IGeometry ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CGeometry::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CGeometry * >( *pVal )->setInternal( m_internal->getGeometryCache().find( fromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "GetGeometry" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetLight( /* [in] */ BSTR name, /* [out, retval] */ ILight ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CLight::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CLight * >( *pVal )->setInternal( m_internal->getLightCache().find( fromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "GetLight" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetCamera( /* [in] */ BSTR name, /* [out, retval] */ ICamera ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CCamera::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CCamera * >( *pVal )->setInternal( m_internal->getCameraCache().find( fromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "GetCamera" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveLight( /* [in] */ ILight * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( val )
			{
				m_internal->getLightCache().remove( static_cast< CLight * >( val )->getInternal()->getName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "RemoveLight" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveNode( /* [in] */ ISceneNode * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( val )
			{
				m_internal->getSceneNodeCache().remove( static_cast< CSceneNode * >( val )->getInternal()->getName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "RemoveNode" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveGeometry( /* [in] */ IGeometry * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( val )
			{
				m_internal->getGeometryCache().remove( static_cast< CGeometry * >( val )->getInternal()->getName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "RemoveGeometry" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveCamera( /* [in] */ ICamera * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( val )
			{
				m_internal->getCameraCache().remove( static_cast< CCamera * >( val )->getInternal()->getName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "RemoveCamera" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

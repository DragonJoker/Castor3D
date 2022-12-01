#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/Castor3D/ComGeometry.hpp"
#include "ComCastor3D/Castor3D/ComCamera.hpp"
#include "ComCastor3D/Castor3D/ComLight.hpp"
#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"

#include <Castor3D/Cache/BillboardCache.hpp>
#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Model/Mesh/MeshGenerator.hpp>
#include <Castor3D/Render/Viewport.hpp>

namespace CastorCom
{
	namespace scene
	{
		static const tstring ERROR_UNINITIALISED = _T( "The scene must be initialised" );
		static const tstring ERROR_NULL_PARENT_NODE = _T( "The parent node must not be null." );
	}

	STDMETHODIMP CScene::ClearScene()noexcept
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
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateSceneNode( /* [in] */ BSTR name, /* [in] */ ISceneNode * parent, /* [out, retval] */ ISceneNode ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSceneNode::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					if ( auto node = m_internal->createSceneNode( fromBstr( name ), *m_internal ) )
					{
						static_cast< CSceneNode * >( *pVal )->setInternal( node.get() );
						node->attachTo( *static_cast< CSceneNode * >( parent )->getInternal() );
						m_internal->addSceneNode( node->getName(), node );
					}
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "CreateSceneNode" ),		// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateGeometry( /* [in] */ BSTR name, /*[in] */ ISceneNode * node, /* [out, retval] */ IGeometry ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( !node )
			{
				hr = CComError::dispatchError(
					E_POINTER,						// This represents the error
					IID_IScene,						// This is the GUID of PixelComponents throwing error
					_T( "CreateLight" ),			// This is generally displayed as the title
					scene::ERROR_NULL_PARENT_NODE.c_str(),	// This is the description
					0,								// This is the context in the help file
					nullptr );
				return hr;
			}

			if ( pVal )
			{
				hr = CGeometry::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					auto geom = m_internal->createGeometry( fromBstr( name )
						, *m_internal
						, *static_cast< CSceneNode * >( node )->getInternal()
						, castor3d::MeshResPtr{} );
					static_cast< CGeometry * >( *pVal )->setInternal( geom.get() );
					m_internal->addGeometry( geom );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "CreateGeometry" ),		// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateCamera( /* [in] */ BSTR name, /* [in] */ int ww, /* [in] */ int wh, /* [in] */ ISceneNode * node, /* [out, retval] */ ICamera ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( !node )
			{
				hr = CComError::dispatchError(
					E_POINTER,						// This represents the error
					IID_IScene,						// This is the GUID of PixelComponents throwing error
					_T( "CreateLight" ),			// This is generally displayed as the title
					scene::ERROR_NULL_PARENT_NODE.c_str(),	// This is the description
					0,								// This is the context in the help file
					nullptr );
				return hr;
			}

			if ( pVal )
			{
				hr = CCamera::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					castor3d::Viewport l_viewport{ *getInternal()->getEngine() };
					l_viewport.setPerspective( castor::Angle::fromDegrees( 120.0f ), 4.0f / 3.0f, 0.1f, 1000.0f );
					l_viewport.resize( castor::Size( uint32_t( std::abs( ww ) ), uint32_t( std::abs( wh ) ) ) );
					auto camera = m_internal->createCamera( fromBstr( name )
						, *m_internal
						, *static_cast< CSceneNode * >( node )->getInternal()
						, std::move( l_viewport ) );
					static_cast< CCamera * >( *pVal )->setInternal( camera.get() );
					m_internal->addCamera( camera->getName(), camera );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "CreateCamera" ),			// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateLight( /* [in] */ BSTR name, /* [in] */ ISceneNode * node, /* [in] */ eLIGHT_TYPE type, /* [out, retval] */ ILight ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( !node )
			{
				hr = CComError::dispatchError(
					E_POINTER,						// This represents the error
					IID_IScene,						// This is the GUID of PixelComponents throwing error
					_T( "CreateLight" ),			// This is generally displayed as the title
					scene::ERROR_NULL_PARENT_NODE.c_str(),	// This is the description
					0,								// This is the context in the help file
					nullptr );
				return hr;
			}

			if ( pVal )
			{
				hr = CLight::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					auto light = m_internal->createLight( fromBstr( name )
						, *m_internal
						, *static_cast< CSceneNode * >( node )->getInternal()
						, m_internal->getLightsFactory()
						, castor3d::LightType( type ) );
					static_cast< CLight * >( *pVal )->setInternal( light.get() );
					m_internal->addLight( light->getName(), light );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "CreateLight" ),			// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateMesh( /* [in] */ BSTR type, /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CMesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					auto l_mesh = m_internal->createMesh( fromBstr( name ), *m_internal );
					m_internal->getEngine()->getMeshFactory().create( fromBstr( name ) )->generate( *l_mesh, castor3d::Parameters{} );
					static_cast< CMesh * >( *pVal )->setInternal( l_mesh );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "CreateMesh" ), scene::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetNode( /* [in] */ BSTR name, /* [out, retval] */ ISceneNode ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSceneNode::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					if ( auto found = m_internal->findSceneNode( fromBstr( name ) ).lock() )
					{
						static_cast< CSceneNode * >( *pVal )->setInternal( found.get() );
					}
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "GetNode" ),				// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetGeometry( /* [in] */ BSTR name, /* [out, retval] */ IGeometry ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CGeometry::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					if ( auto found = m_internal->findGeometry( fromBstr( name ) ).lock() )
					{
						static_cast< CGeometry * >( *pVal )->setInternal( found.get() );
					}
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "GetGeometry" ),			// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetLight( /* [in] */ BSTR name, /* [out, retval] */ ILight ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CLight::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					if ( auto found = m_internal->findLight( fromBstr( name ) ).lock() )
					{
						static_cast< CLight * >( *pVal )->setInternal( found.get() );
					}
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "GetLight" ),				// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetCamera( /* [in] */ BSTR name, /* [out, retval] */ ICamera ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CCamera::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					if ( auto found = m_internal->findCamera( fromBstr( name ) ).lock() )
					{
						static_cast< CCamera * >( *pVal )->setInternal( found.get() );
					}
				}
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "GetCamera" ),			// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveLight( /* [in] */ ILight * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( val )
			{
				m_internal->getLightCache().remove( static_cast< CLight * >( val )->getInternal()->getName() );
				static_cast< CLight * >( val )->setInternal( nullptr );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "RemoveLight" ),			// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveNode( /* [in] */ ISceneNode * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( val )
			{
				m_internal->getSceneNodeCache().remove( static_cast< CSceneNode * >( val )->getInternal()->getName() );
				static_cast< CSceneNode * >( val )->setInternal( nullptr );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "RemoveNode" ),			// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveGeometry( /* [in] */ IGeometry * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( val )
			{
				m_internal->getGeometryCache().remove( static_cast< CGeometry * >( val )->getInternal()->getName() );
				static_cast< CGeometry * >( val )->setInternal( nullptr );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "RemoveGeometry" ),		// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveCamera( /* [in] */ ICamera * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( val )
			{
				m_internal->getCameraCache().remove( static_cast< CCamera * >( val )->getInternal()->getName() );
				static_cast< CCamera * >( val )->setInternal( nullptr );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of PixelComponents throwing error
					 _T( "RemoveCamera" ),			// This is generally displayed as the title
					 scene::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}

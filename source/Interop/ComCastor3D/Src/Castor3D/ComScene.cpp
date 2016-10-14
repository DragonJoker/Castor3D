#include "ComScene.hpp"
#include "ComGeometry.hpp"
#include "ComCamera.hpp"
#include "ComLight.hpp"
#include "ComMesh.hpp"
#include "ComRenderWindow.hpp"

#include <BillboardCache.hpp>
#include <CameraCache.hpp>
#include <GeometryCache.hpp>
#include <LightCache.hpp>
#include <SceneNodeCache.hpp>
#include <MeshCache.hpp>
#include <WindowCache.hpp>

#include <Render/Viewport.hpp>

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The scene must be initialised" );

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
			m_internal->Cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "ClearScene" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CScene::SetBackgroundImage( /* [in] */ BSTR path )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			Castor::Path l_path{ FromBstr( path ) };
			m_internal->SetBackground( l_path.GetPath(), l_path.GetFileName( true ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "SetBackgroundImage" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
					static_cast< CSceneNode * >( *pVal )->SetInternal( m_internal->GetSceneNodeCache().Add( FromBstr( name ), static_cast< CSceneNode * >( parent )->GetInternal() ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "CreateSceneNode" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
					static_cast< CGeometry * >( *pVal )->SetInternal( m_internal->GetGeometryCache().Add( FromBstr( name ), nullptr, nullptr ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "CreateGeometry" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
					Castor3D::Viewport l_viewport{ *GetInternal()->GetEngine() };
					l_viewport.SetPerspective( Castor::Angle::from_degrees( 120 ), 4.0_r / 3.0_r, 0.1_r, 1000.0_r );
					l_viewport.Resize( Castor::Size( ww, wh ) );
					auto l_camera = m_internal->GetCameraCache().Add( FromBstr( name ), node ? static_cast< CSceneNode * >( node )->GetInternal() : nullptr, l_viewport );
					static_cast< CCamera * >( *pVal )->SetInternal( l_camera );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "CreateCamera" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
					static_cast< CLight * >( *pVal )->SetInternal( m_internal->GetLightCache().Add( FromBstr( name ), node ? static_cast< CSceneNode * >( node )->GetInternal() : nullptr, Castor3D::LightType( type ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "CreateLight" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateMesh( /* [in] */ eMESH_TYPE type, /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CMesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					auto l_mesh = m_internal->GetMeshCache().Add( FromBstr( name ) );
					m_internal->GetEngine()->GetMeshFactory().Create( Castor3D::eMESH_TYPE( type ) )->Generate( *l_mesh, Castor3D::UIntArray{}, Castor3D::RealArray{} );
					static_cast< CMesh * >( *pVal )->SetInternal( l_mesh );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateMesh" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CScene::CreateRenderWindow( /* [in] */ BSTR name, /* [out, retval] */ IRenderWindow ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CRenderWindow::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CRenderWindow * >( *pVal )->SetInternal( m_internal->GetRenderWindowCache().Add( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateRenderWindow" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
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
					static_cast< CSceneNode * >( *pVal )->SetInternal( m_internal->GetSceneNodeCache().Find( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "GetNode" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
					static_cast< CGeometry * >( *pVal )->SetInternal( m_internal->GetGeometryCache().Find( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "GetGeometry" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
					static_cast< CLight * >( *pVal )->SetInternal( m_internal->GetLightCache().Find( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "GetLight" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
					static_cast< CCamera * >( *pVal )->SetInternal( m_internal->GetCameraCache().Find( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "GetCamera" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
				m_internal->GetLightCache().Remove( static_cast< CLight * >( val )->GetInternal()->GetName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "RemoveLight" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
				m_internal->GetSceneNodeCache().Remove( static_cast< CSceneNode * >( val )->GetInternal()->GetName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "RemoveNode" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
				m_internal->GetGeometryCache().Remove( static_cast< CGeometry * >( val )->GetInternal()->GetName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "RemoveGeometry" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
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
				m_internal->GetCameraCache().Remove( static_cast< CCamera * >( val )->GetInternal()->GetName() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "RemoveCamera" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetBackgroundImage( /* [out, retval] */ ITextureLayout ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CTextureLayout::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CTextureLayout * >( *pVal )->SetInternal( m_internal->GetBackgroundImage() );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "GetBackgroundImage" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

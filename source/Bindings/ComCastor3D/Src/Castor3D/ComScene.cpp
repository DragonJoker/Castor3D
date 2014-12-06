#include "ComScene.hpp"
#include "ComGeometry.hpp"
#include "ComCamera.hpp"
#include "ComLight.hpp"
#include "ComTextureBase.hpp"

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
			m_internal->ClearScene();
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
			m_internal->SetBackgroundImage( FromBstr( path ) );
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
					static_cast< CSceneNode * >( *pVal )->SetInternal( m_internal->CreateSceneNode( FromBstr( name ), static_cast< CSceneNode * >( parent )->GetInternal() ) );
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
					static_cast< CGeometry * >( *pVal )->SetInternal( m_internal->CreateGeometry( FromBstr( name ) ) );
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

	STDMETHODIMP CScene::CreateCamera( /* [in] */ BSTR name, /* [in] */ int ww, /* [in] */ int wh, /* [in] */ ISceneNode * node, /* [in] */ eVIEWPORT_TYPE type, /* [out, retval] */ ICamera ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CCamera::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CCamera * >( *pVal )->SetInternal( m_internal->CreateCamera( FromBstr( name ), ww, wh, node ? static_cast< CSceneNode* >( node )->GetInternal() : nullptr, Castor3D::eVIEWPORT_TYPE( type ) ) );
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
					static_cast< CLight * >( *pVal )->SetInternal( m_internal->CreateLight( FromBstr( name ), node ? static_cast< CSceneNode* >( node )->GetInternal() : nullptr, Castor3D::eLIGHT_TYPE( type ) ) );
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
                    static_cast< CSceneNode * >( *pVal )->SetInternal( m_internal->GetNode( FromBstr( name ) ) );
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
					static_cast< CGeometry * >( *pVal )->SetInternal( m_internal->GetGeometry( FromBstr( name ) ) );
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
					static_cast< CLight * >( *pVal )->SetInternal( m_internal->GetLight( FromBstr( name ) ) );
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
					static_cast< CCamera * >( *pVal )->SetInternal( m_internal->GetCamera( FromBstr( name ) ) );
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
				m_internal->RemoveLight( static_cast< CLight * >( val )->GetInternal() );
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
				m_internal->RemoveNode( static_cast< CSceneNode * >( val )->GetInternal() );
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
				m_internal->RemoveGeometry( static_cast< CGeometry * >( val )->GetInternal() );
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
				m_internal->RemoveCamera( static_cast< CCamera * >( val )->GetInternal() );
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

	STDMETHODIMP CScene::RemoveAllLights()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->RemoveAllLights();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "RemoveAllLights" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveAllNodes()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->RemoveAllNodes();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "RemoveAllNodes" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveAllGeometries()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->RemoveAllGeometries();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "RemoveAllGeometries" ),	// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CScene::RemoveAllCameras()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->RemoveAllCameras();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IScene,					// This is the GUID of component throwing error
					 cuT( "RemoveAllCameras" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CScene::GetBackgroundImage( /* [out, retval] */ ITextureBase ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CTextureBase::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CTextureBase * >( *pVal )->SetInternal( m_internal->GetBackgroundImage() );
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

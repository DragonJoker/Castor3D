/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_ENGINE_H__
#define __COMC3D_COM_ENGINE_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Engine, C3DEngine );
	/*!
	\~english
	\brief		This class defines a CEngine object accessible from COM.
	\~french
	\brief		Cette classe définit un CEngine accessible depuis COM.
	*/
	class CEngine
		: public CComAtlObjectT< Engine, C3DEngine >
	{
	public:
		STDMETHOD( get_PluginsDirectory )( /*[out, retval]*/ BSTR * pRet )override;
		STDMETHOD( get_EngineDirectory )( /*[out, retval]*/ BSTR * pRet )override;
		STDMETHOD( get_DataDirectory )( /*[out, retval]*/ BSTR * pRet )override;

		COM_DESTROY( CEngine, c3dEngine_delete );
		STDMETHOD( Create )( /*[in]*/ BSTR appName, /*[in]*/ boolean enableValidation )override;
		STDMETHOD( Initialise )( /*[in]*/ int fps, /*[in]*/ boolean threaded )override;
		STDMETHOD( Cleanup )()override;
		STDMETHOD( ClearScenes )( )override;
		STDMETHOD( LoadRenderer )( /*[in]*/ BSTR type )override;
		STDMETHOD( LoadPlugin )( /*[in]*/ BSTR path )override;
		STDMETHOD( LoadScene )( /*[in]*/ BSTR path, /*[out, retval]*/ IRenderTarget ** pTarget )override;
		STDMETHOD( LoadImageFromFile )( /*[in]*/ BSTR name, /*[in]*/ BSTR path, /*[out, retval]*/ IImage ** pRet )override;
		STDMETHOD( LoadImageFromFormat )( /*[in]*/ BSTR name, /*[in]*/ ePIXEL_FORMAT fmt, /*[in]*/ ISize * size, /*[out, retval]*/ IImage ** pRet )override;
		STDMETHOD( LoadFontFromFile )( /*[in]*/ BSTR name, /*[in]*/ BSTR path, /*[in]*/ UINT height, /*[out, retval]*/ ICastorFont ** pRet )override;
		STDMETHOD( AddScene )( /*[in]*/ IScene * val )override;
		STDMETHOD( AddOverlay )( /*[in]*/ IOverlay * val )override;
		STDMETHOD( AddSampler )( /*[in]*/ ISampler * val )override;
		STDMETHOD( RemoveScene )( /*[in]*/ IScene * val )override;
		STDMETHOD( RemoveOverlay )( /*[in]*/ IOverlay * val )override;
		STDMETHOD( RemoveSampler )( /*[in]*/ ISampler * val )override;
		STDMETHOD( GetScene )( /*[in]*/ BSTR name, /*[out, retval]*/ IScene ** pRet )override;
		STDMETHOD( GetOverlay )( /*[in]*/ BSTR name, /*[out, retval]*/ IOverlay ** pRet )override;
		STDMETHOD( GetSampler )( /*[in]*/ BSTR name, /*[out, retval]*/ ISampler ** pRet )override;
		STDMETHOD( RenderOneFrame )( )override;
		STDMETHOD( StartRendering )( )override;
		STDMETHOD( PauseRendering )( )override;
		STDMETHOD( ResumeRendering )( )override;
		STDMETHOD( EndRendering )( )override;
		STDMETHOD( RegisterGuiCallbacks )( /*[in]*/ IGuiCallbacks * callbacks )override;
		STDMETHOD( UnregisterGuiCallbacks )()override;
		STDMETHOD( CreateOverlay )( /*[in]*/ eOVERLAY_TYPE type, /*[in]*/ BSTR name, /*[in]*/ IOverlay * parent, /*[out, retval]*/ IOverlay ** pRet )override;
		STDMETHOD( CreateRenderWindow )( /*[in]*/ BSTR name, /*[in]*/ ISize * size, /*[in]*/ LPVOID hWnd, /*[out, retval]*/ IRenderWindow ** pRet )override;
		STDMETHOD( CreateSampler )( /*[in]*/ BSTR name, /*[out, retval]*/ ISampler ** pRet )override;
		STDMETHOD( CreateScene )( /*[in]*/ BSTR name, /*[out, retval]*/ IScene ** pRet )override;
		STDMETHOD( CreateSkybox )( /*[in]*/ IScene * scene, /*[out, retval]*/ ISkybox ** pRet )override;

	private:
		void onGetClipBoardText( C3DString * text )noexcept;
		void onSetClipBoardText( C3DString text )noexcept;
		void onCursorChange( C3D_MOUSE_CURSOR cursor )noexcept;

		static void onGetClipBoardText( C3DGuiCallbacks * callbacks, C3DString * text )noexcept
		{
			if ( callbacks && callbacks->userContext )
				reinterpret_cast< CEngine * >( callbacks->userContext )->onGetClipBoardText( text );
		}

		static void onSetClipBoardText( C3DGuiCallbacks * callbacks, C3DString text )noexcept
		{
			if ( callbacks && callbacks->userContext )
				reinterpret_cast< CEngine * >( callbacks->userContext )->onSetClipBoardText( text );
		}

		static void onCursorChange( C3DGuiCallbacks * callbacks, C3D_MOUSE_CURSOR cursor )noexcept
		{
			if ( callbacks && callbacks->userContext )
				reinterpret_cast< CEngine * >( callbacks->userContext )->onCursorChange( cursor );
		}

		IGuiCallbacks * m_callbacks;
		C3DGuiCallbacks m_registered{};
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Engine ), CEngine )
}

#endif

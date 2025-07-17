/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RENDER_TARGET_H__
#define __COMC3D_COM_RENDER_TARGET_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComCamera.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( RenderTarget, C3DRenderTarget );
	/*!
	\~english
	\brief		This class defines a CRenderWindow object accessible from COM.
	\~french
	\brief		Cette classe définit un CRenderWindow accessible depuis COM.
	*/
	class ATL_NO_VTABLE CRenderTarget
		: public CComAtlObjectT< RenderTarget, C3DRenderTarget >
	{
	public:
		COM_PROPERTY_EX( ViewportType, eVIEWPORT_TYPE, C3D_VIEWPORT_TYPE, c3dRenderTarget_getViewportType, c3dRenderTarget_setViewportType );
		COM_PROPERTY_MPTR( Camera, ICamera, c3dRenderTarget_getCamera, c3dRenderTarget_setCamera );
		COM_PROPERTY_MPTR( Scene, IScene, c3dRenderTarget_getScene, c3dRenderTarget_setScene );
		COM_PROPERTY_GET_EX( PixelFormat, ePIXEL_FORMAT, C3D_PIXEL_FORMAT, c3dRenderTarget_getPixelFormat );
		COM_PROPERTY_GET_SPTR( RenderSize, ISize, c3dRenderTarget_getRenderSize );
		COM_PROPERTY_GET_SPTR( DisplaySize, ISize, c3dRenderTarget_getDisplaySize );

		COM_DESTROY( CRenderTarget, c3dRenderTarget_delete );
		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderTarget ), CRenderTarget );
}

#endif

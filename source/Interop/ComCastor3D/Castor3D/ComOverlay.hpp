/* See LICENSE file in root folder */
#ifndef __COMC3D_Overlay_H__
#define __COMC3D_Overlay_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComBorderPanelOverlay.hpp"
#include "ComCastor3D/Castor3D/ComPanelOverlay.hpp"
#include "ComCastor3D/Castor3D/ComTextOverlay.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Overlay, C3DOverlay );
	/*!
	\~english
	\brief		This class defines a COverlay object accessible from COM.
	\~french
	\brief		Cette classe définit un COverlay accessible depuis COM.
	*/
	class COverlay
		: public CComAtlObjectT< Overlay, C3DOverlay >
	{
	public:
		COM_PROPERTY_GET_EX( Name, BSTR, C3DString, c3dOverlay_getName );
		COM_PROPERTY_GET_EX( Type, eOVERLAY_TYPE, C3D_OVERLAY_TYPE, c3dOverlay_getOverlayType );
		COM_PROPERTY_GET_MPTR( PanelOverlay, IPanelOverlay, c3dOverlay_getPanelOverlay );
		COM_PROPERTY_GET_MPTR( BorderPanelOverlay, IBorderPanelOverlay, c3dOverlay_getBorderPanelOverlay );
		COM_PROPERTY_GET_MPTR( TextOverlay, ITextOverlay, c3dOverlay_getTextOverlay );

		COM_DESTROY( COverlay, c3dOverlay_delete );
		STDMETHOD( Create )( /*[in]*/ IEngine * engine, /*[in]*/ eOVERLAY_TYPE type, /*[in]*/ BSTR name, /*[in]*/ IOverlay * parent, /*[in]*/ IScene * scene )override;
		STDMETHOD( GetChildrenCount )( /*[in]*/ UINT level, /*[out, retval]*/ UINT * pRet )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Overlay ), COverlay );
}

#endif

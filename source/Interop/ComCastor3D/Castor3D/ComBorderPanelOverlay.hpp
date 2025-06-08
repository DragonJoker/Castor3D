/* See LICENSE file in root folder */
#ifndef __COMC3D_BorderPanelOverlay_H__
#define __COMC3D_BorderPanelOverlay_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComMaterial.hpp"
#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( BorderPanelOverlay, C3DBorderPanelOverlay );
	/*!
	\~english
	\brief		This class defines a CBorderPanelOverlay object accessible from COM.
	\~french
	\brief		Cette classe définit un CBorderPanelOverlay accessible depuis COM.
	*/
	class CBorderPanelOverlay
		: public CComAtlObjectT< BorderPanelOverlay, C3DBorderPanelOverlay >
	{
	public:
		COM_DESTROY( CBorderPanelOverlay, c3dBorderPanelOverlay_delete );

		COM_PROPERTY_SPTR( Position, IPosition, c3dBorderPanelOverlay_getPosition, c3dBorderPanelOverlay_setPosition );
		COM_PROPERTY_SPTR( Size, ISize, c3dBorderPanelOverlay_getSize, c3dBorderPanelOverlay_setSize );
		COM_PROPERTY_EX( Visible, boolean, bool, c3dBorderPanelOverlay_getVisible, c3dBorderPanelOverlay_setVisible );
		COM_PROPERTY_MPTR( Material, IMaterial, c3dBorderPanelOverlay_getMaterial, c3dBorderPanelOverlay_setMaterial );

		COM_PROPERTY( LeftBorderSize, UINT, c3dBorderPanelOverlay_getLeftBorderSize, c3dBorderPanelOverlay_setLeftBorderSize );
		COM_PROPERTY( RightBorderSize, UINT, c3dBorderPanelOverlay_getRightBorderSize, c3dBorderPanelOverlay_setRightBorderSize );
		COM_PROPERTY( TopBorderSize, UINT, c3dBorderPanelOverlay_getTopBorderSize, c3dBorderPanelOverlay_setTopBorderSize );
		COM_PROPERTY( BottomBorderSize, UINT, c3dBorderPanelOverlay_getBottomBorderSize, c3dBorderPanelOverlay_setBottomBorderSize );
		COM_PROPERTY_MPTR( BorderMaterial, IMaterial, c3dBorderPanelOverlay_getBorderMaterial, c3dBorderPanelOverlay_setBorderMaterial );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( BorderPanelOverlay ), CBorderPanelOverlay );
}

#endif

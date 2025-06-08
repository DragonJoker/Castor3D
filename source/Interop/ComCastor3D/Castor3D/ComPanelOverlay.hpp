/* See LICENSE file in root folder */
#ifndef __COMC3D_PanelOverlay_H__
#define __COMC3D_PanelOverlay_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComMaterial.hpp"
#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( PanelOverlay, C3DPanelOverlay );
	/*!
	\~english
	\brief		This class defines a CPanelOverlay object accessible from COM.
	\~french
	\brief		Cette classe définit un CPanelOverlay accessible depuis COM.
	*/
	class CPanelOverlay
		: public CComAtlObjectT< PanelOverlay, C3DPanelOverlay >
	{
	public:
		COM_PROPERTY_SPTR( Position, IPosition, c3dPanelOverlay_getPosition, c3dPanelOverlay_setPosition );
		COM_PROPERTY_SPTR( Size, ISize, c3dPanelOverlay_getSize, c3dPanelOverlay_setSize );
		COM_PROPERTY_EX( Visible, boolean, bool, c3dPanelOverlay_getVisible, c3dPanelOverlay_setVisible );
		COM_PROPERTY_MPTR( Material, IMaterial, c3dPanelOverlay_getMaterial, c3dPanelOverlay_setMaterial );

		COM_DESTROY( CPanelOverlay, c3dPanelOverlay_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PanelOverlay ), CPanelOverlay );
}

#endif

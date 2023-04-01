/* See LICENSE file in root folder */
#ifndef __COMC3D_Overlay_H__
#define __COMC3D_Overlay_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Overlay/Overlay.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Overlay );
	/*!
	\~english
	\brief		This class defines a COverlay object accessible from COM.
	\~french
	\brief		Cette classe définit un COverlay accessible depuis COM.
	*/
	class COverlay
		: public CComAtlObject< Overlay, castor3d::Overlay >
	{
	public:
		COMEX_PROPERTY_GET( Name, BSTR, m_internal, &castor3d::Overlay::getName );
		COMEX_PROPERTY_GET( PanelOverlay, IPanelOverlay *, m_internal, &castor3d::Overlay::getPanelOverlay );
		COMEX_PROPERTY_GET( BorderPanelOverlay, IBorderPanelOverlay *, m_internal, &castor3d::Overlay::getBorderPanelOverlay );
		COMEX_PROPERTY_GET( TextOverlay, ITextOverlay *, m_internal, &castor3d::Overlay::getTextOverlay );
		COMEX_PROPERTY_GET( GetChildrenCount, unsigned int, m_internal, &castor3d::Overlay::getChildrenCount );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Overlay ), COverlay );
}

#endif

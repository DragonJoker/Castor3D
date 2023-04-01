/* See LICENSE file in root folder */
#ifndef __COMC3D_PanelOverlay_H__
#define __COMC3D_PanelOverlay_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <Castor3D/Overlay/PanelOverlay.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, PanelOverlay );
	/*!
	\~english
	\brief		This class defines a CPanelOverlay object accessible from COM.
	\~french
	\brief		Cette classe définit un CPanelOverlay accessible depuis COM.
	*/
	class CPanelOverlay
		: public CComAtlObject< PanelOverlay, castor3d::PanelOverlay >
	{
	public:
		static bool isVisible( castor3d::PanelOverlay const * ov );
		static void setVisible( castor3d::PanelOverlay * ov, bool v );

		COMEX_PROPERTY_GET( Type, eOVERLAY_TYPE, m_internal, &castor3d::PanelOverlay::getType );
		COMEX_PROPERTY( Position, IPosition *, m_internal, &castor3d::PanelOverlay::getPixelPosition, &castor3d::PanelOverlay::setPixelPosition );
		COMEX_PROPERTY( Size, ISize *, m_internal, &castor3d::PanelOverlay::getPixelSize, &castor3d::PanelOverlay::setPixelSize );
		COMEX_PROPERTY( Visible, boolean, m_internal, isVisible, setVisible );

		STDMETHOD( get_Material )( /* [out, retval] */ IMaterial ** pVal );
		STDMETHOD( put_Material )( /* [in] */ IMaterial * val );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PanelOverlay ), CPanelOverlay );
}

#endif

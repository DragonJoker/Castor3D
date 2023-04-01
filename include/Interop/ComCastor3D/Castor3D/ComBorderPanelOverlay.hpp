/* See LICENSE file in root folder */
#ifndef __COMC3D_BorderPanelOverlay_H__
#define __COMC3D_BorderPanelOverlay_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <Castor3D/Overlay/BorderPanelOverlay.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, BorderPanelOverlay );
	/*!
	\~english
	\brief		This class defines a CBorderPanelOverlay object accessible from COM.
	\~french
	\brief		Cette classe définit un CBorderPanelOverlay accessible depuis COM.
	*/
	class CBorderPanelOverlay
		: public CComAtlObject< BorderPanelOverlay, castor3d::BorderPanelOverlay >
	{
	public:
		COMEX_PROPERTY_GET( Type, eOVERLAY_TYPE, m_internal, &castor3d::BorderPanelOverlay::getType );
		COMEX_PROPERTY( Position, IPosition *, m_internal, &castor3d::BorderPanelOverlay::getPixelPosition, &castor3d::BorderPanelOverlay::setPixelPosition );
		COMEX_PROPERTY( Size, ISize *, m_internal, &castor3d::BorderPanelOverlay::getPixelSize, &castor3d::BorderPanelOverlay::setPixelSize );
		COMEX_PROPERTY( Visible, boolean, m_internal, &castor3d::BorderPanelOverlay::isVisible, &castor3d::BorderPanelOverlay::setVisible );

		COMEX_PROPERTY( LeftBorderSize, unsigned int, m_internal, &castor3d::BorderPanelOverlay::getPixelLeftBorderSize, &castor3d::BorderPanelOverlay::setPixelLeftBorderSize );
		COMEX_PROPERTY( RightBorderSize, unsigned int, m_internal, &castor3d::BorderPanelOverlay::getPixelRightBorderSize, &castor3d::BorderPanelOverlay::setPixelRightBorderSize );
		COMEX_PROPERTY( TopBorderSize, unsigned int, m_internal, &castor3d::BorderPanelOverlay::getPixelTopBorderSize, &castor3d::BorderPanelOverlay::setPixelTopBorderSize );
		COMEX_PROPERTY( BottomBorderSize, unsigned int, m_internal, &castor3d::BorderPanelOverlay::getPixelBottomBorderSize, &castor3d::BorderPanelOverlay::setPixelBottomBorderSize );

		STDMETHOD( get_Material )( /* [out, retval] */ IMaterial ** pVal );
		STDMETHOD( put_Material )( /* [in] */ IMaterial * val );
		STDMETHOD( get_BorderMaterial )( /* [out, retval] */ IMaterial ** pVal );
		STDMETHOD( put_BorderMaterial )( /* [in] */ IMaterial * val );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( BorderPanelOverlay ), CBorderPanelOverlay );
}

#endif

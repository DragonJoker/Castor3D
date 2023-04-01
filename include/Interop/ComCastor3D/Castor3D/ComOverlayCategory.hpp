/* See LICENSE file in root folder */
#ifndef __COMC3D_OverlayCategory_H__
#define __COMC3D_OverlayCategory_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <Castor3D/Overlay/OverlayCategory.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, OverlayCategory );
	/*!
	\~english
	\brief		This class defines a COverlayCategory object accessible from COM.
	\~french
	\brief		Cette classe définit un COverlayCategory accessible depuis COM.
	*/
	class COverlayCategory
		: public CComAtlObject< OverlayCategory, castor3d::OverlayCategory >
	{
	public:
		COMEX_PROPERTY_GET( Type, eOVERLAY_TYPE, m_internal, &castor3d::OverlayCategory::getType );
		COMEX_PROPERTY( Position, IPosition *, m_internal, &castor3d::OverlayCategory::getPixelPosition, &castor3d::OverlayCategory::setPixelPosition );
		COMEX_PROPERTY( Size, ISize *, m_internal, &castor3d::OverlayCategory::getPixelSize, &castor3d::OverlayCategory::setPixelSize );
		COMEX_PROPERTY( Visible, boolean, m_internal, &castor3d::OverlayCategory::isVisible, &castor3d::OverlayCategory::setVisible );

		STDMETHOD( get_Material )( /* [out, retval] */ IMaterial ** pVal );
		STDMETHOD( put_Material )( /* [in] */ IMaterial * val );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( OverlayCategory ), COverlayCategory );
}

#endif

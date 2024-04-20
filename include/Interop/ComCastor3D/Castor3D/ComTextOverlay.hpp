/* See LICENSE file in root folder */
#ifndef __COMC3D_TextOverlay_H__
#define __COMC3D_TextOverlay_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <Castor3D/Overlay/TextOverlay.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, TextOverlay );
	/*!
	\~english
	\brief		This class defines a CTextOverlay object accessible from COM.
	\~french
	\brief		Cette classe définit un CTextOverlay accessible depuis COM.
	*/
	class CTextOverlay
		: public CComAtlObject< TextOverlay, castor3d::TextOverlay >
	{
	public:
		COMEX_PROPERTY_GET( Type, eOVERLAY_TYPE, m_internal, &castor3d::TextOverlay::getType );
		COMEX_PROPERTY( Position, IPosition *, m_internal, &castor3d::TextOverlay::getPixelPosition, &castor3d::TextOverlay::setPixelPosition );
		COMEX_PROPERTY( Size, ISize *, m_internal, &castor3d::TextOverlay::getPixelSize, &castor3d::TextOverlay::setPixelSize );
		COMEX_PROPERTY( Visible, boolean, m_internal, &castor3d::TextOverlay::isVisible, &castor3d::TextOverlay::setVisible );
		COMEX_PROPERTY( Caption, BSTR, m_internal, &castor3d::TextOverlay::getCaption, &castor3d::TextOverlay::setCaption );

		STDMETHOD( get_Font )( /* [out, retval] */ BSTR * pVal );
		STDMETHOD( put_Font )( /* [in] */ BSTR val );
		STDMETHOD( get_Material )( /* [out, retval] */ IMaterial ** pVal );
		STDMETHOD( put_Material )( /* [in] */ IMaterial * val );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextOverlay ), CTextOverlay );
}

#endif

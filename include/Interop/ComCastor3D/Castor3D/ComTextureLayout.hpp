/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_LAYOUT_H__
#define __COMC3D_COM_TEXTURE_LAYOUT_H__

#include "ComCastor3D/CastorUtils/ComPixelBuffer.hpp"

#include <Castor3D/Material/Texture/TextureLayout.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, TextureLayout );
	/*!
	\~english
	\brief		This class defines a CTextureLayout object accessible from COM.
	\~french
	\brief		Cette classe définit un CTextureLayout accessible depuis COM.
	*/
	class CTextureLayout
		: public CComAtlObject< TextureLayout, castor3d::TextureLayout >
	{
	public:
		COMEX_PROPERTY_GET( Type, eIMAGE_TYPE, m_internal, &castor3d::TextureLayout::getType );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

		STDMETHOD( InitFromFile )( /* [in] */ BSTR path );
		STDMETHOD( InitFromBuffer )( /* [in] */ IPixelBuffer * val );
		STDMETHOD( Init2D )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ ePIXEL_FORMAT format );
		STDMETHOD( Init3D )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ ePIXEL_FORMAT format );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureLayout ), CTextureLayout );
}

#endif

/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_LAYOUT_H__
#define __COMC3D_COM_TEXTURE_LAYOUT_H__

#include "ComSampler.hpp"
#include "ComPixelBuffer.hpp"
#include "ComTextureImage.hpp"

#include <Texture/TextureLayout.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CTextureLayout object accessible from COM.
	\~french
	\brief		Cette classe définit un CTextureLayout accessible depuis COM.
	*/
	class ATL_NO_VTABLE CTextureLayout
		:	COM_ATL_OBJECT( TextureLayout )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CTextureLayout();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CTextureLayout();

		inline castor3d::TextureLayoutSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::TextureLayoutSPtr state )
		{
			m_internal = state;
		}

		COM_PROPERTY_INDEXED_GET( Image, unsigned int, ITextureImage *, make_indexed_getter( m_internal.get(), &castor3d::TextureLayout::getImage ) );

		COM_PROPERTY_GET( Type, eTEXTURE_TYPE, make_getter( m_internal.get(), &castor3d::TextureLayout::getType ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

	private:
		castor3d::TextureLayoutSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureLayout ), CTextureLayout );

	DECLARE_VARIABLE_PTR_GETTER( TextureLayout, castor3d, TextureLayout );
	DECLARE_VARIABLE_PTR_PUTTER( TextureLayout, castor3d, TextureLayout );
}

#endif

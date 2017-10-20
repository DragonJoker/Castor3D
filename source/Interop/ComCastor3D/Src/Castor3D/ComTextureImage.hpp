/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_IMAGE_H__
#define __COMC3D_COM_TEXTURE_IMAGE_H__

#include "ComSampler.hpp"
#include "ComPixelBuffer.hpp"

#include <Texture/TextureImage.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CTexture object accessible from COM.
	\~french
	\brief		Cette classe définit un CTexture accessible depuis COM.
	*/
	class ATL_NO_VTABLE CTextureImage
		:	COM_ATL_OBJECT( TextureImage )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CTextureImage();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CTextureImage();

		inline castor3d::TextureImageSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::TextureImageSPtr state )
		{
			m_internal = state;
		}

		COM_PROPERTY_GET( Buffer, IPixelBuffer *, make_getter( m_internal.get(), &castor3d::TextureImage::getBuffer ) );

		STDMETHOD( Resize2D )( /* [in] */ unsigned int w, /* [in] */ unsigned int h );
		STDMETHOD( Resize3D )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d );
		STDMETHOD( StaticSource )( /* [in] */ IPixelBuffer * val );
		STDMETHOD( Dynamic2DSource )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ ePIXEL_FORMAT format );
		STDMETHOD( Dynamic3DSource )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ ePIXEL_FORMAT format );

	private:
		castor3d::TextureImageSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureImage ), CTextureImage );

	DECLARE_VARIABLE_PTR_GETTER( TextureImage, castor3d, TextureImage );
}

#endif

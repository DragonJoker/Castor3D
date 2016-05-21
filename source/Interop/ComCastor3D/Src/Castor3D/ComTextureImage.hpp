/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
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

		inline Castor3D::TextureImageSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::TextureImageSPtr state )
		{
			m_internal = state;
		}

		COM_PROPERTY_GET( Buffer, IPixelBuffer *, make_getter( m_internal.get(), &Castor3D::TextureImage::GetBuffer ) );
		COM_PROPERTY_GET( Width, unsigned int, make_getter( m_internal.get(), &Castor3D::TextureImage::GetWidth ) );
		COM_PROPERTY_GET( Height, unsigned int, make_getter( m_internal.get(), &Castor3D::TextureImage::GetHeight ) );
		COM_PROPERTY_GET( Depth, unsigned int, make_getter( m_internal.get(), &Castor3D::TextureImage::GetDepth ) );

		STDMETHOD( Initialise )( /* [in] */ eTEXTURE_TYPE p_type, /* [in] */ unsigned int p_cpuAccess, /* [in] */ unsigned int p_gpuAccess );
		STDMETHOD( Cleanup )();
		STDMETHOD( Resize2D )( /* [in] */ unsigned int w, /* [in] */ unsigned int h );
		STDMETHOD( Resize3D )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d );
		STDMETHOD( Static2DSource )( /* [in] */ IPixelBuffer * val );
		STDMETHOD( Static3DSource )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ IPixelBuffer * val );
		STDMETHOD( Dynamic2DSource )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ ePIXEL_FORMAT format );
		STDMETHOD( Dynamic3DSource )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ ePIXEL_FORMAT format );

	private:
		Castor3D::TextureImageSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureImage ), CTextureImage );

	DECLARE_VARIABLE_PTR_GETTER( TextureImage, Castor3D, TextureImage );
}

#endif

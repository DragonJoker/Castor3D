/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

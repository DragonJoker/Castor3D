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
#ifndef __COMC3D_COM_IMAGE_H__
#define __COMC3D_COM_IMAGE_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include "ComPixelBuffer.hpp"
#include <Graphics/Image.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CImage object accessible from COM.
	\~french
	\brief		Cette classe définit un CImage accessible depuis COM.
	*/
	class ATL_NO_VTABLE CImage
		:	COM_ATL_OBJECT( Image )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CImage();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CImage();

		inline void setInternal( castor::ImageSPtr p_image )
		{
			m_image = p_image;
		}

		inline castor::ImageSPtr getInternal()const
		{
			return m_image;
		}

		COM_PROPERTY_GET( Buffer, IPixelBuffer *, make_getter( m_image.get(), &castor::Image::getPixels ) );

		STDMETHOD( LoadFromFile )( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ BSTR val );
		STDMETHOD( LoadFromFormat )( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ ePIXEL_FORMAT fmt, /* [in] */ ISize * size );
		STDMETHOD( resample )( /* [in] */ ISize * val );
		STDMETHOD( Fill )( /* [in] */ IColour * val );
		STDMETHOD( copyImage )( /* [in] */ IImage * pVal );
		STDMETHOD( subImage )( /* [in] */ IRect * val, /* [out, retval] */ IImage ** pVal );
		STDMETHOD( Flip )( /* [out, retval] */ IImage ** pVal );
		STDMETHOD( Mirror )( /* [out, retval] */ IImage ** pVal );

	private:
		castor::ImageSPtr m_image;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Image ), CImage );

	DECLARE_VARIABLE_PTR_GETTER( Image, castor, Image );
	DECLARE_VARIABLE_PTR_PUTTER( Image, castor, Image );
}

#endif

/* See LICENSE file in root folder */
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

		COM_PROPERTY_GET( Buffer, IPixelBuffer *, makeGetter( m_image.get(), &castor::Image::getPixels ) );

		STDMETHOD( LoadFromFile )( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ BSTR val );
		STDMETHOD( LoadFromFormat )( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ ePIXEL_FORMAT fmt, /* [in] */ ISize * size );
		STDMETHOD( Resample )( /* [in] */ ISize * val );
		STDMETHOD( Fill )( /* [in] */ IRgbaColour * val );
		STDMETHOD( CopyImage )( /* [in] */ IImage * pVal );
		STDMETHOD( SubImage )( /* [in] */ IRect * val, /* [out, retval] */ IImage ** pVal );
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

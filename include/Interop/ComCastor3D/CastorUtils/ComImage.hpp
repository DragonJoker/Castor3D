/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_IMAGE_H__
#define __COMC3D_COM_IMAGE_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComPixelBuffer.hpp"

#include <CastorUtils/Graphics/Image.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_SPTR( castor, Image );
	/*!
	\~english
	\brief		This class defines a CImage object accessible from COM.
	\~french
	\brief		Cette classe définit un CImage accessible depuis COM.
	*/
	class CImage
		: public CComAtlObject< Image, castor::Image >
	{
	public:
		COM_PROPERTY_GET( Buffer, IPixelBuffer *, makeGetter( m_internal.get(), &castor::Image::getPixels ) );

		STDMETHOD( LoadFromFile )( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ BSTR val, /* [in] */ boolean dropAlpha );
		STDMETHOD( LoadFromFormat )( /* [in] */ IEngine * engine, /* [in] */ BSTR name, /* [in] */ ePIXEL_FORMAT fmt, /* [in] */ ISize * size );
		STDMETHOD( Resample )( /* [in] */ ISize * val );
		STDMETHOD( Fill )( /* [in] */ IRgbaColour * val );
		STDMETHOD( Flip )( /* [out, retval] */ IImage ** pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Image ), CImage );
}

#endif

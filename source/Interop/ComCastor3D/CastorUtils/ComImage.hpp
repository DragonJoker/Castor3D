/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_IMAGE_H__
#define __COMC3D_COM_IMAGE_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComPixelBuffer.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Image, C3DImage );
	/*!
	\~english
	\brief		This class defines a CImage object accessible from COM.
	\~french
	\brief		Cette classe définit un CImage accessible depuis COM.
	*/
	class CImage
		: public CComAtlObjectT< Image, C3DImage >
	{
	public:
		COM_PROPERTY_GET_MPTR( Buffer, IPixelBuffer, c3dImage_getBuffer );

		COM_DESTROY( CImage, c3dImage_delete );
		STDMETHOD( Resample )( /*[in]*/ ISize * val )override;
		STDMETHOD( Fill )( /*[in]*/ IRgbaColour * val )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Image ), CImage );
}

#endif

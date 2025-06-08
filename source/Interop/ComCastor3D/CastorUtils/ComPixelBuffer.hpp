/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_PIXEL_BUFFER_H__
#define __COMC3D_COM_PIXEL_BUFFER_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( PixelBuffer, C3DPixelBuffer );
	/*!
	\~english
	\brief		This class defines a CImage object accessible from COM.
	\~french
	\brief		Cette classe définit un CImage accessible depuis COM.
	*/
	class CPixelBuffer
		: public CComAtlObjectT< PixelBuffer, C3DPixelBuffer >
	{
	public:
		COM_PROPERTY_GET_SPTR( Dimensions, ISize, c3dPixelBuffer_getDimensions );
		COM_PROPERTY_GET( Width, UINT, c3dPixelBuffer_getWidth );
		COM_PROPERTY_GET( Height, UINT, c3dPixelBuffer_getHeight );
		COM_PROPERTY_GET_EX( PixelFormat, ePIXEL_FORMAT, C3D_PIXEL_FORMAT, c3dPixelBuffer_getPixelFormat );

		COM_DESTROY( CPixelBuffer, c3dPixelBuffer_delete );
		STDMETHOD( Create )( /*[in]*/ ISize * size, /*[in]*/ ePIXEL_FORMAT format )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PixelBuffer ), CPixelBuffer );
}

#endif

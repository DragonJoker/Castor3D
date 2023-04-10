/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_PIXEL_BUFFER_H__
#define __COMC3D_COM_PIXEL_BUFFER_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR_EX( PixelBuffer, castor, PxBufferBase );
	/*!
	\~english
	\brief		This class defines a CImage object accessible from COM.
	\~french
	\brief		Cette classe définit un CImage accessible depuis COM.
	*/
	class CPixelBuffer
		: public CComAtlObject< PixelBuffer, castor::PxBufferBase >
	{
	public:
		COMEX_PROPERTY_GET( PixelFormat, ePIXEL_FORMAT, m_internal, &castor::PxBufferBase::getFormat );
		COMEX_PROPERTY_GET( Width, UINT, m_internal, &castor::PxBufferBase::getWidth );
		COMEX_PROPERTY_GET( Height, UINT, m_internal, &castor::PxBufferBase::getHeight );
		COMEX_PROPERTY_GET( Dimensions, ISize *, m_internal, &castor::PxBufferBase::getDimensions );

		STDMETHOD( Flip )();
		STDMETHOD( Initialise )( /* [in] */ ISize * size, /* [in] */ ePIXEL_FORMAT format );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PixelBuffer ), CPixelBuffer );
}

#endif

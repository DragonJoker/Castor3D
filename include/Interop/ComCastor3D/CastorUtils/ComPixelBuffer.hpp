/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_PIXEL_BUFFER_H__
#define __COMC3D_COM_PIXEL_BUFFER_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

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
	class ATL_NO_VTABLE CPixelBuffer
		:	COM_ATL_OBJECT( PixelBuffer )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CPixelBuffer();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CPixelBuffer();

		inline void setInternal( castor::PxBufferBaseSPtr p_buffer )
		{
			m_buffer = p_buffer;
		}

		inline castor::PxBufferBaseSPtr getInternal()const
		{
			return m_buffer;
		}

		COM_PROPERTY_GET( PixelFormat, ePIXEL_FORMAT, makeGetter( m_buffer.get(), &castor::PxBufferBase::getFormat ) );
		COM_PROPERTY_GET( Width, UINT, makeGetter( m_buffer.get(), &castor::PxBufferBase::getWidth ) );
		COM_PROPERTY_GET( Height, UINT, makeGetter( m_buffer.get(), &castor::PxBufferBase::getHeight ) );
		COM_PROPERTY_GET( Dimensions, ISize *, makeGetter( m_buffer.get(), &castor::PxBufferBase::getDimensions ) );

		STDMETHOD( Flip )();
		STDMETHOD( Mirror )();
		STDMETHOD( Initialise )( /* [in] */ ISize * size, /* [in] */ ePIXEL_FORMAT format );

	private:
		castor::PxBufferBaseSPtr m_buffer;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PixelBuffer ), CPixelBuffer );

	DECLARE_VARIABLE_PTR_GETTER( PixelBuffer, castor, PxBufferBase );
	DECLARE_VARIABLE_PTR_PUTTER( PixelBuffer, castor, PxBufferBase );
}

#endif

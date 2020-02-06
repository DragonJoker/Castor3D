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
#ifndef __COMC3D_COM_PIXEL_BUFFER_H__
#define __COMC3D_COM_PIXEL_BUFFER_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include "ComSize.hpp"
#include <PixelBuffer.hpp>

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

		inline void SetInternal( Castor::PxBufferBaseSPtr p_buffer )
		{
			m_buffer = p_buffer;
		}

		inline Castor::PxBufferBaseSPtr GetInternal()const
		{
			return m_buffer;
		}

		COM_PROPERTY_GET( PixelFormat, ePIXEL_FORMAT, make_getter( m_buffer.get(), &Castor::PxBufferBase::format ) );
		COM_PROPERTY_GET( Width, UINT, make_getter( m_buffer.get(), &Castor::PxBufferBase::width ) );
		COM_PROPERTY_GET( Height, UINT, make_getter( m_buffer.get(), &Castor::PxBufferBase::height ) );
		COM_PROPERTY_GET( Dimensions, ISize *, make_getter( m_buffer.get(), &Castor::PxBufferBase::dimensions ) );

		STDMETHOD( Flip )();
		STDMETHOD( Mirror )();
		STDMETHOD( Initialise )( /* [in] */ ISize * size, /* [in] */ ePIXEL_FORMAT format );

	private:
		Castor::PxBufferBaseSPtr m_buffer;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PixelBuffer ), CPixelBuffer );

	DECLARE_VARIABLE_PTR_GETTER( PixelBuffer, Castor, PxBufferBase );
	DECLARE_VARIABLE_PTR_PUTTER( PixelBuffer, Castor, PxBufferBase );
}

#endif

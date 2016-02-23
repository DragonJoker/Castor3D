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
#ifndef __COMC3D_COM_TEXTURE_BASE_H__
#define __COMC3D_COM_TEXTURE_BASE_H__

#include "ComSampler.hpp"
#include "ComPixelBuffer.hpp"

#include <Texture.hpp>

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
	class ATL_NO_VTABLE CTexture
		:	COM_ATL_OBJECT( Texture )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CTexture();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CTexture();

		inline Castor3D::TextureSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::TextureSPtr state )
		{
			m_internal = state;
		}

		COM_PROPERTY( Type, eTEXTURE_TYPE, make_getter( m_internal.get(), &Castor3D::Texture::GetType ), make_putter( m_internal.get(), &Castor3D::Texture::SetType ) );
		COM_PROPERTY( Sampler, ISampler *, make_getter( m_internal.get(), &Castor3D::Texture::GetSampler ), make_putter( m_internal.get(), &Castor3D::Texture::SetSampler ) );
		COM_PROPERTY( MappingMode, eTEXTURE_MAP_MODE, make_getter( m_internal.get(), &Castor3D::Texture::GetMappingMode ), make_putter( m_internal.get(), &Castor3D::Texture::SetMappingMode ) );
		COM_PROPERTY( Buffer, IPixelBuffer *, make_getter( m_internal.get(), &Castor3D::Texture::GetBuffer ), make_putter( m_internal.get(), &Castor3D::Texture::SetImage ) );

		COM_PROPERTY_GET( BaseType, eTEXTURE_BASE_TYPE, make_getter( m_internal.get(), &Castor3D::Texture::GetBaseType ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
	private:
		Castor3D::TextureSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Texture ), CTexture );

	DECLARE_VARIABLE_PTR_GETTER( Texture, Castor3D, Texture );
	DECLARE_VARIABLE_PTR_PUTTER( Texture, Castor3D, Texture );
}

#endif

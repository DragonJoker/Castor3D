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
#ifndef __COMC3D_COM_TEXTURE_LAYOUT_H__
#define __COMC3D_COM_TEXTURE_LAYOUT_H__

#include "ComSampler.hpp"
#include "ComPixelBuffer.hpp"
#include "ComTextureImage.hpp"

#include <Texture/TextureLayout.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CTextureLayout object accessible from COM.
	\~french
	\brief		Cette classe définit un CTextureLayout accessible depuis COM.
	*/
	class ATL_NO_VTABLE CTextureLayout
		:	COM_ATL_OBJECT( TextureLayout )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CTextureLayout();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CTextureLayout();

		inline Castor3D::TextureLayoutSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::TextureLayoutSPtr state )
		{
			m_internal = state;
		}

		COM_PROPERTY_INDEXED_GET( Image, unsigned int, ITextureImage *, make_indexed_getter( m_internal.get(), &Castor3D::TextureLayout::GetImage ) );

		COM_PROPERTY_GET( Type, eTEXTURE_TYPE, make_getter( m_internal.get(), &Castor3D::TextureLayout::GetType ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();

	private:
		Castor3D::TextureLayoutSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureLayout ), CTextureLayout );

	DECLARE_VARIABLE_PTR_GETTER( TextureLayout, Castor3D, TextureLayout );
	DECLARE_VARIABLE_PTR_PUTTER( TextureLayout, Castor3D, TextureLayout );
}

#endif

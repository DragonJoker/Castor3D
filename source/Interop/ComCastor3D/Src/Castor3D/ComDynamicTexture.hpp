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
#ifndef __COMC3D_COM_DYNAMIC_TEXTURE_H__
#define __COMC3D_COM_DYNAMIC_TEXTURE_H__

#include "ComTexture.hpp"
#include "ComRenderTarget.hpp"

#include <Texture/DynamicTexture.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CDynamicTexture object accessible from COM.
	\~french
	\brief		Cette classe définit un CDynamicTexture accessible depuis COM.
	*/
	class ATL_NO_VTABLE CDynamicTexture
		:	COM_ATL_OBJECT( DynamicTexture )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CDynamicTexture();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CDynamicTexture();

		inline Castor3D::DynamicTextureSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::DynamicTextureSPtr state )
		{
			m_internal = state;
		}

		COM_PROPERTY( RenderTarget, IRenderTarget *, make_getter( m_internal.get(), &Castor3D::DynamicTexture::GetRenderTarget ), make_putter( m_internal.get(), &Castor3D::DynamicTexture::SetRenderTarget ) );
		COM_PROPERTY( Type, eTEXTURE_TYPE, make_getter( m_internal.get(), &Castor3D::Texture::GetType ), make_putter( m_internal.get(), &Castor3D::Texture::SetType ) );
		COM_PROPERTY( MappingMode, eTEXTURE_MAP_MODE, make_getter( m_internal.get(), &Castor3D::Texture::GetMappingMode ), make_putter( m_internal.get(), &Castor3D::Texture::SetMappingMode ) );
		COM_PROPERTY( Buffer, IPixelBuffer *, make_getter( m_internal.get(), &Castor3D::Texture::GetBuffer ), make_putter( m_internal.get(), &Castor3D::Texture::SetImage ) );
		COM_PROPERTY( SamplesCount, unsigned int, make_getter( m_internal.get(), &Castor3D::DynamicTexture::GetSamplesCount ), make_putter( m_internal.get(), &Castor3D::DynamicTexture::SetSamplesCount ) );

		COM_PROPERTY_GET( BaseType, eTEXTURE_BASE_TYPE, make_getter( m_internal.get(), &Castor3D::Texture::GetBaseType ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
		STDMETHOD( Set2DImage )( /* [in] */ ISize * size, /* [in] */ ePIXEL_FORMAT fmt );
		STDMETHOD( Set3DImage )( /* [in] */ ISize * size, /* [in] */ unsigned int depth, /* [in] */ ePIXEL_FORMAT fmt );
		STDMETHOD( Resize2D )( /* [in] */ ISize * size );
		STDMETHOD( Resize3D )( /* [in] */ ISize * size, /* [in] */unsigned int depth );

	private:
		Castor3D::DynamicTextureSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( DynamicTexture ), CDynamicTexture );

	DECLARE_VARIABLE_PTR_GETTER( DynamicTexture, Castor3D, DynamicTexture );
	DECLARE_VARIABLE_PTR_PUTTER( DynamicTexture, Castor3D, DynamicTexture );
}

#endif

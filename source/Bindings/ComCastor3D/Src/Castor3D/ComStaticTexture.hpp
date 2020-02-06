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
#ifndef __COMC3D_COM_STATIC_TEXTURE_H__
#define __COMC3D_COM_STATIC_TEXTURE_H__

#include "ComTextureBase.hpp"

#include <StaticTexture.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CStaticTexture object accessible from COM.
	\~french
	\brief		Cette classe définit un CStaticTexture accessible depuis COM.
	*/
	class ATL_NO_VTABLE CStaticTexture
		:	COM_ATL_OBJECT( StaticTexture )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CStaticTexture();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CStaticTexture();

		inline Castor3D::StaticTextureSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::StaticTextureSPtr state )
		{
			m_internal = state;
		}

		COM_PROPERTY( Dimension, eTEXTURE_DIMENSION, make_getter( m_internal.get(), &Castor3D::TextureBase::GetDimension ), make_putter( m_internal.get(), &Castor3D::TextureBase::SetDimension ) );
		COM_PROPERTY( Sampler, ISampler *, make_getter( m_internal.get(), &Castor3D::TextureBase::GetSampler ), make_putter( m_internal.get(), &Castor3D::TextureBase::SetSampler ) );
		COM_PROPERTY( MappingMode, eTEXTURE_MAP_MODE, make_getter( m_internal.get(), &Castor3D::TextureBase::GetMappingMode ), make_putter( m_internal.get(), &Castor3D::TextureBase::SetMappingMode ) );
		COM_PROPERTY( Buffer, IPixelBuffer *, make_getter( m_internal.get(), &Castor3D::TextureBase::GetBuffer ), make_putter( m_internal.get(), &Castor3D::TextureBase::SetImage ) );

		COM_PROPERTY_GET( Type, eTEXTURE_TYPE, make_getter( m_internal.get(), &Castor3D::TextureBase::GetType ) );

		STDMETHOD( Initialise )( /* [in] */ unsigned int index );
		STDMETHOD( Cleanup )();
		STDMETHOD( SetImage )( /* [in] */ IPixelBuffer * buffer );
		STDMETHOD( Set3DImage )( /* [in] */ ISize * size, /* [in] */ unsigned int depth, /* [in] */ IPixelBuffer * buffer );

	private:
		Castor3D::StaticTextureSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( StaticTexture ), CStaticTexture )

	template< typename Class >
	struct VariableGetter< Class, Castor3D::StaticTextureSPtr >
	{
		typedef Castor3D::StaticTextureSPtr( Class::*Function )()const;
		VariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IStaticTexture ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CStaticTexture::CreateInstance( value );

					if ( hr == S_OK )
					{
						static_cast< CStaticTexture * >( *value )->SetInternal( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IStaticTexture,					// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class >
	struct VariablePutter< Class, Castor3D::StaticTextureSPtr >
	{
		typedef void ( Class::*Function )( Castor3D::StaticTextureSPtr );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IStaticTexture * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( static_cast< CStaticTexture * >( value )->GetInternal() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IStaticTexture,					// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};
}

#endif

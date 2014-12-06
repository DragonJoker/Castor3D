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
#ifndef __COMC3D_COM_LIGHT_CATEGORY_H__
#define __COMC3D_COM_LIGHT_CATEGORY_H__

#include "ComColour.hpp"

#include <LightCategory.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CLightCategory object accessible from COM.
	\~french
	\brief		Cette classe définit un CLightCategory accessible depuis COM.
	*/
	class ATL_NO_VTABLE CLightCategory
		:	COM_ATL_OBJECT( LightCategory )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CLightCategory();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CLightCategory();

		inline Castor3D::LightCategorySPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::LightCategorySPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Ambient, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetAmbient ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetAmbient ) );
		COM_PROPERTY( Diffuse, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetDiffuse ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetDiffuse ) );
		COM_PROPERTY( Specular, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetSpecular ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetSpecular ) );

	private:
		Castor3D::LightCategorySPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LightCategory ), CLightCategory )

	template< typename Class >
	struct VariableGetter< Class, Castor3D::LightCategorySPtr >
	{
		typedef Castor3D::LightCategorySPtr( Class::*Function )()const;
		VariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( ILightCategory ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CLightCategory::CreateInstance( value );

					if ( hr == S_OK )
					{
						static_cast< CLightCategory * >( *value )->SetInternal( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_ILightCategory,					// This is the GUID of component throwing error
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
	struct VariablePutter< Class, Castor3D::LightCategorySPtr >
	{
		typedef void ( Class::*Function )( Castor3D::LightCategorySPtr );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( ILightCategory * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( static_cast< CLightCategory * >( value )->GetInternal() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_ILightCategory,					// This is the GUID of component throwing error
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

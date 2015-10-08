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
#ifndef __COMC3D_COM_COLOUR_H__
#define __COMC3D_COM_COLOUR_H__

#include "ComAtlObject.hpp"

#include <Colour.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CColour object accessible from COM.
	\~french
	\brief		Cette classe définit un CColour accessible depuis COM
	*/
	class ATL_NO_VTABLE CColour
		: COM_ATL_OBJECT( Colour )
		, public Castor::Colour
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CColour();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CColour();
		/**
		*\~english
		*\brief		Implicit conversion operator, to Castor::Point4f.
		*\~french
		*\brief		Opérateur de conversion implicite vers Castor::Point4f.
		*/
		inline operator Castor::Point4f()const
		{
			Castor::Point4f l_return;
			this->to_bgra( l_return );
			return l_return;
		}

		COM_PROPERTY( R, FLOAT, make_getter( this, &Castor::Colour::get, Castor::Colour::eCOMPONENT_RED ), make_putter( this, &Castor::Colour::get, Castor::Colour::eCOMPONENT_RED ) );
		COM_PROPERTY( G, FLOAT, make_getter( this, &Castor::Colour::get, Castor::Colour::eCOMPONENT_GREEN ), make_putter( this, &Castor::Colour::get, Castor::Colour::eCOMPONENT_GREEN ) );
		COM_PROPERTY( B, FLOAT, make_getter( this, &Castor::Colour::get, Castor::Colour::eCOMPONENT_BLUE ), make_putter( this, &Castor::Colour::get, Castor::Colour::eCOMPONENT_BLUE ) );
		COM_PROPERTY( A, FLOAT, make_getter( this, &Castor::Colour::get, Castor::Colour::eCOMPONENT_ALPHA ), make_putter( this, &Castor::Colour::get, Castor::Colour::eCOMPONENT_ALPHA ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Colour ), CColour );

	DECLARE_VARIABLE_REF_GETTER( Colour, Castor, Colour );
	DECLARE_VARIABLE_REF_PUTTER( Colour, Castor, Colour );

	template< typename Class >
	struct VariableRefGetter< Class, Castor::Point4f >
	{
		typedef Castor::Point4f Value;
		typedef Value const & ( Class::*Function )( )const;
		VariableRefGetter( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		HRESULT operator()( IColour ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CColour::CreateInstance( value );

					if ( hr == S_OK )
					{
						Castor::Colour * l_colour = static_cast< Castor::Colour * >( static_cast< CColour * >( *value ) );
						l_colour->from_bgra( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, IID_IColour, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class >
	struct VariablePutter< Class, Castor::Point4f const & >
	{
		typedef void ( Class::*Function )( Castor::Point4f const & );
		VariablePutter( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		HRESULT operator()( IColour * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( *static_cast< CColour * >( value ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, IID_IColour, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};
}

#endif

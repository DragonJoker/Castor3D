/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_COLOUR_H__
#define __COMC3D_COM_COLOUR_H__

#include "ComAtlObject.hpp"

#include <Graphics/Colour.hpp>

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
		, public castor::Colour
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CColour();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CColour();
		/**
		*\~english
		*\brief		Implicit conversion operator, to castor::Point4f.
		*\~french
		*\brief		Opérateur de conversion implicite vers castor::Point4f.
		*/
		inline operator castor::Point4f()const
		{
			return toBGRAFloat( *this );
		}

		COM_PROPERTY( R, FLOAT, make_getter( this, &castor::Colour::get, castor::Component::eRed ), make_putter( this, &castor::Colour::get, castor::Component::eRed ) );
		COM_PROPERTY( G, FLOAT, make_getter( this, &castor::Colour::get, castor::Component::eGreen ), make_putter( this, &castor::Colour::get, castor::Component::eGreen ) );
		COM_PROPERTY( B, FLOAT, make_getter( this, &castor::Colour::get, castor::Component::eBlue ), make_putter( this, &castor::Colour::get, castor::Component::eBlue ) );
		COM_PROPERTY( A, FLOAT, make_getter( this, &castor::Colour::get, castor::Component::eAlpha ), make_putter( this, &castor::Colour::get, castor::Component::eAlpha ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Colour ), CColour );

	DECLARE_VARIABLE_REF_GETTER( Colour, castor, Colour );
	DECLARE_VARIABLE_REF_PUTTER( Colour, castor, Colour );

	template< typename Class >
	struct VariableRefgetter< Class, castor::Point4f >
	{
		typedef castor::Point4f Value;
		typedef Value const & ( Class::*Function )( )const;
		VariableRefgetter( Class * instance, Function function )
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
						castor::Colour * l_colour = static_cast< castor::Colour * >( static_cast< CColour * >( *value ) );
						l_colour->fromBGRA( ( m_instance->*m_function )() );
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
	struct VariablePutter< Class, castor::Point4f const & >
	{
		typedef void ( Class::*Function )( castor::Point4f const & );
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

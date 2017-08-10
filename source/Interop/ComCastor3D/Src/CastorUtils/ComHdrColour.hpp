/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __COMC3D_COM_HDRCOLOUR_H__
#define __COMC3D_COM_HDRCOLOUR_H__

#include "ComAtlObject.hpp"

#include <Graphics/Colour.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a HdrColour object accessible from COM.
	\~french
	\brief		Cette classe définit un HdrColour accessible depuis COM
	*/
	class ATL_NO_VTABLE CHdrColour
		: COM_ATL_OBJECT( HdrColour )
		, public castor::HdrColour
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CHdrColour();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CHdrColour();
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

		COM_PROPERTY( R, FLOAT, make_getter( this, &castor::HdrColour::get, castor::Component::eRed ), make_putter( this, &castor::HdrColour::get, castor::Component::eRed ) );
		COM_PROPERTY( G, FLOAT, make_getter( this, &castor::HdrColour::get, castor::Component::eGreen ), make_putter( this, &castor::HdrColour::get, castor::Component::eGreen ) );
		COM_PROPERTY( B, FLOAT, make_getter( this, &castor::HdrColour::get, castor::Component::eBlue ), make_putter( this, &castor::HdrColour::get, castor::Component::eBlue ) );
		COM_PROPERTY( A, FLOAT, make_getter( this, &castor::HdrColour::get, castor::Component::eAlpha ), make_putter( this, &castor::HdrColour::get, castor::Component::eAlpha ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( HdrColour ), CHdrColour );

	DECLARE_VARIABLE_REF_GETTER( HdrColour, castor, HdrColour );
	DECLARE_VARIABLE_REF_PUTTER( HdrColour, castor, HdrColour );
/*
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
		HRESULT operator()( IHdrColour ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CHdrColour::CreateInstance( value );

					if ( hr == S_OK )
					{
						castor::Colour * l_colour = static_cast< castor::HdrColour * >( static_cast< CHdrColour * >( *value ) );
						l_colour->fromBGRA( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, IID_IHdrColour, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
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
		HRESULT operator()( IHdrColour * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( *static_cast< CHdrColour * >( value ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, IID_IHdrColour, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};
*/
}

#endif

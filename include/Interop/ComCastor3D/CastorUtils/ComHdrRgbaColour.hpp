/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_HDRRGBACOLOUR_H__
#define __COMC3D_COM_HDRRGBACOLOUR_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Colour.hpp>

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
	class ATL_NO_VTABLE CHdrRgbaColour
		: COM_ATL_OBJECT( HdrRgbaColour )
		, public castor::HdrRgbaColour
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CHdrRgbaColour();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CHdrRgbaColour();
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

		COM_PROPERTY( R, FLOAT, makeGetter( this, &castor::HdrRgbaColour::get, castor::RgbaComponent::eRed ), makePutter( this, &castor::HdrRgbaColour::get, castor::RgbaComponent::eRed ) );
		COM_PROPERTY( G, FLOAT, makeGetter( this, &castor::HdrRgbaColour::get, castor::RgbaComponent::eGreen ), makePutter( this, &castor::HdrRgbaColour::get, castor::RgbaComponent::eGreen ) );
		COM_PROPERTY( B, FLOAT, makeGetter( this, &castor::HdrRgbaColour::get, castor::RgbaComponent::eBlue ), makePutter( this, &castor::HdrRgbaColour::get, castor::RgbaComponent::eBlue ) );
		COM_PROPERTY( A, FLOAT, makeGetter( this, &castor::HdrRgbaColour::get, castor::RgbaComponent::eAlpha ), makePutter( this, &castor::HdrRgbaColour::get, castor::RgbaComponent::eAlpha ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( HdrRgbaColour ), CHdrRgbaColour );

	DECLARE_VARIABLE_REF_GETTER( HdrRgbaColour, castor, HdrRgbaColour );
	DECLARE_VARIABLE_REF_PUTTER( HdrRgbaColour, castor, HdrRgbaColour );
/*
	template< typename Class >
	struct VariableRefGetter< Class, castor::Point4f >
	{
		typedef castor::Point4f Value;
		typedef Value const & ( Class::*Function )( )const;
		VariableRefGetter( Class * instance, Function function )
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
				hr = CComError::dispatchError( E_FAIL, IID_IHdrColour, cuT( "nullptr instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, nullptr );
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
				hr = CComError::dispatchError( E_FAIL, IID_IHdrColour, cuT( "nullptr instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, nullptr );
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

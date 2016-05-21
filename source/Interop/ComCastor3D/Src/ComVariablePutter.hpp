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
#ifndef ___C3DCOM_COM_VARIABLE_PUTTER_H___
#define ___C3DCOM_COM_VARIABLE_PUTTER_H___

#include "ComParameterCast.hpp"

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>

namespace CastorCom
{
	template< typename Class, typename Value >
	struct VariablePutter
	{
		typedef void ( Class::*Function )( Value );
		VariablePutter( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( parameter_cast< Value >( value ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_Castor3D, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename _Class >
	VariablePutter< Class, Value >
	make_putter( _Class * instance, void ( Class::*function )( Value ) )
	{
		return VariablePutter< Class, Value >( ( Class * )instance, function );
	}

	template< typename Class, typename Value >
	struct VariablePutterEvt
	{
		typedef void ( Class::*Function )( Value );
		VariablePutterEvt( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					m_instance->GetEngine()->PostEvent( Castor3D::MakeFunctorEvent( Castor3D::eEVENT_TYPE_PRE_RENDER, [this, value]
					{
						( m_instance->*m_function )( parameter_cast< Value >( value ) );
					} ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_Castor3D, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename _Class >
	VariablePutterEvt< Class, Value >
	make_putter_evt( _Class * instance, void ( Class::*function )( Value ) )
	{
		return VariablePutterEvt< Class, Value >( ( Class * )instance, function );
	}

	template< typename Class, typename Value >
	struct VariableRetPutter
	{
		typedef Value & ( Class::*Function )();
		VariableRetPutter( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename Parameter >
		HRESULT operator()( Parameter value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				( m_instance->*m_function )() = parameter_cast< Value >( value );
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_Castor3D, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename _Class >
	VariableRetPutter< Class, Value >
	make_putter( _Class * instance, Value & ( Class::*function )() )
	{
		return VariableRetPutter< Class, Value >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct ParameteredVariablePutter
	{
		typedef Value & ( Class::*Function )( Index );
		ParameteredVariablePutter( Class * instance, Function function, Index index )
			: m_instance( instance )
			, m_function( function )
			, m_index( index )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				// No parameter_cast here, to be able to compile this with Value = Castor3D::ColourComponent and _Value = FLOAT
				( m_instance->*m_function )( m_index ) = value;
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_Castor3D, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
		Index m_index;
	};

	template< typename Class, typename Value, typename Index, typename _Class, typename _Index >
	ParameteredVariablePutter< Class, Value, Index >
	make_putter( _Class * instance, Value & ( Class::*function )( Index ), _Index index )
	{
		return ParameteredVariablePutter< Class, Value, Index >( ( Class * )instance, function, Index( index ) );
	}

	template< typename Class, typename Value, typename Index >
	struct ParameteredParVariablePutter
	{
		typedef void ( Class::*Function )( Index, Value );
		ParameteredParVariablePutter( Class * instance, Function function, Index index )
			: m_instance( instance )
			, m_function( function )
			, m_index( index )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				( m_instance->*m_function )( m_index, parameter_cast< Value >( value ) );
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_Castor3D, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
		Index m_index;
	};

	template< typename Class, typename Value, typename Index, typename _Class, typename _Index >
	ParameteredParVariablePutter< Class, Value, Index >
	make_putter( _Class * instance, void ( Class::*function )( Index, Value ), _Index index )
	{
		return ParameteredParVariablePutter< Class, Value, Index >( ( Class * )instance, function, Index( index ) );
	}

#define DECLARE_VARIABLE_REF_PUTTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariablePutter< Class, nmspc::type const & >\
	{\
		typedef void ( Class::*Function )( nmspc::type const & );\
		VariablePutter( Class * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		HRESULT operator()( I##ctype * value )\
		{\
			HRESULT hr = E_POINTER;\
			if ( m_instance )\
			{\
				if ( value )\
				{\
					( m_instance->*m_function )( *static_cast< C##ctype * >( value ) );\
					hr = S_OK;\
				}\
			}\
			else\
			{\
				hr = CComError::DispatchError( E_FAIL, IID_I##ctype, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );\
			}\
			return hr;\
		}\
	private:\
		Class * m_instance;\
		Function m_function;\
	}

#define DECLARE_VARIABLE_PTR_PUTTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariablePutter< Class, nmspc::type##SPtr >\
	{\
		typedef void ( Class::*Function )( nmspc::type##SPtr );\
		VariablePutter( Class * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		HRESULT operator()( I##ctype * value )\
		{\
			HRESULT hr = E_POINTER;\
			if ( m_instance )\
			{\
				if ( value )\
				{\
					( m_instance->*m_function )( static_cast< C##ctype * >( value )->GetInternal() );\
					hr = S_OK;\
				}\
			}\
			else\
			{\
				hr = CComError::DispatchError( E_FAIL, IID_I##ctype, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );\
			}\
			return hr;\
		}\
	private:\
		Class * m_instance;\
		Function m_function;\
	}
}

#endif

/* See LICENSE file in root folder */
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
				hr = CComError::DispatchError( E_FAIL, LIBID_castor3d, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
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
					m_instance->getEngine()->postEvent( castor3d::MakeFunctorEvent( castor3d::eEVENT_TYPE_PRE_RENDER, [this, value]
					{
						( m_instance->*m_function )( parameter_cast< Value >( value ) );
					} ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_castor3d, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
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
				hr = CComError::DispatchError( E_FAIL, LIBID_castor3d, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
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
				// No parameter_cast here, to be able to compile this with Value = castor3d::ColourComponent and _Value = FLOAT
				( m_instance->*m_function )( m_index ) = value;
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_castor3d, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
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
				hr = CComError::DispatchError( E_FAIL, LIBID_castor3d, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
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
					( m_instance->*m_function )( static_cast< C##ctype * >( value )->getInternal() );\
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

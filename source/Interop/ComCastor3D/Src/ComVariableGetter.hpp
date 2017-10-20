/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_GETTER_H___
#define ___C3DCOM_COM_VARIABLE_GETTER_H___

#include "ComParameterCast.hpp"

namespace CastorCom
{
	template< typename Class, typename Value >
	struct Variablegetter
	{
		typedef Value( Class::*Function )()const;
		Variablegetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					*value = parameter_cast< _Value >( ( m_instance->*m_function )() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 LIBID_castor3d,						// This is the GUID of PixelComponents throwing error
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

	template< typename Class, typename Value, typename _Class >
	Variablegetter< Class, Value >
	make_getter( _Class * instance, Value( Class::*function )()const )
	{
		return Variablegetter< Class, Value >( ( Class * )instance, function );
	}

	template< typename Value >
	struct Staticgetter
	{
		typedef std::function< Value() > Function;
		Staticgetter( Function function )
			: m_function( function )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( value )
			{
				*value = parameter_cast< _Value >( m_function() );
				hr = S_OK;
			}

			return hr;
		}

	private:
		Function m_function;
	};

	template< typename Value >
	Staticgetter< Value >
	make_static_getter( Value( *function )() )
	{
		return Staticgetter< Value >( function );
	}

	template< typename Class, typename Value >
	struct VariableRefgetter
	{
		typedef Value const & ( Class::*Function )()const;
		VariableRefgetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					*value = parameter_cast< _Value >( ( m_instance->*m_function )() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 LIBID_castor3d,						// This is the GUID of PixelComponents throwing error
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

	template< typename Class, typename Value, typename _Class >
	VariableRefgetter< Class, Value >
	make_getter( _Class * instance, Value const & ( Class::*function )()const )
	{
		return VariableRefgetter< Class, Value >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct ParameteredVariablegetter
	{
		typedef Value( Class::*Function )( Index )const;
		ParameteredVariablegetter( Class * instance, Function function, Index index )
			:	m_instance( instance )
			,	m_function( function )
			,	m_index( index )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					*value = parameter_cast< _Value >( ( m_instance->*m_function )( m_index ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 LIBID_castor3d,						// This is the GUID of PixelComponents throwing error
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
		Index m_index;
	};

	template< typename Class, typename Value, typename Index, typename _Class, typename _Index >
	ParameteredVariablegetter< Class, Value, Index >
	make_getter( _Class * instance, Value( Class::*function )( Index )const, _Index index )
	{
		return ParameteredVariablegetter< Class, Value, Index >( ( Class * )instance, function, Index( index ) );
	}

	template< typename Class, typename Value, typename Index >
	struct ParameteredRefVariablegetter
	{
		typedef Value const & ( Class::*Function )( Index )const;
		ParameteredRefVariablegetter( Class * instance, Function function, Index index )
			:	m_instance( instance )
			,	m_function( function )
			,	m_index( index )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					*value = parameter_cast< _Value >( ( m_instance->*m_function )( m_index ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 LIBID_castor3d,						// This is the GUID of PixelComponents throwing error
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
		Index m_index;
	};

	template< typename Class, typename Value, typename Index, typename _Class, typename _Index >
	ParameteredRefVariablegetter< Class, Value, Index >
	make_getter( _Class * instance, Value const & ( Class::*function )( Index )const, _Index index )
	{
		return ParameteredRefVariablegetter< Class, Value, Index >( ( Class * )instance, function, Index( index ) );
	}

#define DECLARE_VARIABLE_VAL_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct Variablegetter< Class, nmspc::type >\
	{\
		typedef nmspc::type( Class::*Function )()const;\
		Variablegetter( Class * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		HRESULT operator()( I##ctype ** value )\
		{\
			HRESULT hr = E_POINTER;\
			if ( m_instance )\
			{\
				if ( value )\
				{\
					hr = C##ctype::CreateInstance( value );\
					if ( hr == S_OK )\
					{\
						*static_cast< nmspc::type * >( static_cast< C##ctype * >( *value ) ) = ( m_instance->*m_function )();\
					}\
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

#define DECLARE_VARIABLE_REF_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariableRefgetter< Class, castor::type >\
	{\
		typedef nmspc::type const & ( Class::*Function )( )const;\
		VariableRefgetter( Class * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		HRESULT operator()( I##ctype ** value )\
		{\
			HRESULT hr = E_POINTER;\
			if ( m_instance )\
			{\
				if ( value )\
				{\
					hr = C##ctype::CreateInstance( value );\
					if ( hr == S_OK )\
					{\
						*static_cast< nmspc::type * >( static_cast< C##ctype * >( *value ) ) = ( m_instance->*m_function )( );\
					}\
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

#define DECLARE_VARIABLE_PTR_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct Variablegetter< Class, nmspc::type##SPtr >\
	{\
		typedef nmspc::type##SPtr( Class::*Function )( )const;\
		Variablegetter( Class * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		HRESULT operator()( I##ctype ** value )\
		{\
			HRESULT hr = E_POINTER;\
			if ( m_instance )\
			{\
				if ( value )\
				{\
					hr = C##ctype::CreateInstance( value );\
					if ( hr == S_OK )\
					{\
						static_cast< C##ctype * >( *value )->setInternal( ( m_instance->*m_function )( ) );\
					}\
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

#define DECLARE_VARIABLE_REF_PARAM_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariableParamgetter\
	{\
		typedef void ( Class::*Function )( nmspc::type & )const;\
		VariableParamgetter( Class * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		HRESULT operator()( I##ctype ** value )\
		{\
			HRESULT hr = E_POINTER;\
			if ( m_instance )\
			{\
				if ( value )\
				{\
					hr = C##ctype::CreateInstance( value );\
					if ( hr == S_OK )\
					{\
						( m_instance->*m_function )( *static_cast< C##ctype * >( *value ) );\
					}\
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
	};\
	template< typename Class, typename _Class >\
	VariableParamgetter< Class >\
	make_getter( _Class * instance, void ( Class::*function )( nmspc::type & )const )\
	{\
		return VariableParamgetter< Class >( ( Class * )instance, function );\
	}
}

#endif

/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_GETTER_H___
#define ___C3DCOM_COM_VARIABLE_GETTER_H___

#include "ComCastor3D/ComParameterCast.hpp"

namespace CastorCom
{
	template< typename Class, typename Value >
	struct VariableGetter
	{
		using Function = Value( Class::* )()const;
		VariableGetter( Class * instance, Function function )
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
					*value = parameterCast< _Value >( ( m_instance->*m_function )() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, _T( "Null instance" )
					, ERROR_UNINITIALISED_INSTANCE.c_str()
					, 0
					, nullptr );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename _Class >
	VariableGetter< Class, Value >
	makeGetter( _Class * instance, Value( Class::*function )()const )
	{
		return VariableGetter< Class, Value >( ( Class * )instance, function );
	}

	template< typename Value >
	struct Staticgetter
	{
		typedef std::function< Value() > Function;
		explicit Staticgetter( Function function )
			: m_function( function )
		{
		}
		template< typename _Value >
		HRESULT operator()( _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( value )
			{
				*value = parameterCast< _Value >( m_function() );
				hr = S_OK;
			}

			return hr;
		}

	private:
		Function m_function;
	};

	template< typename Value >
	Staticgetter< Value >
	makeStaticGetter( Value( *function )() )
	{
		return Staticgetter< Value >( function );
	}

	template< typename Class, typename Value >
	struct VariableRefGetter
	{
		using Function = Value const & ( Class::* )()const;
		VariableRefGetter( Class * instance, Function function )
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
					*value = parameterCast< _Value >( ( m_instance->*m_function )() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, _T( "Null instance" )
					, ERROR_UNINITIALISED_INSTANCE.c_str()
					, 0
					, nullptr );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename _Class >
	VariableRefGetter< Class, Value >
	makeGetter( _Class * instance, Value const & ( Class::*function )()const )
	{
		return VariableRefGetter< Class, Value >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct ParameteredVariablegetter
	{
		using Function = Value( Class::* )( Index )const;
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
					*value = parameterCast< _Value >( ( m_instance->*m_function )( m_index ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, _T( "Null instance" )
					, ERROR_UNINITIALISED_INSTANCE.c_str()
					, 0
					, nullptr );
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
	makeGetter( _Class * instance, Value( Class::*function )( Index )const, _Index index )
	{
		return ParameteredVariablegetter< Class, Value, Index >( ( Class * )instance, function, Index( index ) );
	}

	template< typename Class, typename Value, typename Index >
	struct ParameteredRefVariablegetter
	{
		using Function = Value const & ( Class::* )( Index )const;
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
					*value = parameterCast< _Value >( ( m_instance->*m_function )( m_index ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, _T( "Null instance" )
					, ERROR_UNINITIALISED_INSTANCE.c_str()
					, 0
					, nullptr );
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
	makeGetter( _Class * instance, Value const & ( Class::*function )( Index )const, _Index index )
	{
		return ParameteredRefVariablegetter< Class, Value, Index >( ( Class * )instance, function, Index( index ) );
	}

#define DECLARE_VARIABLE_VAL_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariableGetter< Class, nmspc::type >\
	{\
		using Function = nmspc::type( Class::* )()const;\
		VariableGetter( Class * instance, Function function )\
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
				hr = CComError::dispatchError( E_FAIL\
					, LIBID_Castor3D\
					, _T( "Null instance" )\
					, ERROR_UNINITIALISED_INSTANCE.c_str()\
					, 0\
					, nullptr );\
			}\
			return hr;\
		}\
	private:\
		Class * m_instance;\
		Function m_function;\
	}

#define DECLARE_VARIABLE_REF_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariableRefGetter< Class, nmspc::type >\
	{\
		using Function = nmspc::type const &( Class::* )()const;\
		VariableRefGetter( Class * instance, Function function )\
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
				hr = CComError::dispatchError( E_FAIL\
					, LIBID_Castor3D\
					, _T( "Null instance" )\
					, ERROR_UNINITIALISED_INSTANCE.c_str()\
					, 0\
					, nullptr );\
			}\
			return hr;\
		}\
	private:\
		Class * m_instance;\
		Function m_function;\
	}

#define DECLARE_VARIABLE_PTR_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariableGetter< Class, std::shared_ptr< nmspc::type > >\
	{\
		using Function = std::shared_ptr< nmspc::type >( Class::* )()const;\
		VariableGetter( Class * instance, Function function )\
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
				hr = CComError::dispatchError( E_FAIL\
					, LIBID_Castor3D\
					, _T( "Null instance" )\
					, ERROR_UNINITIALISED_INSTANCE.c_str()\
					, 0\
					, nullptr );\
			}\
			return hr;\
		}\
	private:\
		Class * m_instance;\
		Function m_function;\
	}

#define DECLARE_VARIABLE_REF_PARAM_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariableParamGetter\
	{\
		using Function = void( Class::* )( nmspc::type & )const;\
		VariableParamGetter( Class * instance, Function function )\
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
				hr = CComError::dispatchError( E_FAIL\
					, LIBID_Castor3D\
					, _T( "Null instance" )\
					, ERROR_UNINITIALISED_INSTANCE.c_str()\
					, 0\
					, nullptr );\
			}\
			return hr;\
		}\
	private:\
		Class * m_instance;\
		Function m_function;\
	};\
	template< typename Class, typename _Class >\
	VariableParamGetter< Class >\
	makeGetter( _Class * instance, void ( Class::*function )( nmspc::type & )const )\
	{\
		return VariableParamGetter< Class >( ( Class * )instance, function );\
	}
}

#endif

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
#ifndef ___C3DCOM_COM_VARIABLE_GETTER_H___
#define ___C3DCOM_COM_VARIABLE_GETTER_H___

#include "ComParameterCast.hpp"

namespace CastorCom
{
	template< typename Class, typename Value >
	struct VariableGetter
	{
		typedef Value( Class::*Function )()const;
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
					*value = parameter_cast< _Value >( ( m_instance->*m_function )() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 LIBID_Castor3D,						// This is the GUID of component throwing error
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
	VariableGetter< Class, Value >
	make_getter( _Class * instance, Value( Class::*function )()const )
	{
		return VariableGetter< Class, Value >( ( Class * )instance, function );
	}

	template< typename Value >
	struct StaticGetter
	{
		typedef std::function< Value() > Function;
		StaticGetter( Function function )
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
	StaticGetter< Value >
	make_static_getter( Value( *function )() )
	{
		return StaticGetter< Value >( function );
	}

	template< typename Class, typename Value >
	struct VariableRefGetter
	{
		typedef Value const & ( Class::*Function )()const;
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
					*value = parameter_cast< _Value >( ( m_instance->*m_function )() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 LIBID_Castor3D,						// This is the GUID of component throwing error
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
	VariableRefGetter< Class, Value >
	make_getter( _Class * instance, Value const & ( Class::*function )()const )
	{
		return VariableRefGetter< Class, Value >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct ParameteredVariableGetter
	{
		typedef Value( Class::*Function )( Index )const;
		ParameteredVariableGetter( Class * instance, Function function, Index index )
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
						 LIBID_Castor3D,						// This is the GUID of component throwing error
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
	ParameteredVariableGetter< Class, Value, Index >
	make_getter( _Class * instance, Value( Class::*function )( Index )const, _Index index )
	{
		return ParameteredVariableGetter< Class, Value, Index >( ( Class * )instance, function, Index( index ) );
	}

	template< typename Class, typename Value, typename Index >
	struct ParameteredRefVariableGetter
	{
		typedef Value const & ( Class::*Function )( Index )const;
		ParameteredRefVariableGetter( Class * instance, Function function, Index index )
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
						 LIBID_Castor3D,						// This is the GUID of component throwing error
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
	ParameteredRefVariableGetter< Class, Value, Index >
	make_getter( _Class * instance, Value const & ( Class::*function )( Index )const, _Index index )
	{
		return ParameteredRefVariableGetter< Class, Value, Index >( ( Class * )instance, function, Index( index ) );
	}

#define DECLARE_VARIABLE_VAL_GETTER( ctype, nmspc, type )\
	template< typename Class >\
	struct VariableGetter< Class, nmspc::type >\
	{\
		typedef nmspc::type( Class::*Function )()const;\
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
	struct VariableRefGetter< Class, Castor::type >\
	{\
		typedef nmspc::type const & ( Class::*Function )( )const;\
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
	struct VariableGetter< Class, nmspc::type##SPtr >\
	{\
		typedef nmspc::type##SPtr( Class::*Function )( )const;\
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
						static_cast< C##ctype * >( *value )->SetInternal( ( m_instance->*m_function )( ) );\
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
	struct VariableParamGetter\
	{\
		typedef void ( Class::*Function )( nmspc::type & )const;\
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
				hr = CComError::DispatchError( E_FAIL, IID_I##ctype, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );\
			}\
			return hr;\
		}\
	private:\
		Class * m_instance;\
		Function m_function;\
	};\
	template< typename Class, typename _Class >\
	VariableParamGetter< Class >\
	make_getter( _Class * instance, void ( Class::*function )( nmspc::type & )const )\
	{\
		return VariableParamGetter< Class >( ( Class * )instance, function );\
	}
}

#endif

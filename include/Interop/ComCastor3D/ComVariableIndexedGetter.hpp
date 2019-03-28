/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_INDEXED_GETTER_H___
#define ___C3DCOM_COM_VARIABLE_INDEXED_GETTER_H___

#include "ComCastor3D/ComParameterCast.hpp"

namespace CastorCom
{
	template< typename Class, typename Value, typename Index >
	struct IndexedVariableGetter
	{
		using Function = Value( Class::* )( Index )const;
		IndexedVariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					*value = parameterCast< _Value >( ( m_instance->*m_function )( parameterCast< Index >( index ) ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, cuT( "Null instance" )
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

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariableGetter< Class, Value, Index >
	makeIndexedGetter( _Class * instance, Value( Class::*function )( Index )const )
	{
		return IndexedVariableGetter< Class, Value, Index >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct IndexedRefVariableGetter
	{
		using Function = Value const & ( Class::* )( Index )const;
		IndexedRefVariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					*value = parameterCast< _Value & >( ( m_instance->*m_function )( parameterCast< Index >( index ) ) );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, cuT( "Null instance" )
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

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedRefVariableGetter< Class, Value, Index >
	makeIndexedGetter( _Class * instance, Value const & ( Class::*function )( Index )const )
	{
		return IndexedRefVariableGetter< Class, Value, Index >( ( Class * )instance, function );
	}
}

#endif

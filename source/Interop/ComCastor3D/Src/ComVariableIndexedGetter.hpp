/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_INDEXED_GETTER_H___
#define ___C3DCOM_COM_VARIABLE_INDEXED_GETTER_H___

#include "ComParameterCast.hpp"

namespace CastorCom
{
	template< typename Class, typename Value, typename Index >
	struct IndexedVariablegetter
	{
		typedef Value( Class::*Function )( Index )const;
		IndexedVariablegetter( Class * instance, Function function )
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
					*value = parameter_cast< _Value >( ( m_instance->*m_function )( parameter_cast< Index >( index ) ) );
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

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablegetter< Class, Value, Index >
	make_indexed_getter( _Class * instance, Value( Class::*function )( Index )const )
	{
		return IndexedVariablegetter< Class, Value, Index >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct IndexedRefVariablegetter
	{
		typedef Value const & ( Class::*Function )( Index )const;
		IndexedRefVariablegetter( Class * instance, Function function )
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
					*value = parameter_cast< _Value & >( ( m_instance->*m_function )( parameter_cast< Index >( index ) ) );
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

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedRefVariablegetter< Class, Value, Index >
	make_indexed_getter( _Class * instance, Value const & ( Class::*function )( Index )const )
	{
		return IndexedRefVariablegetter< Class, Value, Index >( ( Class * )instance, function );
	}
}

#endif

/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_INDEXED_PUTTER_H___
#define ___C3DCOM_COM_VARIABLE_INDEXED_PUTTER_H___

#include "ComParameterCast.hpp"

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>

namespace CastorCom
{
	template< typename Class, typename Value, typename Index >
	struct IndexedVariablePutter
	{
		typedef void ( Class::*Function )( Index, Value );
		IndexedVariablePutter( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				( m_instance->*m_function )( parameter_cast< Index >( index ), parameter_cast< Value >( value ) );
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_castor3d, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return S_OK;
		}

	private:
		Class * m_instance;
		Function m_function;
	};
	
	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablePutter< Class, Value, Index >
	make_indexed_putter( _Class * instance, void ( Class::*function )( Index, Value ) )
	{
		return IndexedVariablePutter< Class, Value, Index >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct IndexedVariablePutterRev
	{
		typedef void ( Class::*Function )( Value, Index );
		IndexedVariablePutterRev( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				( m_instance->*m_function )( parameter_cast< Value >( value ), parameter_cast< Index >( index ) );
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_castor3d, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return S_OK;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablePutterRev< Class, Value, Index >
	make_indexed_putter_rev( _Class * instance, void ( Class::*function )( Value, Index ) )
	{
		return IndexedVariablePutterRev< Class, Value, Index >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct IndexedVariablePutterEvt
	{
		typedef void ( Class::*Function )( Index, Value );
		IndexedVariablePutterEvt( Class * instance, Function function )
			: m_instance( instance )
			, m_function( function )
		{
		}
		template< typename _Index, typename _Value >
		HRESULT operator()( _Index index, _Value value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				m_instance->getEngine()->postEvent( castor3d::MakeFunctorEvent( castor3d::eEVENT_TYPE_PRE_RENDER, [this, index, value]()
				{
					( m_instance->*m_function )( parameter_cast< Index >( index ), parameter_cast< Value >( value ) );
				} ) );
				hr = S_OK;
			}
			else
			{
				hr = CComError::DispatchError( E_FAIL, LIBID_castor3d, cuT( "NULL instance" ), ERROR_UNINITIALISED_INSTANCE.c_str(), 0, NULL );
			}

			return S_OK;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablePutterEvt< Class, Value, Index >
	make_indexed_putter_evt( _Class * instance, void ( Class::*function )( Index, Value ) )
	{
		return IndexedVariablePutterEvt< Class, Value, Index >( ( Class * )instance, function );
	}

#define DECLARE_INDEXED_VAL_PUTTER( nmspc, type )\
	template< typename Value, typename Index >\
	struct IndexedVariablePutter< nmspc::type, Value, Index >\
	{\
		typedef void ( nmspc::type::*Function )( Value, Index );\
		IndexedVariablePutter( nmspc::type * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		template< typename _Index, typename _Value >\
		HRESULT operator()( _Index index, _Value value )\
		{\
			( m_instance->*m_function )( parameter_cast< Value >( value ), index );\
			return S_OK;\
		}\
	private:\
		nmspc::type * m_instance;\
		Function m_function;\
	};\
	template< typename Value, typename Index, typename _Class >\
	IndexedVariablePutter< nmspc::type, Value, Index >\
	make_indexed_putter( _Class * instance, void ( nmspc::type::*function )( Value, Index ) )\
	{\
		return IndexedVariablePutter< nmspc::type, Value, Index >( ( nmspc::type * )instance, function );\
	}

#define DECLARE_INDEXED_VAL_PUTTER_EVT( nmspc, type )\
	template< typename Value, typename Index >\
	struct IndexedVariablePutterEvt< nmspc::type, Value, Index >\
	{\
		typedef void ( nmspc::type::*Function )( Value, Index );\
		IndexedVariablePutterEvt( nmspc::type * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		template< typename _Index, typename _Value >\
		HRESULT operator()( _Index index, _Value value )\
		{\
			m_instance->getEngine()->postEvent( castor3d::MakeFunctorEvent( castor3d::eEVENT_TYPE_PRE_RENDER, [this, value, index]()\
			{\
				( m_instance->*m_function )( parameter_cast< Value >( value ), index );\
			} ) );\
			return S_OK;\
		}\
	private:\
		nmspc::type * m_instance;\
		Function m_function;\
	};\
	template< typename Value, typename Index, typename _Class >\
	IndexedVariablePutterEvt< nmspc::type, Value, Index >\
	make_indexed_putter_evt( _Class * instance, void ( nmspc::type::*function )( Value, Index ) )\
	{\
		return IndexedVariablePutterEvt< nmspc::type, Value, Index >( ( nmspc::type * )instance, function );\
	}
}

#endif

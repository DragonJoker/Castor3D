/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_INDEXED_PUTTER_H___
#define ___C3DCOM_COM_VARIABLE_INDEXED_PUTTER_H___

#include "ComCastor3D/ComParameterCast.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>

namespace CastorCom
{
	template< typename Class, typename Value, typename Index >
	struct IndexedVariablePutter
	{
		using Function = void ( Class::* )( Index, Value );
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
				( m_instance->*m_function )( parameterCast< Index >( index ), parameterCast< Value >( value ) );
				hr = S_OK;
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

			return S_OK;
		}

	private:
		Class * m_instance;
		Function m_function;
	};
	
	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablePutter< Class, Value, Index >
	makeIndexedPutter( _Class * instance, void ( Class::*function )( Index, Value ) )
	{
		return IndexedVariablePutter< Class, Value, Index >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct IndexedVariablePutterRev
	{
		using Function = void ( Class::* )( Value, Index );
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
				( m_instance->*m_function )( parameterCast< Value >( value ), parameterCast< Index >( index ) );
				hr = S_OK;
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

			return S_OK;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablePutterRev< Class, Value, Index >
	makeIndexedPutterRev( _Class * instance, void ( Class::*function )( Value, Index ) )
	{
		return IndexedVariablePutterRev< Class, Value, Index >( ( Class * )instance, function );
	}

	template< typename Class, typename Value, typename Index >
	struct IndexedVariablePutterEvt
	{
		using Function = void ( Class::* )( Index, Value );
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
				m_instance->getEngine()->postEvent( castor3d::makeFunctorEvent( castor3d::EventType::ePreRender, [this, index, value]()
				{
					( m_instance->*m_function )( parameterCast< Index >( index ), parameterCast< Value >( value ) );
				} ) );
				hr = S_OK;
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

			return S_OK;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class, typename Value, typename Index, typename _Class >
	IndexedVariablePutterEvt< Class, Value, Index >
	makeIndexedPutterEvt( _Class * instance, void ( Class::*function )( Index, Value ) )
	{
		return IndexedVariablePutterEvt< Class, Value, Index >( ( Class * )instance, function );
	}

#define DECLARE_INDEXED_VAL_PUTTER( nmspc, type )\
	template< typename Value, typename Index >\
	struct IndexedVariablePutter< nmspc::type, Value, Index >\
	{\
		using Function = void( nmspc::type::* )( Value, Index );\
		IndexedVariablePutter( nmspc::type * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		template< typename _Index, typename _Value >\
		HRESULT operator()( _Index index, _Value value )\
		{\
			( m_instance->*m_function )( parameterCast< Value >( value ), index );\
			return S_OK;\
		}\
	private:\
		nmspc::type * m_instance;\
		Function m_function;\
	};\
	template< typename Value, typename Index, typename _Class >\
	IndexedVariablePutter< nmspc::type, Value, Index >\
	makeIndexedPutter( _Class * instance, void ( nmspc::type::*function )( Value, Index ) )\
	{\
		return IndexedVariablePutter< nmspc::type, Value, Index >( ( nmspc::type * )instance, function );\
	}

#define DECLARE_INDEXED_VAL_PUTTER_EVT( nmspc, type )\
	template< typename Value, typename Index >\
	struct IndexedVariablePutterEvt< nmspc::type, Value, Index >\
	{\
		using Function = void( nmspc::type::* )( Value, Index );\
		IndexedVariablePutterEvt( nmspc::type * instance, Function function )\
			: m_instance( instance )\
			, m_function( function )\
		{\
		}\
		template< typename _Index, typename _Value >\
		HRESULT operator()( _Index index, _Value value )\
		{\
			m_instance->getEngine()->postEvent( castor3d::makeFunctorEvent( castor3d::EventType::ePreRender, [this, value, index]()\
			{\
				( m_instance->*m_function )( parameterCast< Value >( value ), index );\
			} ) );\
			return S_OK;\
		}\
	private:\
		nmspc::type * m_instance;\
		Function m_function;\
	};\
	template< typename Value, typename Index, typename _Class >\
	IndexedVariablePutterEvt< nmspc::type, Value, Index >\
	makeIndexedPutterEvt( _Class * instance, void ( nmspc::type::*function )( Value, Index ) )\
	{\
		return IndexedVariablePutterEvt< nmspc::type, Value, Index >( ( nmspc::type * )instance, function );\
	}
}

#endif

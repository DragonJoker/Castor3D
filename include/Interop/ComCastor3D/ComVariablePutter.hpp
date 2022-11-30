/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_PUTTER_H___
#define ___C3DCOM_COM_VARIABLE_PUTTER_H___

#include "ComCastor3D/ComParameterCast.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>

namespace CastorCom
{
	template< typename ValueT >
	struct VariablePutterT
	{
		using Function = std::function< void( ValueT ) >;

		VariablePutterT( Function function )
			: m_function( function )
		{
		}

		template< typename ITypePtrT >
		HRESULT operator()( ITypePtrT value )
		{
			if ( !value )
			{
				return CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, _T( "Null value" )
					, ERROR_UNINITIALISED_INSTANCE.c_str()
					, 0
					, nullptr );
			}

			if constexpr ( isComITypeV< ITypePtrT > )
			{
				m_function( static_cast< ComITypeCTypeT< ITypePtrT > * >( value )->getInternal() );
			}
			else
			{
				m_function( details::parameterCast< ValueT >( value ) );
			}

			return S_OK;
		}

	private:
		Function m_function;
	};

	template< typename ClassT, typename ValueT, typename InstanceT >
	auto makePutter( InstanceT * instance
		, void ( ClassT::* function )( ValueT ) )
	{
		return VariablePutterT< ValueT >( [function, instance]( ValueT value )
			{
				( static_cast< ClassT * >( instance )->*function )( value );
			} );
	}

	template< typename ClassT, typename ValueT, typename InstanceT >
	auto makePutter( InstanceT * instance
		, void ( *function )( ClassT *, ValueT ) )
	{
		return VariablePutterT< ValueT >( [function, instance]( ValueT value )
			{
				( *function )( static_cast< ClassT * >( instance ), value );
			} );
	}

	template< typename ClassT, typename ValueT, typename InstanceT >
	auto makePutter( InstanceT * instance
		, ValueT & ( ClassT::* function )() )
	{
		return VariablePutterT< ValueT >( [function, instance]( ValueT value )
			{
				( static_cast< ClassT * >( instance )->*function )() = value;
			} );
	}

	template< typename ClassT, typename ValueT, typename IndexT, typename InstanceT, typename IndexU >
	auto makePutter( InstanceT * instance
		, ValueT & ( ClassT:: * function )( IndexT )
		, IndexU index )
	{
		return VariablePutterT< ValueT >( [function, instance, index]( ValueT value )
			{
				( static_cast< ClassT * >( instance )->*function )( IndexT( index ) ) = value;
			} );
	}

	template< typename ClassT, typename ValueT, typename IndexT, typename InstanceT, typename IndexU >
	auto makePutter( InstanceT * instance
		, void( ClassT:: * function )( IndexT, ValueT )
		, IndexU index )
	{
		return VariablePutterT< ValueT >( [function, instance, index]( ValueT value )
			{
				( static_cast< ClassT * >( instance )->*function )( IndexT( index ), value );
			} );
	}

	template< typename ValueT >
	struct VariablePutterEvtT
	{
		using Function = std::function< void( ValueT ) >;

		VariablePutterEvtT( castor3d::Engine & engine
			, Function function )
			: m_engine( engine )
			, m_function( function )
		{
		}

		template< typename ITypePtrT >
		HRESULT operator()( ITypePtrT value )
		{
			if ( !value )
			{
				return CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, _T( "Null value" )
					, ERROR_UNINITIALISED_INSTANCE.c_str()
					, 0
					, nullptr );
			}

			m_engine.postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
				, [value, this]()
				{
					if constexpr ( isComITypeV< ITypePtrT > )
					{
						m_function( static_cast< ComITypeCTypeT< ITypePtrT > * >( value )->getInternal() );
					}
					else
					{
						m_function( details::parameterCast< ValueT >( value ) );
					}
				} ) );

			return S_OK;
		}

	private:
		castor3d::Engine & m_engine;
		Function m_function;
	};

	template< typename ClassT, typename ValueT, typename InstanceT >
	auto makePutterEvt( InstanceT * instance
		, void ( ClassT::* function )( ValueT ) )
	{
		return VariablePutterEvtT< ValueT >( *instance->getEngine()
			, [function, instance]( ValueT value )
			{
				( static_cast< ClassT * >( instance )->*function )( value );
			} );
	}
}

#endif

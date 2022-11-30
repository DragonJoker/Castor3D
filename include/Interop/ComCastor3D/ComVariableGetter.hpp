/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_VARIABLE_GETTER_H___
#define ___C3DCOM_COM_VARIABLE_GETTER_H___

#include "ComCastor3D/ComParameterCast.hpp"

namespace CastorCom
{
	template< typename ValueT >
	struct VariableGetterT
	{
		using Function = std::function< ValueT() >;

		VariableGetterT( Function function )
			: m_function( function )
		{
		}

		template< typename ITypePtrT >
		HRESULT operator()( ITypePtrT * value )
		{
			if ( !value )
			{
				return CComError::dispatchError( E_FAIL
					, LIBID_Castor3D
					, _T( "Null instance" )
					, ERROR_UNINITIALISED_INSTANCE.c_str()
					, 0
					, nullptr );
			}

			HRESULT hr = S_OK;

			if constexpr ( isComITypeV< ITypePtrT > )
			{
				hr = ComITypeCTypeT< ITypePtrT >::CreateInstance( value );

				if ( hr == S_OK )
				{
					static_cast< ComITypeCTypeT< ITypePtrT > * >( *value )->setInternal( m_function() );
				}
			}
			else
			{
				*value = details::parameterCast< ITypePtrT >( m_function() );
			}

			return S_OK;
		}

	private:
		Function m_function;
	};

	template< typename ClassT, typename ValueT, typename InstanceT >
	auto makeGetter( InstanceT const * instance
		, ValueT( ClassT::* function )()const )
	{
		return VariableGetterT< ComTypeGetInternalT< ValueT > >( [function, instance]()
			{
				return ( static_cast< ClassT const * >( instance )->*function )();
			} );
	}

	template< typename ClassT, typename ValueT, typename InstanceT >
	auto makeGetter( InstanceT const * instance
		, ValueT const &( ClassT::* function )()const )
	{
		return VariableGetterT< ComTypeGetInternalT< ValueT > >( [function, instance]()
			{
				return ( static_cast< ClassT const * >( instance )->*function )();
			} );
	}

	template< typename ClassT, typename ValueT, typename IndexT, typename InstanceT, typename IndexU >
	auto makeGetter( InstanceT const * instance
		, ValueT( ClassT::* function )( IndexT )const
		, IndexU index )
	{
		return VariableGetterT< ComTypeGetInternalT< ValueT > >( [function, instance, index]()
			{
				return ( static_cast< ClassT const * >( instance )->*function )( IndexT( index ) );
			} );
	}

	template< typename ClassT, typename ValueT, typename InstanceT >
	auto makeGetter( InstanceT const * instance
		, ValueT( *function )( ClassT const * ) )
	{
		return VariableGetterT< ComTypeGetInternalT< ValueT > >( [function, instance]()
			{
				return ( *function )( static_cast< ClassT const * >( instance ) );
			} );
	}

	template< typename ValueT >
	auto makeGetter( ValueT( *function )() )
	{
		return VariableGetterT< ComTypeGetInternalT< ValueT > >( [function]()
			{
				return ( *function )();
			} );
	}
}

#endif

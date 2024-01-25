/*
See LICENSE file in root folder
*/
#include "CastorUtils/Design/Resource.hpp"

namespace castor
{
	//*********************************************************************************************

	template< typename ResT, typename KeyT >
	inline void Deleter< ResourceT< ResT, KeyT > >::operator()( ResourceT< ResT, KeyT > * pointer )noexcept
	{
		delete pointer;
	}

	//*********************************************************************************************

	template< typename ResT, typename KeyT >
	template< typename ... ParametersT >
	ResourceT< ResT, KeyT >::ResourceT( ParametersT && ... params )
		: ResT{ castor::forward< ParametersT >( params )... }
	{
	}

	template< typename ResT, typename KeyT >
	ResourceT< ResT, KeyT >::ResourceT( ElementT && rhs )
		: ResT{ castor::move( rhs ) }
	{
	}

	template< typename ResT, typename KeyT >
	ResourceT< ResT, KeyT > & ResourceT< ResT, KeyT >::operator=( ElementT && rhs )
	{
		ResT::operator=( castor::move( rhs ) );
		return *this;
	}

	template< typename ResT, typename KeyT >
	template< typename ... ParametersT >
	void ResourceT< ResT, KeyT >::initialise( ParametersT && ... params )
	{
		if ( !m_initialised.exchange( true ) )
		{
			onInitialising( *this );
			ResT::initialise( castor::forward< ParametersT >( params )... );
			onInitialised( *this );
		}
	}

	template< typename ResT, typename KeyT >
	template< typename ... ParametersT >
	void ResourceT< ResT, KeyT >::cleanup( ParametersT && ... params )
	{
		if ( m_initialised.exchange( false ) )
		{
			onCleaning( *this );
			ResT::cleanup( castor::forward< ParametersT >( params )... );
			onCleaned( *this );
		}
	}

	template< typename ResT, typename KeyT >
	template< typename ... ParametersT, typename ... ParametersU >
	void ResourceT< ResT, KeyT >::reinitialise( ParametersT && ... paramsT
		, ParametersU && ... paramsU )
	{
		if ( m_initialised )
		{
			cleanup( castor::forward< ParametersT >( paramsT )... );
			initialise( castor::forward< ParametersU >( paramsU )... );
		}
	}

	//*********************************************************************************************
}

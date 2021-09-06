/*
See LICENSE file in root folder
*/
#include "CastorUtils/Design/Resource.hpp"

namespace castor
{
	//*********************************************************************************************

	template< typename ResT, typename KeyT >
	template< typename ... ParametersT >
	ResourceT< ResT, KeyT >::ResourceT( ParametersT && ... params )
		: ResT{ std::forward< ParametersT >( params )... }
	{
	}

	template< typename ResT, typename KeyT >
	ResourceT< ResT, KeyT >::ResourceT( ElementT && rhs )
		: ResT{ std::move( rhs ) }
	{
	}

	template< typename ResT, typename KeyT >
	ResourceT< ResT, KeyT > & ResourceT< ResT, KeyT >::operator=( ElementT && rhs )
	{
		ResT::operator=( std::move( rhs ) );
		return *this;
	}

	template< typename ResT, typename KeyT >
	template< typename ... ParametersT >
	void ResourceT< ResT, KeyT >::initialise( ParametersT && ... params )
	{
		if ( !m_initialised.exchange( true ) )
		{
			onInitialising( *this );
			ResT::initialise( std::forward< ParametersT >( params )... );
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
			ResT::cleanup( std::forward< ParametersT >( params )... );
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
			cleanup( std::forward< ParametersT >( paramsT )... );
			initialise( std::forward< ParametersU >( paramsU )... );
		}
	}

	//*********************************************************************************************
}

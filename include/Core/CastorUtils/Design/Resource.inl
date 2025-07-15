/*
See LICENSE file in root folder
*/
#include "CastorUtils/Design/Resource.hpp"

namespace c3d
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
		: ResT{ c3d::forward< ParametersT >( params )... }
	{
	}

	template< typename ResT, typename KeyT >
	ResourceT< ResT, KeyT >::ResourceT( ElementT && rhs )
		: ResT{ c3d::move( rhs ) }
	{
	}

	template< typename ResT, typename KeyT >
	ResourceT< ResT, KeyT > & ResourceT< ResT, KeyT >::operator=( ElementT && rhs )
	{
		ResT::operator=( c3d::move( rhs ) );
		return *this;
	}

	template< typename ResT, typename KeyT >
	template< typename ... ParametersT >
	void ResourceT< ResT, KeyT >::initialise( ParametersT && ... params )
	{
		if ( !m_initialised.exchange( true ) )
		{
			onInitialising( *this );
			ResT::initialise( c3d::forward< ParametersT >( params )... );
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
			ResT::cleanup( c3d::forward< ParametersT >( params )... );
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
			cleanup( c3d::forward< ParametersT >( paramsT )... );
			initialise( c3d::forward< ParametersU >( paramsU )... );
		}
	}

	//*********************************************************************************************
}

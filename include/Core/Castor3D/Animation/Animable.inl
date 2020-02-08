#include "Animable.hpp"

namespace castor3d
{
	//*************************************************************************************************

	template< typename OwnerT >
	inline Animable< OwnerT >::Animable( OwnerT & owner )
		: castor::OwnedBy< OwnerT >{ owner }
	{
	}

	template< typename OwnerT >
	inline void Animable< OwnerT >::cleanupAnimations()
	{
		m_animations.clear();
	}

	template< typename OwnerT >
	inline bool Animable< OwnerT >::hasAnimation( castor::String const & name )const
	{
		return m_animations.find( name ) != m_animations.end();
	}

	template< typename OwnerT >
	inline Animation< OwnerT > const & Animable< OwnerT >::getAnimation( castor::String const & name )const
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( cuT( "No animation named " ) + name );
		}

		return *it->second;
	}

	template< typename OwnerT >
	inline Animation< OwnerT > & Animable< OwnerT >::getAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( cuT( "No animation named " ) + name );
		}

		return *it->second;
	}

	template< typename OwnerT >
	inline void Animable< OwnerT >::doAddAnimation( AnimationSPtr && animation )
	{
		m_animations[animation->getName()] = std::move( animation );
	}

	template< typename OwnerT >
	template< typename Type >
	inline Type & Animable< OwnerT >::doGetAnimation( castor::String const & name )
	{
		return static_cast< Type & >( getAnimation( name ) );
	}

	//*************************************************************************************************
}

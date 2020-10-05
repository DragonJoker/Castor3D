#include "Animable.hpp"

namespace castor3d
{
	//*************************************************************************************************

	template< typename AnimableHanlerT >
	inline AnimableT< AnimableHanlerT >::AnimableT( AnimableHanlerT & owner )
		: castor::OwnedBy< AnimableHanlerT >{ owner }
	{
	}

	template< typename AnimableHanlerT >
	inline void AnimableT< AnimableHanlerT >::cleanupAnimations()
	{
		m_animations.clear();
	}

	template< typename AnimableHanlerT >
	inline bool AnimableT< AnimableHanlerT >::hasAnimation( castor::String const & name )const
	{
		return m_animations.find( name ) != m_animations.end();
	}

	template< typename AnimableHanlerT >
	inline AnimationT< AnimableHanlerT > const & AnimableT< AnimableHanlerT >::getAnimation( castor::String const & name )const
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( cuT( "No animation named [" ) + name + "]" );
		}

		return *it->second;
	}

	template< typename AnimableHanlerT >
	inline AnimationT< AnimableHanlerT > & AnimableT< AnimableHanlerT >::getAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( cuT( "No animation named [" ) + name + "]" );
		}

		return *it->second;
	}

	template< typename AnimableHanlerT >
	inline void AnimableT< AnimableHanlerT >::doAddAnimation( AnimationPtr animation )
	{
		CU_Require( animation != nullptr );
		auto it = m_animations.emplace( animation->getName(), nullptr ).first;

		if ( it->second )
		{
			CU_Exception( cuT( "Already an animation named [" ) + animation->getName() + "]" );
		}

		it->second = std::move( animation );
	}

	template< typename AnimableHanlerT >
	inline void AnimableT< AnimableHanlerT >::doRemoveAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( cuT( "No animation named [" ) + name + "]" );
		}

		m_animations.erase( it );
	}

	template< typename AnimableHanlerT >
	template< typename AnimationType >
	inline AnimationType & AnimableT< AnimableHanlerT >::doGetAnimation( castor::String const & name )
	{
		return static_cast< AnimationType & >( getAnimation( name ) );
	}

	//*************************************************************************************************
}

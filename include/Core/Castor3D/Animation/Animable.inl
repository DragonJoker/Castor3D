#include "Animable.hpp"

namespace castor3d
{
	//*************************************************************************************************

	template< typename AnimableHandlerT >
	inline AnimableT< AnimableHandlerT >::AnimableT( AnimableHandlerT & owner )
		: castor::OwnedBy< AnimableHandlerT >{ owner }
	{
	}

	template< typename AnimableHandlerT >
	inline void AnimableT< AnimableHandlerT >::cleanupAnimations()
	{
		m_animations.clear();
	}

	template< typename AnimableHandlerT >
	inline bool AnimableT< AnimableHandlerT >::hasAnimation()const
	{
		return !m_animations.empty();
	}

	template< typename AnimableHandlerT >
	inline bool AnimableT< AnimableHandlerT >::hasAnimation( castor::String const & name )const
	{
		return m_animations.find( name ) != m_animations.end();
	}

	template< typename AnimableHandlerT >
	inline AnimationT< AnimableHandlerT > const & AnimableT< AnimableHandlerT >::getAnimation( castor::String const & name )const
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( cuT( "No animation named [" ) + name + "]" );
		}

		return *it->second;
	}

	template< typename AnimableHandlerT >
	inline AnimationT< AnimableHandlerT > & AnimableT< AnimableHandlerT >::getAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( cuT( "No animation named [" ) + name + "]" );
		}

		return *it->second;
	}

	template< typename AnimableHandlerT >
	inline void AnimableT< AnimableHandlerT >::addAnimation( AnimationPtr animation )
	{
		CU_Require( animation != nullptr );
		auto it = m_animations.emplace( animation->getName(), nullptr ).first;

		if ( it->second )
		{
			CU_Exception( cuT( "Already an animation named [" ) + animation->getName() + "]" );
		}

		it->second = std::move( animation );
	}

	template< typename AnimableHandlerT >
	inline void AnimableT< AnimableHandlerT >::doRemoveAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( cuT( "No animation named [" ) + name + "]" );
		}

		m_animations.erase( it );
	}

	template< typename AnimableHandlerT >
	template< typename AnimationType >
	inline AnimationType & AnimableT< AnimableHandlerT >::doGetAnimation( castor::String const & name )
	{
		return static_cast< AnimationType & >( getAnimation( name ) );
	}

	template< typename AnimableHandlerT >
	template< typename AnimationType >
	inline AnimationType const & AnimableT< AnimableHandlerT >::doGetAnimation( castor::String const & name )const
	{
		return static_cast< AnimationType const & >( getAnimation( name ) );
	}

	//*************************************************************************************************
}

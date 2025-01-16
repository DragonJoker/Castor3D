#include "Castor3D/Animation/Animable.hpp"

#include "Castor3D/Animation/Animation.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

CU_ImplementSmartPtr( castor3d, Animable )

namespace castor3d
{
	//*************************************************************************************************

	Animable::Animable( Engine & owner )
		: castor::OwnedBy< Engine >{ owner }
	{
	}

	void Animable::cleanupAnimations()
	{
		m_animations.clear();
	}

	bool Animable::hasAnimation()const noexcept
	{
		return !m_animations.empty();
	}

	bool Animable::hasAnimation( castor::String const & name )const
	{
		return m_animations.find( name ) != m_animations.end();
	}

	Animation const & Animable::getAnimation( castor::String const & name )const
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( "No animation named [" + castor::toUtf8( name ) + "]" );
		}

		return *it->second;
	}

	Animation & Animable::getAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( "No animation named [" + castor::toUtf8( name ) + "]" );
		}

		return *it->second;
	}

	void Animable::addAnimation( AnimationPtr animation )
	{
		CU_Require( animation != nullptr );
		auto it = m_animations.try_emplace( animation->getName(), nullptr ).first;

		if ( it->second )
		{
			uint32_t index{};

			while ( it->second )
			{
				auto name = animation->getName() + castor::string::toString( index );
				++index;
				it = m_animations.try_emplace( name, nullptr ).first;
			}
		}

		it->second = castor::move( animation );
	}

	void Animable::doRemoveAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it != m_animations.end() )
		{
			m_animations.erase( it );
		}
	}

	//*************************************************************************************************
}

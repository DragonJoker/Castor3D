#include "Animable.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Animable >::DoWrite( Animable const & p_obj )
	{
		bool l_return = true;

		for ( auto && l_it : p_obj.m_animations )
		{
			l_return &= BinaryWriter< Animation >{}.Write( *l_it.second, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< Animable >::DoParse( Animable & p_obj )
	{
		bool l_return = true;
		AnimationSPtr l_animation;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_ANIMATION:
				l_animation = std::make_shared< Animation >( p_obj );
				l_return = BinaryParser< Animation >{}.Parse( *l_animation, l_chunk );

				if ( l_return )
				{
					p_obj.m_animations.insert( { l_animation->GetName(), l_animation } );
				}

				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Animable::Animable()
	{
	}

	Animable::~Animable()
	{
	}

	AnimationSPtr Animable::CreateAnimation( Castor::String const & p_name )
	{
		AnimationSPtr l_return;
		auto l_it = m_animations.find( p_name );

		if ( l_it == m_animations.end() )
		{
			l_return = std::make_shared< Animation >( *this, p_name );
			m_animations.insert( { p_name, l_return } );
		}
		else
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void Animable::CleanupAnimations()
	{
		m_animations.clear();
	}

	AnimationSPtr Animable::GetAnimation( Castor::String const & p_name )
	{
		AnimationSPtr l_return;
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	//*************************************************************************************************
}

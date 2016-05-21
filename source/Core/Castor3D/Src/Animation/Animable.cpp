#include "Animable.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Animable::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Animable >( p_path )
	{
	}

	bool Animable::BinaryParser::Fill( Animable const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_ANIMABLE );

		for ( auto && l_it : p_obj.m_animations )
		{
			l_return &= Animation::BinaryParser( m_path ).Fill( *l_it.second, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool Animable::BinaryParser::Parse( Animable & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		AnimationSPtr l_animation;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_ANIMATION:
					l_animation = std::make_shared< Animation >( cuEmptyString );
					l_return = Animation::BinaryParser( m_path ).Parse( *l_animation, l_chunk );

					if ( l_return )
					{
						p_obj.m_animations.insert( std::make_pair( l_animation->GetName(), l_animation ) );
					}

					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
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
			l_return = std::make_shared< Animation >( p_name );
			m_animations.insert( std::make_pair( p_name, l_return ) );
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

#include "Animable.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Animable::BinaryWriter::BinaryWriter( Path const & p_path )
		: Castor3D::BinaryWriter< Animable >( p_path )
	{
	}

	bool Animable::BinaryWriter::DoWrite( Animable const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		for ( auto && l_it : p_obj.m_animations )
		{
			l_return &= Animation::BinaryWriter{ m_path }.Write( *l_it.second, p_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	Animable::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Animable >( p_path )
	{
	}

	bool Animable::BinaryParser::DoParse( Animable & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		AnimationSPtr l_animation;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_ANIMATION:
			l_animation = std::make_shared< Animation >( p_obj );
			l_return = Animation::BinaryParser{ m_path }.Parse( *l_animation, p_chunk );

			if ( l_return )
			{
				p_obj.m_animations.insert( { l_animation->GetName(), l_animation } );
			}

			break;
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

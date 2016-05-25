#include "Animation.hpp"

#include "Bone.hpp"
#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationNode.hpp"
#include "SkeletonAnimationObject.hpp"

#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		Castor::String const & GetMovingTypeName( eANIMATION_OBJECT_TYPE p_type )
		{
			static std::array< String, eANIMATION_OBJECT_TYPE_COUNT > const Names
			{
				cuT( "Node_" ),
				cuT( "Object_" ),
				cuT( "Bone_" ),
			};

			return Names[p_type];
		}

		void RecursiveAddChildren( Animation & p_animation, AnimationObjectSPtr p_moving, AnimationObjectSPtr p_parent )
		{
			if ( !p_animation.HasObject( p_moving->GetType(), p_moving->GetName() ) )
			{
				p_animation.AddObject( p_moving, p_parent );

				for ( auto l_moving : p_moving->GetChildren() )
				{
					RecursiveAddChildren( p_animation, l_moving, p_moving );
				}
			}
		}
	}

	//*************************************************************************************************

	Animation::BinaryWriter::BinaryWriter( Path const & p_path )
		: Castor3D::BinaryWriter< Animation >( p_path )
	{
	}

	bool Animation::BinaryWriter::DoWrite( Animation const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_ANIMATION );

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetScale(), eCHUNK_TYPE_ANIM_SCALE, l_chunk );
		}

		for ( auto && l_moving : p_obj.m_arrayMoving )
		{
			switch ( l_moving->GetType() )
			{
			case eANIMATION_OBJECT_TYPE_NODE:
				l_return &= SkeletonAnimationNode::BinaryWriter{ m_path }.Write( *std::static_pointer_cast< SkeletonAnimationNode >( l_moving ), l_chunk );
				break;

			case eANIMATION_OBJECT_TYPE_OBJECT:
				l_return &= SkeletonAnimationObject::BinaryWriter{ m_path }.Write( *std::static_pointer_cast< SkeletonAnimationObject >( l_moving ), l_chunk );
				break;

			case eANIMATION_OBJECT_TYPE_BONE:
				l_return &= SkeletonAnimationBone::BinaryWriter{ m_path }.Write( *std::static_pointer_cast< SkeletonAnimationBone >( l_moving ), l_chunk );
				break;
			}
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	Animation::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Animation >( p_path )
	{
	}

	bool Animation::BinaryParser::DoParse( Animation & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		SkeletonAnimationNodeSPtr l_node;
		SkeletonAnimationObjectSPtr l_object;
		SkeletonAnimationBoneSPtr l_bone;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NAME:
					l_return = DoParseChunk( l_name, l_chunk );

					if ( l_return )
					{
						p_obj.m_name = l_name;
					}

					break;

				case eCHUNK_TYPE_ANIM_SCALE:
					l_return = DoParseChunk( p_obj.m_scale, l_chunk );
					break;

				case eCHUNK_TYPE_MOVING_NODE:
					l_node = std::make_shared< SkeletonAnimationNode >( p_obj );
					l_return = SkeletonAnimationNode::BinaryParser( m_path ).Parse( *l_node, l_chunk );

					if ( l_return )
					{
						RecursiveAddChildren( p_obj, l_node, nullptr );
					}

					break;

				case eCHUNK_TYPE_MOVING_OBJECT:
					l_object = std::make_shared< SkeletonAnimationObject >( p_obj );
					l_return = SkeletonAnimationObject::BinaryParser( m_path ).Parse( *l_object, l_chunk );

					if ( l_return )
					{
						RecursiveAddChildren( p_obj, l_object, nullptr );
					}

					break;

				case eCHUNK_TYPE_MOVING_BONE:
					l_bone = std::make_shared< SkeletonAnimationBone >( p_obj );
					l_return = SkeletonAnimationBone::BinaryParser( m_path ).Parse( *l_bone, l_chunk );

					if ( l_return )
					{
						RecursiveAddChildren( p_obj, l_bone, nullptr );
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

	Animation::Animation( Animable & p_animable, String const & p_name )
		: Named{ p_name }
		, OwnedBy< Animable >{ p_animable }
	{
	}

	Animation::~Animation()
	{
	}

	void Animation::Update( real p_tslf )
	{
		if ( m_state != eANIMATION_STATE_STOPPED )
		{
			if ( m_length == 0 )
			{
				for ( auto l_it : m_toMove )
				{
					m_length = std::max( m_length, l_it.second->GetLength() );
				}
			}

			if ( m_state == eANIMATION_STATE_PLAYING )
			{
				m_currentTime += ( p_tslf * m_scale );

				if ( m_currentTime >= m_length )
				{
					if ( !m_looped )
					{
						m_state = eANIMATION_STATE_PAUSED;
						m_currentTime = m_length;
					}
					else
					{
						do
						{
							m_currentTime -= m_length;
						}
						while ( m_currentTime >= m_length );
					}
				}
				else if ( m_currentTime < 0.0_r )
				{
					if ( !m_looped )
					{
						m_state = eANIMATION_STATE_PAUSED;
						m_currentTime = 0.0_r;
					}
					else
					{
						do
						{
							m_currentTime += m_length;
						}
						while ( m_currentTime < 0.0_r );
					}
				}
			}

			for ( auto l_moving : m_arrayMoving )
			{
				l_moving->Update( m_currentTime, Matrix4x4r{ 1 } );
			}
		}
	}

	void Animation::Play()
	{
		m_state = eANIMATION_STATE_PLAYING;
	}

	void Animation::Pause()
	{
		if ( m_state == eANIMATION_STATE_PLAYING )
		{
			m_state = eANIMATION_STATE_PAUSED;
		}
	}

	void Animation::Stop()
	{
		if ( m_state != eANIMATION_STATE_STOPPED )
		{
			m_state = eANIMATION_STATE_STOPPED;
			m_currentTime = 0.0;
		}
	}

	AnimationObjectSPtr Animation::AddObject( Castor::String const & p_name, AnimationObjectSPtr p_parent )
	{
		std::shared_ptr< SkeletonAnimationNode > l_return = std::make_shared< SkeletonAnimationNode >( *this, p_name );
		String l_name = GetMovingTypeName( eANIMATION_OBJECT_TYPE_NODE ) + p_name;
		auto l_it = m_toMove.find( l_name );

		if ( l_it == m_toMove.end() )
		{
			m_toMove.insert( std::make_pair( l_name, l_return ) );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( l_return );
			}
		}
		else
		{
			Logger::LogWarning( "Can't add this node : already added" );
		}

		return l_return;
	}

	AnimationObjectSPtr Animation::AddObject( GeometrySPtr p_object, AnimationObjectSPtr p_parent )
	{
		AnimationObjectSPtr l_return;
		String l_name = GetMovingTypeName( eANIMATION_OBJECT_TYPE_OBJECT ) + p_object->GetName();
		auto l_it = m_toMove.find( l_name );

		if ( l_it == m_toMove.end() )
		{
			std::shared_ptr< SkeletonAnimationObject > l_moving = std::make_shared< SkeletonAnimationObject >( *this );
			l_moving->SetObject( p_object );
			l_return = l_moving;
			m_toMove.insert( std::make_pair( l_name, l_return ) );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( l_return );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this movable : already added" );
		}

		return l_return;
	}

	AnimationObjectSPtr Animation::AddObject( BoneSPtr p_bone, AnimationObjectSPtr p_parent )
	{
		AnimationObjectSPtr l_return;
		String l_name = GetMovingTypeName( eANIMATION_OBJECT_TYPE_BONE ) + p_bone->GetName();
		auto l_it = m_toMove.find( l_name );

		if ( l_it == m_toMove.end() )
		{
			std::shared_ptr< SkeletonAnimationBone > l_moving = std::make_shared< SkeletonAnimationBone >( *this );
			l_moving->SetBone( p_bone );

			if ( p_parent && p_parent->GetType() == eANIMATION_OBJECT_TYPE_BONE )
			{
				p_bone->SetFinalTransformation( std::static_pointer_cast< SkeletonAnimationBone >( p_parent )->GetBone()->GetFinalTransformation() * p_bone->GetOffsetMatrix() );
			}

			l_return = l_moving;
			m_toMove.insert( std::make_pair( l_name, l_return ) );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( l_return );
			}
		}
		else
		{
			Logger::LogWarning( "Can't add this bone : already added" );
		}

		return l_return;
	}

	void Animation::AddObject( AnimationObjectSPtr p_object, AnimationObjectSPtr p_parent )
	{
		String l_name = GetMovingTypeName( p_object->GetType() ) + p_object->GetName();
		auto l_it = m_toMove.find( l_name );

		if ( l_it == m_toMove.end() )
		{
			m_toMove.insert( std::make_pair( l_name, p_object ) );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( p_object );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "Can't add this object : already added" );
		}
	}

	bool Animation::HasObject( eANIMATION_OBJECT_TYPE p_type, Castor::String const & p_name )const
	{
		return m_toMove.find( GetMovingTypeName( p_type ) + p_name ) != m_toMove.end();
	}

	AnimationObjectSPtr Animation::GetObject( MovableObjectSPtr p_object )const
	{
		AnimationObjectSPtr l_return;
		auto l_it = m_toMove.find( GetMovingTypeName( eANIMATION_OBJECT_TYPE_OBJECT ) + p_object->GetName() );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	AnimationObjectSPtr Animation::GetObject( BoneSPtr p_bone )const
	{
		AnimationObjectSPtr l_return;
		auto l_it = m_toMove.find( GetMovingTypeName( eANIMATION_OBJECT_TYPE_BONE ) + p_bone->GetName() );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void Animation::SetInterpolationMode( eINTERPOLATOR_MODE p_mode )
	{
		for ( auto l_moving : m_arrayMoving )
		{
			l_moving->SetInterpolationMode( p_mode );
		}
	}

	AnimationSPtr Animation::Clone( Animable & p_animable )const
	{
		auto l_clone = std::make_shared< Animation >( p_animable, GetName() );
		AnimationObjectPtrStrMap l_toMove;

		for ( auto l_moving : m_arrayMoving )
		{
			auto l_mcln = l_moving->Clone( *l_clone );
			ENSURE( l_mcln );

			if ( l_mcln )
			{
				l_clone->m_arrayMoving.push_back( l_mcln );
				l_clone->m_toMove.insert( std::make_pair( GetMovingTypeName( l_mcln->GetType() ) + l_mcln->GetName(), l_mcln ) );
			}
		}

		return l_clone;
	}

	//*************************************************************************************************
}

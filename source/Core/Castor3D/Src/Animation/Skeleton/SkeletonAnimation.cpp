#include "SkeletonAnimation.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationNode.hpp"

#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		Castor::String const & GetMovingTypeName( SkeletonAnimationObjectType p_type )
		{
			static std::map< SkeletonAnimationObjectType, String > Names
			{
				{ SkeletonAnimationObjectType::Node, cuT( "Node_" ) },
				{ SkeletonAnimationObjectType::Bone, cuT( "Bone_" ) },
			};

			return Names[p_type];
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimation >::DoWrite( SkeletonAnimation const & p_obj )
	{
		bool l_return = true;

		for ( auto l_moving : p_obj.m_arrayMoving )
		{
			switch ( l_moving->GetType() )
			{
			case SkeletonAnimationObjectType::Node:
				l_return &= BinaryWriter< SkeletonAnimationNode >{}.Write( *std::static_pointer_cast< SkeletonAnimationNode >( l_moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::Bone:
				l_return &= BinaryWriter< SkeletonAnimationBone >{}.Write( *std::static_pointer_cast< SkeletonAnimationBone >( l_moving ), m_chunk );
				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimation >::DoParse( SkeletonAnimation & p_obj )
	{
		bool l_return = true;
		SkeletonAnimationNodeSPtr l_node;
		SkeletonAnimationObjectSPtr l_object;
		SkeletonAnimationBoneSPtr l_bone;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_SKELETON_ANIMATION_NODE:
				l_node = std::make_shared< SkeletonAnimationNode >( p_obj );
				l_return = BinaryParser< SkeletonAnimationNode >{}.Parse( *l_node, l_chunk );

				if ( l_return )
				{
					p_obj.AddObject( l_node, nullptr );
				}

				break;

			case eCHUNK_TYPE_SKELETON_ANIMATION_BONE:
				l_bone = std::make_shared< SkeletonAnimationBone >( p_obj );
				l_return = BinaryParser< SkeletonAnimationBone >{}.Parse( *l_bone, l_chunk );

				if ( l_return )
				{
					p_obj.AddObject( l_bone, nullptr );
				}

				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	SkeletonAnimation::SkeletonAnimation( Animable & p_animable, String const & p_name )
		: Animation{ AnimationType::Skeleton, p_animable, p_name }
	{
	}

	SkeletonAnimation::~SkeletonAnimation()
	{
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::AddObject( Castor::String const & p_name, SkeletonAnimationObjectSPtr p_parent )
	{
		return AddObject( std::make_shared< SkeletonAnimationNode >( *this, p_name ), p_parent );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::AddObject( BoneSPtr p_bone, SkeletonAnimationObjectSPtr p_parent )
	{
		std::shared_ptr< SkeletonAnimationBone > l_return = std::make_shared< SkeletonAnimationBone >( *this );
		l_return->SetBone( p_bone );
		auto l_added = AddObject( l_return, p_parent );
		return l_return;
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::AddObject( SkeletonAnimationObjectSPtr p_object, SkeletonAnimationObjectSPtr p_parent )
	{
		String l_name = GetMovingTypeName( p_object->GetType() ) + p_object->GetName();
		auto l_it = m_toMove.find( l_name );
		SkeletonAnimationObjectSPtr l_return;

		if ( l_it == m_toMove.end() )
		{
			m_toMove.insert( { l_name, p_object } );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( p_object );
			}

			l_return = p_object;
		}
		else
		{
			Logger::LogWarning( cuT( "This object was already added" ) );
			l_return = l_it->second;
		}

		return l_return;
	}

	bool SkeletonAnimation::HasObject( SkeletonAnimationObjectType p_type, Castor::String const & p_name )const
	{
		return m_toMove.find( GetMovingTypeName( p_type ) + p_name ) != m_toMove.end();
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::GetObject( Bone const & p_bone )const
	{
		return GetObject( SkeletonAnimationObjectType::Node, p_bone.GetName() );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::GetObject( String const & p_name )const
	{
		return GetObject( SkeletonAnimationObjectType::Node, p_name );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::GetObject( SkeletonAnimationObjectType p_type, Castor::String const & p_name )const
	{
		SkeletonAnimationObjectSPtr l_return;
		auto l_it = m_toMove.find( GetMovingTypeName( p_type ) + p_name );

		if ( l_it != m_toMove.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void SkeletonAnimation::DoUpdateLength()
	{
		for ( auto l_it : m_toMove )
		{
			m_length = std::max( m_length, l_it.second->GetLength() );
		}
	}

	//*************************************************************************************************
}

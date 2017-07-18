#include "SkeletonAnimation.hpp"

#include "Engine.hpp"

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
				{ SkeletonAnimationObjectType::eNode, cuT( "Node_" ) },
				{ SkeletonAnimationObjectType::eBone, cuT( "Bone_" ) },
			};

			return Names[p_type];
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimation >::DoWrite( SkeletonAnimation const & p_obj )
	{
		bool l_result = true;

		for ( auto l_moving : p_obj.m_arrayMoving )
		{
			switch ( l_moving->GetType() )
			{
			case SkeletonAnimationObjectType::eNode:
				l_result &= BinaryWriter< SkeletonAnimationNode >{}.Write( *std::static_pointer_cast< SkeletonAnimationNode >( l_moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				l_result &= BinaryWriter< SkeletonAnimationBone >{}.Write( *std::static_pointer_cast< SkeletonAnimationBone >( l_moving ), m_chunk );
				break;
			}
		}

		return l_result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimation >::DoParse( SkeletonAnimation & p_obj )
	{
		bool l_result = true;
		SkeletonAnimationNodeSPtr l_node;
		SkeletonAnimationObjectSPtr l_object;
		SkeletonAnimationBoneSPtr l_bone;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_result && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eSkeletonAnimationNode:
				l_node = std::make_shared< SkeletonAnimationNode >( p_obj );
				l_result = BinaryParser< SkeletonAnimationNode >{}.Parse( *l_node, l_chunk );

				if ( l_result )
				{
					p_obj.AddObject( l_node, nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationBone:
				l_bone = std::make_shared< SkeletonAnimationBone >( p_obj );
				l_result = BinaryParser< SkeletonAnimationBone >{}.Parse( *l_bone, l_chunk );

				if ( l_result )
				{
					p_obj.AddObject( l_bone, nullptr );
				}

				break;
			}
		}

		return l_result;
	}

	//*************************************************************************************************

	SkeletonAnimation::SkeletonAnimation( Animable & p_animable, String const & p_name )
		: Animation{ AnimationType::eSkeleton, p_animable, p_name }
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
		std::shared_ptr< SkeletonAnimationBone > l_result = std::make_shared< SkeletonAnimationBone >( *this );
		l_result->SetBone( p_bone );
		auto l_added = AddObject( l_result, p_parent );
		return l_result;
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::AddObject( SkeletonAnimationObjectSPtr p_object, SkeletonAnimationObjectSPtr p_parent )
	{
		String l_name = GetMovingTypeName( p_object->GetType() ) + p_object->GetName();
		auto l_it = m_toMove.find( l_name );
		SkeletonAnimationObjectSPtr l_result;

		if ( l_it == m_toMove.end() )
		{
			m_toMove.insert( { l_name, p_object } );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( p_object );
			}

			l_result = p_object;
		}
		else
		{
			Logger::LogWarning( cuT( "This object was already added" ) );
			l_result = l_it->second;
		}

		return l_result;
	}

	bool SkeletonAnimation::HasObject( SkeletonAnimationObjectType p_type, Castor::String const & p_name )const
	{
		return m_toMove.find( GetMovingTypeName( p_type ) + p_name ) != m_toMove.end();
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::GetObject( Bone const & p_bone )const
	{
		return GetObject( SkeletonAnimationObjectType::eNode, p_bone.GetName() );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::GetObject( String const & p_name )const
	{
		return GetObject( SkeletonAnimationObjectType::eNode, p_name );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::GetObject( SkeletonAnimationObjectType p_type, Castor::String const & p_name )const
	{
		SkeletonAnimationObjectSPtr l_result;
		auto l_it = m_toMove.find( GetMovingTypeName( p_type ) + p_name );

		if ( l_it != m_toMove.end() )
		{
			l_result = l_it->second;
		}

		return l_result;
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

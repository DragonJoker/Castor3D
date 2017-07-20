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
		bool result = true;

		for ( auto moving : p_obj.m_arrayMoving )
		{
			switch ( moving->GetType() )
			{
			case SkeletonAnimationObjectType::eNode:
				result &= BinaryWriter< SkeletonAnimationNode >{}.Write( *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				result &= BinaryWriter< SkeletonAnimationBone >{}.Write( *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimation >::DoParse( SkeletonAnimation & p_obj )
	{
		bool result = true;
		SkeletonAnimationNodeSPtr node;
		SkeletonAnimationObjectSPtr object;
		SkeletonAnimationBoneSPtr bone;
		String name;
		BinaryChunk chunk;

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eSkeletonAnimationNode:
				node = std::make_shared< SkeletonAnimationNode >( p_obj );
				result = BinaryParser< SkeletonAnimationNode >{}.Parse( *node, chunk );

				if ( result )
				{
					p_obj.AddObject( node, nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationBone:
				bone = std::make_shared< SkeletonAnimationBone >( p_obj );
				result = BinaryParser< SkeletonAnimationBone >{}.Parse( *bone, chunk );

				if ( result )
				{
					p_obj.AddObject( bone, nullptr );
				}

				break;
			}
		}

		return result;
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
		std::shared_ptr< SkeletonAnimationBone > result = std::make_shared< SkeletonAnimationBone >( *this );
		result->SetBone( p_bone );
		auto added = AddObject( result, p_parent );
		return result;
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::AddObject( SkeletonAnimationObjectSPtr p_object, SkeletonAnimationObjectSPtr p_parent )
	{
		String name = GetMovingTypeName( p_object->GetType() ) + p_object->GetName();
		auto it = m_toMove.find( name );
		SkeletonAnimationObjectSPtr result;

		if ( it == m_toMove.end() )
		{
			m_toMove.insert( { name, p_object } );

			if ( !p_parent )
			{
				m_arrayMoving.push_back( p_object );
			}

			result = p_object;
		}
		else
		{
			Logger::LogWarning( cuT( "This object was already added" ) );
			result = it->second;
		}

		return result;
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
		SkeletonAnimationObjectSPtr result;
		auto it = m_toMove.find( GetMovingTypeName( p_type ) + p_name );

		if ( it != m_toMove.end() )
		{
			result = it->second;
		}

		return result;
	}

	void SkeletonAnimation::DoUpdateLength()
	{
		for ( auto it : m_toMove )
		{
			m_length = std::max( m_length, it.second->GetLength() );
		}
	}

	//*************************************************************************************************
}

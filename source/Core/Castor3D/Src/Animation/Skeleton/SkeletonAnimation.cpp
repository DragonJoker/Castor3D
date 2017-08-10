#include "SkeletonAnimation.hpp"

#include "Engine.hpp"

#include "Mesh/Skeleton/Bone.hpp"
#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationNode.hpp"

#include "Scene/Geometry.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		castor::String const & getMovingTypeName( SkeletonAnimationObjectType p_type )
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

	bool BinaryWriter< SkeletonAnimation >::doWrite( SkeletonAnimation const & p_obj )
	{
		bool result = true;

		for ( auto moving : p_obj.m_arrayMoving )
		{
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				result &= BinaryWriter< SkeletonAnimationNode >{}.write( *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				result &= BinaryWriter< SkeletonAnimationBone >{}.write( *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimation >::doParse( SkeletonAnimation & p_obj )
	{
		bool result = true;
		SkeletonAnimationNodeSPtr node;
		SkeletonAnimationObjectSPtr object;
		SkeletonAnimationBoneSPtr bone;
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonAnimationNode:
				node = std::make_shared< SkeletonAnimationNode >( p_obj );
				result = BinaryParser< SkeletonAnimationNode >{}.parse( *node, chunk );

				if ( result )
				{
					p_obj.addObject( node, nullptr );
				}

				break;

			case ChunkType::eSkeletonAnimationBone:
				bone = std::make_shared< SkeletonAnimationBone >( p_obj );
				result = BinaryParser< SkeletonAnimationBone >{}.parse( *bone, chunk );

				if ( result )
				{
					p_obj.addObject( bone, nullptr );
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

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( castor::String const & p_name, SkeletonAnimationObjectSPtr p_parent )
	{
		return addObject( std::make_shared< SkeletonAnimationNode >( *this, p_name ), p_parent );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( BoneSPtr p_bone, SkeletonAnimationObjectSPtr p_parent )
	{
		std::shared_ptr< SkeletonAnimationBone > result = std::make_shared< SkeletonAnimationBone >( *this );
		result->setBone( p_bone );
		auto added = addObject( result, p_parent );
		return result;
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( SkeletonAnimationObjectSPtr p_object, SkeletonAnimationObjectSPtr p_parent )
	{
		String name = getMovingTypeName( p_object->getType() ) + p_object->getName();
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
			Logger::logWarning( cuT( "This object was already added" ) );
			result = it->second;
		}

		return result;
	}

	bool SkeletonAnimation::hasObject( SkeletonAnimationObjectType p_type, castor::String const & p_name )const
	{
		return m_toMove.find( getMovingTypeName( p_type ) + p_name ) != m_toMove.end();
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( Bone const & p_bone )const
	{
		return getObject( SkeletonAnimationObjectType::eNode, p_bone.getName() );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( String const & p_name )const
	{
		return getObject( SkeletonAnimationObjectType::eNode, p_name );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( SkeletonAnimationObjectType p_type, castor::String const & p_name )const
	{
		SkeletonAnimationObjectSPtr result;
		auto it = m_toMove.find( getMovingTypeName( p_type ) + p_name );

		if ( it != m_toMove.end() )
		{
			result = it->second;
		}

		return result;
	}

	void SkeletonAnimation::doUpdateLength()
	{
		for ( auto it : m_toMove )
		{
			m_length = std::max( m_length, it.second->getLength() );
		}
	}

	//*************************************************************************************************
}

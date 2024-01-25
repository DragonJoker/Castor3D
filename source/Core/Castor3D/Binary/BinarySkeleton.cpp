#include "Castor3D/Binary/BinarySkeleton.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Binary/BinaryAnimation.hpp"
#include "Castor3D/Binary/BinaryBoneNode.hpp"
#include "Castor3D/Binary/BinarySkeletonNode.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Skeleton >::doWrite( Skeleton const & obj )
	{
		bool result = doWriteChunk( obj.getGlobalInverseTransform(), ChunkType::eSkeletonGlobalInverse, m_chunk );

		for ( auto & node : obj.m_nodes )
		{
			switch ( node->getType() )
			{
			case SkeletonNodeType::eNode:
				result = result && BinaryWriter< SkeletonNode >{}.write( *node, m_chunk );
				break;
			case SkeletonNodeType::eBone:
				result = result && BinaryWriter< BoneNode >{}.write( static_cast< BoneNode const & >( *node ), m_chunk );
				break;
			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< Skeleton >::Name = cuT( "Skeleton" );

	bool BinaryParser< Skeleton >::doParse( Skeleton & obj )
	{
		bool result = true;
		SkeletonNodeUPtr node;
		BinaryChunk chunk{ doIsLittleEndian() };
		SkeletonAnimationUPtr animation;
		castor::Map< SkeletonNode *, castor::String > hierarchy;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonGlobalInverse:
				result = doParseChunk( obj.m_globalInverse, chunk );
				checkError( result, cuT( "Couldn't parse global inverse matrix." ) );
				break;
			case ChunkType::eSkeletonBone:
				{
					node = castor::makeUniqueDerived< SkeletonNode, BoneNode >( castor::cuEmptyString, obj, castor::Matrix4x4f{ 1.0f }, 0u );
					auto & bone = static_cast< BoneNode & >( *node );
					auto parser = createBinaryParser< BoneNode >();
					result = parser.parse( bone, chunk );
					checkError( result, cuT( "Couldn't parse bone." ) );

					if ( result )
					{
						if ( m_fileVersion > Version{ 1, 5, 0 } )
						{
							if ( !parser.parentName.empty() )
							{
								hierarchy.try_emplace( &bone, parser.parentName );
							}
						}
						else
						{
							bone.m_id = uint32_t( obj.m_bones.size() );
						}

						obj.m_bones.emplace_back( &bone );
						obj.m_nodes.emplace_back( castor::move( node ) );
					}
				}
				break;
			case ChunkType::eSkeletonNode:
				{
					node = castor::makeUnique< SkeletonNode >( castor::cuEmptyString, obj );
					auto parser = createBinaryParser< SkeletonNode >();
					result = parser.parse( *node, chunk );
					checkError( result, cuT( "Couldn't parse bone." ) );

					if ( result )
					{
						if ( m_fileVersion > Version{ 1, 5, 0 }
							&& !parser.parentName.empty() )
						{
							hierarchy.try_emplace( node.get(), parser.parentName );
						}

						obj.m_nodes.emplace_back( castor::move( node ) );
					}
				}
				break;
			case ChunkType::eAnimation:
				animation = castor::makeUnique< SkeletonAnimation >( obj );
				result = createBinaryParser< Animation >().parse( *animation, chunk );
				checkError( result, cuT( "Couldn't parse animation." ) );
				if ( result )
				{
					auto name = animation->getName();
					obj.m_animations.try_emplace( name
						, castor::ptrRefCast< Animation >( animation ) );
				}
				break;

			default:
				break;
			}
		}

		if ( m_fileVersion > Version{ 1, 5, 0 } )
		{
			for ( auto const & [subnode, name] : hierarchy )
			{
				if ( result )
				{
					auto parent = obj.findNode( name );
					result = parent != nullptr;
					checkError( result, cuT( "Couldn't find parent node." ) );

					if ( result )
					{
						obj.setNodeParent( *subnode, *parent );
					}
				}
			}
		}

		return result;
	}
}

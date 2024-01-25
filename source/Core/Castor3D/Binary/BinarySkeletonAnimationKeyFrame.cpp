#include "Castor3D/Binary/BinarySkeletonAnimationKeyFrame.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationKeyFrame >::doWrite( SkeletonAnimationKeyFrame const & obj )
	{
		bool result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eSkeletonAnimationKeyFrameTime, m_chunk );

		for ( auto & it : obj )
		{
			if ( result )
			{
				result = doWriteChunk( uint8_t( it.object->getType() ), ChunkType::eSkeletonAnimationKeyFrameObjectType, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.object->getName(), ChunkType::eSkeletonAnimationKeyFrameObjectName, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.transform.translate, ChunkType::eSkeletonAnimationKeyFrameObjectTranslate, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.transform.rotate, ChunkType::eSkeletonAnimationKeyFrameObjectRotate, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.transform.scale, ChunkType::eSkeletonAnimationKeyFrameObjectScale, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< SkeletonAnimationKeyFrame >::Name = cuT( "SkeletonAnimationKeyFrame" );

	bool BinaryParser< SkeletonAnimationKeyFrame >::doParse( SkeletonAnimationKeyFrame & obj )
	{
		bool result = true;
		double time{ 0.0 };
		castor::String name;
		uint8_t type{};
		BinaryChunk chunk{ doIsLittleEndian() };
		castor::Point3f translate;
		castor::Point3f scale;
		castor::Quaternion rotate;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonAnimationKeyFrameTime:
				result = doParseChunk( time, chunk );
				checkError( result, cuT( "Couldn't parse time index." ) );
				obj.doSetTimeIndex( castor::Milliseconds{ int64_t( time * 1000 ) } );
				break;
			case ChunkType::eSkeletonAnimationKeyFrameObjectType:
				result = doParseChunk( type, chunk );
				checkError( result, cuT( "Couldn't parse object type." ) );
				break;
			case ChunkType::eSkeletonAnimationKeyFrameObjectName:
				result = doParseChunk( name, chunk );
				checkError( result, cuT( "Couldn't parse object name." ) );
				break;
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
			case ChunkType::eSkeletonAnimationKeyFrameObjectTransform:
#pragma GCC diagnostic pop
#pragma warning( pop )
				if ( m_fileVersion > Version{ 1, 5, 0 } )
				{
					castor::Matrix4x4f matrix;
					result = doParseChunk( matrix, chunk );
					checkError( result, cuT( "Couldn't parse object transform." ) );

					if ( result )
					{
						castor::matrix::decompose( matrix, translate, scale, rotate );
						obj.addAnimationObject( *obj.getOwner()->getObject( SkeletonNodeType( type ), name )
							, translate
							, rotate
							, scale );
					}
				}
				break;
			case ChunkType::eSkeletonAnimationKeyFrameObjectTranslate:
				result = doParseChunk( translate, chunk );
				checkError( result, cuT( "Couldn't parse object translate." ) );
				break;
			case ChunkType::eSkeletonAnimationKeyFrameObjectRotate:
				result = doParseChunk( rotate, chunk );
				checkError( result, cuT( "Couldn't parse object rotate." ) );
				break;
			case ChunkType::eSkeletonAnimationKeyFrameObjectScale:
				result = doParseChunk( scale, chunk );
				checkError( result, cuT( "Couldn't parse object scale." ) );
				if ( result )
				{
					obj.addAnimationObject( *obj.getOwner()->getObject( SkeletonNodeType( type )
						, name )
						, translate
						, rotate
						, scale );
				}
				break;
			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< SkeletonAnimationKeyFrame >::doParse_v1_3( SkeletonAnimationKeyFrame & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 3, 0 } )
		{
			castor::SquareMatrix< double, 4 > matrix;
			castor::String name;
			uint8_t type{};
			BinaryChunk chunk{ doIsLittleEndian() };

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
				case ChunkType::eSkeletonAnimationKeyFrameObjectType:
					result = doParseChunk( type, chunk );
					checkError( result, cuT( "Couldn't parse object type." ) );
					break;
				case ChunkType::eSkeletonAnimationKeyFrameObjectName:
					result = doParseChunk( name, chunk );
					checkError( result, cuT( "Couldn't parse object name." ) );
					break;
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eSkeletonAnimationKeyFrameObjectTransform:
#pragma GCC diagnostic pop
#pragma warning( pop )
					result = doParseChunk( matrix, chunk );
					checkError( result, cuT( "Couldn't parse object transform." ) );
					if ( result )
					{
						castor::Point3f translate;
						castor::Point3f scale;
						castor::Quaternion rotate;
						castor::matrix::decompose( matrix, translate, scale, rotate );
						obj.addAnimationObject( *obj.getOwner()->getObject( SkeletonNodeType( type ), name )
							, translate
							, rotate
							, scale );
					}
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	bool BinaryParser< SkeletonAnimationKeyFrame >::doParse_v1_5( SkeletonAnimationKeyFrame & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 5, 0 } )
		{
			castor::Matrix4x4f matrix;
			castor::String name;
			uint8_t type{};
			BinaryChunk chunk{ doIsLittleEndian() };

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
				case ChunkType::eSkeletonAnimationKeyFrameObjectType:
					result = doParseChunk( type, chunk );
					checkError( result, cuT( "Couldn't parse object type." ) );
					break;
				case ChunkType::eSkeletonAnimationKeyFrameObjectName:
					result = doParseChunk( name, chunk );
					checkError( result, cuT( "Couldn't parse object name." ) );
					break;
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eSkeletonAnimationKeyFrameObjectTransform:
#pragma GCC diagnostic pop
#pragma warning( pop )
					if ( m_fileVersion > Version{ 1, 3, 0 } )
					{
						result = doParseChunk( matrix, chunk );
						checkError( result, cuT( "Couldn't parse object transform." ) );

						if ( result )
						{
							castor::Point3f translate;
							castor::Point3f scale;
							castor::Quaternion rotate;
							castor::matrix::decompose( matrix, translate, scale, rotate );
							obj.addAnimationObject( *obj.getOwner()->getObject( SkeletonNodeType( type ), name )
								, translate
								, rotate
								, scale );
						}
					}
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}

#include "Castor3D/Binary/BinarySceneNodeAnimationKeyFrame.hpp"

#include "Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp"

namespace castor3d
{
	bool BinaryWriter< SceneNodeAnimationKeyFrame >::doWrite( SceneNodeAnimationKeyFrame const & obj )
	{
		auto result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eSceneNodeAnimationKeyFrameTime, m_chunk );

		if ( result )
		{
			result = doWriteChunk( obj.getPosition(), ChunkType::eSceneNodeAnimationKeyFrameTranslate, m_chunk );
		}

		if ( result )
		{
			result = doWriteChunk( obj.getRotation(), ChunkType::eSceneNodeAnimationKeyFrameRotate, m_chunk );
		}

		if ( result )
		{
			result = doWriteChunk( obj.getScale(), ChunkType::eSceneNodeAnimationKeyFrameScale, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< SceneNodeAnimationKeyFrame >::Name = cuT( "SceneNodeAnimationKeyFrame" );

	bool BinaryParser< SceneNodeAnimationKeyFrame >::doParse( SceneNodeAnimationKeyFrame & obj )
	{
		bool result = true;
		BinaryChunk chunk;
		castor::Point3f vec{};
		castor::Quaternion quat{};
		double time{ 0.0 };
		SubmeshSPtr submesh;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSceneNodeAnimationKeyFrameTime:
				result = doParseChunk( time, chunk );
				checkError( result, "Couldn't parse time index." );
				obj.doSetTimeIndex( castor::Milliseconds{ int64_t( time * 1000 ) } );
				break;

			case ChunkType::eSceneNodeAnimationKeyFrameTranslate:
				result = doParseChunk( vec, chunk );
				checkError( result, "Couldn't parse translate." );

				if ( result )
				{
					obj.setPosition( vec );
				}

				break;

			case ChunkType::eSceneNodeAnimationKeyFrameRotate:
				result = doParseChunk( quat, chunk );
				checkError( result, "Couldn't parse rotate." );

				if ( result )
				{
					obj.setRotation( quat );
				}

				break;

			case ChunkType::eSceneNodeAnimationKeyFrameScale:
				result = doParseChunk( vec, chunk );
				checkError( result, "Couldn't parse scale." );

				if ( result )
				{
					obj.setScale( vec );
				}
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}

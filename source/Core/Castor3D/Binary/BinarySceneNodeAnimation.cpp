#include "Castor3D/Binary/BinarySceneNodeAnimation.hpp"

#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinarySceneNodeAnimationKeyFrame.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SceneNodeAnimation >::doWrite( SceneNodeAnimation const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		for ( auto const & keyframe : obj )
		{
			result = result && BinaryWriter< SceneNodeAnimationKeyFrame >{}.write( static_cast< SceneNodeAnimationKeyFrame const & >( *keyframe ), m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< SceneNodeAnimation >::Name = cuT( "SceneNodeAnimation" );

	bool BinaryParser< SceneNodeAnimation >::doParse( SceneNodeAnimation & obj )
	{
		bool result = true;
		SceneNodeAnimationKeyFrameUPtr keyFrame;
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );
				checkError( result, "Couldn't parse name." );

				if ( result )
				{
					obj.m_name = name;
				}

				break;

			case ChunkType::eSceneNodeAnimationKeyFrame:
				keyFrame = std::make_unique< SceneNodeAnimationKeyFrame >( obj, 0_ms );
				result = createBinaryParser< SceneNodeAnimationKeyFrame >().parse( *keyFrame, chunk );
				checkError( result, "Couldn't parse keyframe." );

				if ( result )
				{
					obj.addKeyFrame( std::move( keyFrame ) );
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

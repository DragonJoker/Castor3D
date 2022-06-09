#include "Castor3D/Binary/BinaryMeshAnimation.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinaryMeshAnimationKeyFrame.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::doWrite( MeshAnimation const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		for ( auto const & keyframe : obj )
		{
			result = result && BinaryWriter< MeshAnimationKeyFrame >{}.write( static_cast< MeshAnimationKeyFrame const & >( *keyframe ), m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< MeshAnimation >::Name = cuT( "MeshAnimation" );

	bool BinaryParser< MeshAnimation >::doParse( MeshAnimation & obj )
	{
		bool result = true;
		MeshAnimationKeyFrameUPtr keyFrame;
		castor::String name;
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

			case ChunkType::eMeshAnimationKeyFrame:
				keyFrame = std::make_unique< MeshAnimationKeyFrame >( obj, 0_ms );
				result = createBinaryParser< MeshAnimationKeyFrame >().parse( *keyFrame, chunk );
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

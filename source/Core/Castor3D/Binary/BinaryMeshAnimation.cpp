#include "Castor3D/Binary/BinaryMeshAnimation.hpp"

#include "Castor3D/Animation/Mesh/MeshAnimation.hpp"
#include "Castor3D/Animation/Mesh/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinaryMeshAnimationKeyFrame.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::doWrite( MeshAnimation const & obj )
	{
		bool result = true;

		for ( auto const & keyframe : obj )
		{
			result = result && BinaryWriter< MeshAnimationKeyFrame >{}.write( static_cast< MeshAnimationKeyFrame const & >( *keyframe ), m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimation >::doParse( MeshAnimation & obj )
	{
		bool result = true;
		MeshAnimationKeyFrameUPtr keyFrame;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMeshAnimationKeyFrame:
				keyFrame = std::make_unique< MeshAnimationKeyFrame >( obj, 0_ms );
				result = createBinaryParser< MeshAnimationKeyFrame >().parse( *keyFrame, chunk );

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

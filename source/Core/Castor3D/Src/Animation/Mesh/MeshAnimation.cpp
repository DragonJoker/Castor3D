#include "MeshAnimation.hpp"

#include "Engine.hpp"
#include "Animation/Mesh/MeshAnimationKeyFrame.hpp"
#include "Mesh/Submesh.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::doWrite( MeshAnimation const & obj )
	{
		bool result = true;

		for ( auto const & keyframe : obj )
		{
			result &= BinaryWriter< MeshAnimationKeyFrame >{}.write( static_cast< MeshAnimationKeyFrame const & >( *keyframe ), m_chunk );
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
				keyFrame = std::make_unique< MeshAnimationKeyFrame >( obj );
				result = BinaryParser< MeshAnimationKeyFrame >{}.parse( *keyFrame, chunk );

				if ( result )
				{
					obj.addKeyFrame( std::move( keyFrame ) );
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	MeshAnimation::MeshAnimation( Mesh & mesh
		, String const & name )
		: Animation{ AnimationType::eMesh
			, mesh
			, name }
	{
	}

	MeshAnimation::~MeshAnimation()
	{
	}

	void MeshAnimation::addChild( MeshAnimationSubmesh && object )
	{
		m_submeshes.push_back( std::move( object ) );
	}

	//*************************************************************************************************
}

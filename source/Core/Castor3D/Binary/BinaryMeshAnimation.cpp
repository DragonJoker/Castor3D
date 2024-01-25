#include "Castor3D/Binary/BinaryMeshAnimation.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"
#include "Castor3D/Binary/BinaryMeshMorphTarget.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::doWrite( MeshAnimation const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		for ( auto const & keyframe : obj )
		{
			result = result && BinaryWriter< MeshMorphTarget >{}.write( static_cast< MeshMorphTarget const & >( *keyframe ), m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< MeshAnimation >::Name = cuT( "MeshAnimation" );

	bool BinaryParser< MeshAnimation >::doParse( MeshAnimation & obj )
	{
		bool result = true;
		MeshMorphTargetUPtr keyFrame;
		castor::String name;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );
				checkError( result, cuT( "Couldn't parse name." ) );

				if ( result )
				{
					obj.m_name = name;
				}

				break;

			case ChunkType::eMeshMorphTarget:
				keyFrame = castor::makeUnique< MeshMorphTarget >( obj, 0_ms );
				result = createBinaryParser< MeshMorphTarget >().parse( *keyFrame, chunk );
				checkError( result, cuT( "Couldn't parse keyframe." ) );

				if ( result )
				{
					obj.addKeyFrame( castor::ptrRefCast< AnimationKeyFrame >( keyFrame ) );
				}

				break;

			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< MeshAnimation >::doParse_v1_5( MeshAnimation & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 5, 0 } )
		{
			MeshMorphTargetUPtr keyFrame;
			castor::String name;
			BinaryChunk chunk{ doIsLittleEndian() };

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
				case ChunkType::eName:
					result = doParseChunk( name, chunk );
					checkError( result, cuT( "Couldn't parse name." ) );

					if ( result )
					{
						obj.m_name = name;
					}

					break;

#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eMeshAnimationKeyFrame:
#pragma GCC diagnostic pop
#pragma warning( pop )
					keyFrame = castor::makeUnique< MeshMorphTarget >( obj, 0_ms );
					result = createBinaryParser< MeshMorphTarget >().parse( *keyFrame, chunk );
					checkError( result, cuT( "Couldn't parse keyframe." ) );

					if ( result )
					{
						obj.addKeyFrame( castor::ptrRefCast< AnimationKeyFrame >( keyFrame ) );
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

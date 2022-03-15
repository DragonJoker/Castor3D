#include "Castor3D/Binary/BinaryMeshAnimationKeyFrame.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"

namespace castor3d
{
	//*************************************************************************************************

	namespace v1_3
	{
		template< typename T >
		using OldInterleavedVertexTArray = std::vector< OldInterleavedVertexT< T > >;

		template< typename T >
		void doConvert( OldInterleavedVertexTArray< T > const & in
			, InterleavedVertexArray & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto & inVtx : in )
			{
				auto & outVtx = *it;
				outVtx.pos[0] = float( inVtx.pos[0] );
				outVtx.pos[1] = float( inVtx.pos[1] );
				outVtx.pos[2] = float( inVtx.pos[2] );
				outVtx.nml[0] = float( inVtx.nml[0] );
				outVtx.nml[1] = float( inVtx.nml[1] );
				outVtx.nml[2] = float( inVtx.nml[2] );
				outVtx.tan[0] = float( inVtx.tan[0] );
				outVtx.tan[1] = float( inVtx.tan[1] );
				outVtx.tan[2] = float( inVtx.tan[2] );
				outVtx.tex[0] = float( inVtx.tex[0] );
				outVtx.tex[1] = float( inVtx.tex[1] );
				outVtx.tex[2] = float( inVtx.tex[2] );
				++it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< MeshAnimationKeyFrame >::doWrite( MeshAnimationKeyFrame const & obj )
	{
		auto result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eMeshAnimationKeyFrameTime, m_chunk );

		for ( auto & it : obj )
		{
			if ( result )
			{
				result = doWriteChunk( uint32_t( it.first ), ChunkType::eMeshAnimationKeyFrameSubmeshID, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( uint32_t( it.second.buffer.size() ), ChunkType::eMeshAnimationKeyFrameBufferSize, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.second.buffer, ChunkType::eMeshAnimationKeyFrameBufferData, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< MeshAnimationKeyFrame >::Name = cuT( "MeshAnimationKeyFrame" );

	bool BinaryParser< MeshAnimationKeyFrame >::doParse( MeshAnimationKeyFrame & obj )
	{
		bool result = true;
		castor::Matrix4x4f transform;
		InterleavedVertexArray buffer;
		BinaryChunk chunk;
		uint32_t id{ 0 };
		uint32_t count{ 0 };
		double time{ 0.0 };
		SubmeshSPtr submesh;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMeshAnimationKeyFrameTime:
				result = doParseChunk( time, chunk );
				checkError( result, "Couldn't parse time index." );
				obj.doSetTimeIndex( castor::Milliseconds{ int64_t( time * 1000 ) } );
				break;

			case ChunkType::eMeshAnimationKeyFrameSubmeshID:
				result = doParseChunk( id, chunk );
				checkError( result, "Couldn't parse submesh ID." );

				if ( result )
				{
					submesh = static_cast< Mesh const & >( *obj.getOwner()->getAnimable() ).getSubmesh( id );
				}

				break;

			case ChunkType::eMeshAnimationKeyFrameBufferSize:
				result = doParseChunk( count, chunk );
				checkError( result, "Couldn't parse buffer size." );

				if ( result )
				{
					buffer.resize( count );
				}

				break;

			case ChunkType::eMeshAnimationKeyFrameBufferData:
				if ( m_fileVersion > Version{ 1, 3, 0 } )
				{
					result = doParseChunk( buffer, chunk );
					checkError( result, "Couldn't parse buffer data." );

					if ( result )
					{
						obj.addSubmeshBuffer( *submesh, buffer );
					}
				}
				break;

			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< MeshAnimationKeyFrame >::doParse_v1_3( MeshAnimationKeyFrame & obj )
	{
		bool result = true;
		v1_3::OldInterleavedVertexTArray< double > bufferd;
		InterleavedVertexArray buffer;
		BinaryChunk chunk;
		SubmeshSPtr submesh;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMeshAnimationKeyFrameBufferData:
				if ( m_fileVersion <= Version{ 1, 3, 0 } )
				{
					result = doParseChunk( bufferd, chunk );
					checkError( result, "Couldn't parse buffer data." );

					if ( result )
					{
						v1_3::doConvert( bufferd, buffer );
						obj.addSubmeshBuffer( *submesh, buffer );
					}
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

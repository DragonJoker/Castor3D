#include "Castor3D/Binary/BinaryMeshAnimationKeyFrame.hpp"

#include "Castor3D/Animation/Mesh/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Animation/Mesh/MeshAnimation.hpp"
#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Mesh/Mesh.hpp"

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		using InterleavedVertexArrayd = InterleavedVertexTArray< double >;
		template< typename T >
		using OldInterleavedVertexTArray = std::vector< OldInterleavedVertexT< T > >;

		template< typename T, typename U >
		void doConvert( OldInterleavedVertexTArray< T > const & in
			, InterleavedVertexTArray< U > & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto & inVtx : in )
			{
				auto & outVtx = *it;
				outVtx.pos[0] = U( inVtx.pos[0] );
				outVtx.pos[1] = U( inVtx.pos[1] );
				outVtx.pos[2] = U( inVtx.pos[2] );
				outVtx.nml[0] = U( inVtx.nml[0] );
				outVtx.nml[1] = U( inVtx.nml[1] );
				outVtx.nml[2] = U( inVtx.nml[2] );
				outVtx.tan[0] = U( inVtx.tan[0] );
				outVtx.tan[1] = U( inVtx.tan[1] );
				outVtx.tan[2] = U( inVtx.tan[2] );
				outVtx.tex[0] = U( inVtx.tex[0] );
				outVtx.tex[1] = U( inVtx.tex[1] );
				outVtx.tex[2] = U( inVtx.tex[2] );
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
				result = doWriteChunk( uint32_t( it.second.m_buffer.size() ), ChunkType::eMeshAnimationKeyFrameBufferSize, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.second.m_buffer, ChunkType::eMeshAnimationKeyFrameBufferData, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

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
				obj.doSetTimeIndex( castor::Milliseconds{ int64_t( time * 1000 ) } );
				break;

			case ChunkType::eMeshAnimationKeyFrameSubmeshID:
				result = doParseChunk( id, chunk );

				if ( result )
				{
					submesh = static_cast< Mesh const & >( *obj.getOwner()->getOwner() ).getSubmesh( id );
				}

				break;

			case ChunkType::eMeshAnimationKeyFrameBufferSize:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					buffer.resize( count );
				}

				break;

			case ChunkType::eMeshAnimationKeyFrameBufferData:
				if ( m_fileVersion > Version{ 1, 3, 0 } )
				{
					result = doParseChunk( buffer, chunk );

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
		OldInterleavedVertexTArray< double > bufferd;
		InterleavedVertexArray buffer;
		BinaryChunk chunk;
		SubmeshSPtr submesh;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMeshAnimationKeyFrameBufferData:
				if ( m_fileVersion > Version{ 1, 3, 0 } )
				{
					result = doParseChunk( bufferd, chunk );

					if ( result )
					{
						doConvert( bufferd, buffer );
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

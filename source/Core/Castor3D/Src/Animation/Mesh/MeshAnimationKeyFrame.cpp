#include "MeshAnimationKeyFrame.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Submesh.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		using InterleavedVertexArrayd = InterleavedVertexTArray< double >;

		template< typename T, typename U >
		void doConvert( InterleavedVertexTArray< T > const & in
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
				outVtx.bin[0] = U( inVtx.bin[0] );
				outVtx.bin[1] = U( inVtx.bin[1] );
				outVtx.bin[2] = U( inVtx.bin[2] );
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

		BoundingBox doComputeBoundingBox( InterleavedVertexArray const & points )
		{
			Point3r min{ points[0].pos };
			Point3r max{ points[0].pos };

			for ( auto & vertex : points )
			{
				Point3r cur{ vertex.pos };
				max[0] = std::max( cur[0], max[0] );
				max[1] = std::max( cur[1], max[1] );
				max[2] = std::max( cur[2], max[2] );
				min[0] = std::min( cur[0], min[0] );
				min[1] = std::min( cur[1], min[1] );
				min[2] = std::min( cur[2], min[2] );
			}

			return BoundingBox{ min, max };
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< MeshAnimationKeyFrame >::doWrite( MeshAnimationKeyFrame const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eMeshAnimationKeyFrameTime, m_chunk );
		}

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
				InterleavedVertexArrayd buffer;
				doConvert( it.second.m_buffer, buffer );
				result = doWriteChunk( buffer, ChunkType::eMeshAnimationKeyFrameBufferData, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimationKeyFrame >::doParse( MeshAnimationKeyFrame & obj )
	{
		bool result = true;
		Matrix4x4r transform;
		InterleavedVertexArrayd bufferd;
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
				obj.doSetTimeIndex( Milliseconds{ int64_t( time * 1000 ) } );
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
				result = doParseChunk( buffer, chunk );

				if ( result )
				{
					doConvert( bufferd, buffer );
					obj.addSubmeshBuffer( *submesh, buffer );
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	MeshAnimationKeyFrame::MeshAnimationKeyFrame( MeshAnimation & parent
		, Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< MeshAnimation >{ parent }
	{
	}

	void MeshAnimationKeyFrame::addSubmeshBuffer( Submesh const & submesh
		, InterleavedVertexArray const & buffer )
	{
		auto boundingBox = doComputeBoundingBox( buffer );
		m_submeshesBuffers.emplace( submesh.getId()
			, SubmeshAnimationBuffer
			{
				buffer,
				boundingBox
			} );

		if ( m_submeshesBuffers.size() == 1u )
		{
			m_boundingBox = boundingBox;
		}
		else
		{
			m_boundingBox = m_boundingBox.getUnion( boundingBox );
		}
	}

	//*************************************************************************************************
}

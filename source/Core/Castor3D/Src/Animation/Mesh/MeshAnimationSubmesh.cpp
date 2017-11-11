#include "MeshAnimationSubmesh.hpp"

#include "MeshAnimation.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/MorphComponent.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		using InterleavedVertexd = InterleavedVertexT< double >;
		using SubmeshAnimationBufferd = SubmeshAnimationBufferT< double >;

		template< typename T, typename U >
		void doConvert( std::vector< InterleavedVertexT< T > > const & in
			, std::vector< InterleavedVertexT< U > > & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto & inVtx : in )
			{
				auto & outVtx = *it;
				outVtx.m_pos[0] = U( inVtx.m_pos[0] );
				outVtx.m_pos[1] = U( inVtx.m_pos[1] );
				outVtx.m_pos[2] = U( inVtx.m_pos[2] );
				outVtx.m_bin[0] = U( inVtx.m_bin[0] );
				outVtx.m_bin[1] = U( inVtx.m_bin[1] );
				outVtx.m_bin[2] = U( inVtx.m_bin[2] );
				outVtx.m_nml[0] = U( inVtx.m_nml[0] );
				outVtx.m_nml[1] = U( inVtx.m_nml[1] );
				outVtx.m_nml[2] = U( inVtx.m_nml[2] );
				outVtx.m_tan[0] = U( inVtx.m_tan[0] );
				outVtx.m_tan[1] = U( inVtx.m_tan[1] );
				outVtx.m_tan[2] = U( inVtx.m_tan[2] );
				outVtx.m_tex[0] = U( inVtx.m_tex[0] );
				outVtx.m_tex[1] = U( inVtx.m_tex[1] );
				outVtx.m_tex[2] = U( inVtx.m_tex[2] );
				++it;
			}
		}

		void doConvert( std::vector< SubmeshAnimationBufferd > const & in
			, std::vector< SubmeshAnimationBuffer > & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto const & inAnim : in )
			{
				it->m_timeIndex = float( inAnim.m_timeIndex );
				doConvert( inAnim.m_buffer, it->m_buffer );
				++it;
			}
		}

		void doConvert( std::vector< SubmeshAnimationBuffer > const & in
			, std::vector< SubmeshAnimationBufferd > & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto const & inAnim : in )
			{
				it->m_timeIndex = float( inAnim.m_timeIndex );
				doConvert( inAnim.m_buffer, it->m_buffer );
				++it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< MeshAnimationSubmesh >::doWrite( MeshAnimationSubmesh const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( real( obj.m_length.count() ) / 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( !obj.m_buffers.empty() )
		{
			if ( result )
			{
				result = doWriteChunk( uint32_t( obj.m_buffers.begin()->m_buffer.size() ), ChunkType::eSubmeshAnimationBufferSize, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( uint32_t( obj.m_buffers.size() ), ChunkType::eSubmeshAnimationBuffersCount, m_chunk );
			}

			if ( result )
			{
				std::vector< SubmeshAnimationBufferd > buffers;
				doConvert( obj.m_buffers, buffers );
				result = doWriteChunk( buffers, ChunkType::eSubmeshAnimationBuffers, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimationSubmesh >::doParse( MeshAnimationSubmesh & obj )
	{
		bool result = true;
		Matrix4x4r transform;
		std::vector< SubmeshAnimationBufferd > buffers;
		BinaryChunk chunk;
		uint32_t size{ 0 };
		uint32_t count{ 0 };
		real length{ 0.0_r };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmeshAnimationBufferSize:
				result = doParseChunk( size, chunk );

				if ( result )
				{
					buffers.resize( size );
				}

				break;

			case ChunkType::eSubmeshAnimationBuffersCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					for ( auto & buffer : buffers )
					{
						buffer.m_buffer.resize( count );
					}
				}

				break;

			case ChunkType::eSubmeshAnimationBuffers:
				result = doParseChunk( buffers, chunk );

				if ( result )
				{
					doConvert( buffers, obj.m_buffers );
				}

				break;

			case ChunkType::eAnimLength:
				result = doParseChunk( length, chunk );
				obj.m_length = Milliseconds{ int64_t( length * 1000 ) };
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	MeshAnimationSubmesh::MeshAnimationSubmesh( MeshAnimation & animation, Submesh & submesh )
		: OwnedBy< MeshAnimation >{ animation }
		, m_submesh{ submesh }
		, m_component{ std::make_shared< MorphComponent >( submesh ) }
	{
		m_submesh.addComponent( MorphComponent::Name, m_component );
	}

	MeshAnimationSubmesh::~MeshAnimationSubmesh()
	{
	}

	bool MeshAnimationSubmesh::addBuffer( Milliseconds const & from
		, InterleavedVertexArray && buffer )
	{
		auto it = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [&from]( SubmeshAnimationBuffer const & keyframe )
			{
				return Milliseconds{ int64_t( keyframe.m_timeIndex ) } == from;
			} );

		bool result = false;

		if ( it == m_buffers.end() )
		{
			m_length = from;
			m_buffers.insert( m_buffers.end(), SubmeshAnimationBuffer{ float( from.count() ), std::move( buffer ) } );
			result = true;
		}

		return result;
	}

	//*************************************************************************************************
}

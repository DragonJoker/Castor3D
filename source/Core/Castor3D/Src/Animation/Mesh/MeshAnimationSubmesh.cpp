#include "MeshAnimationSubmesh.hpp"

#include "MeshAnimation.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		using InterleavedVertexd = InterleavedVertexT< double >;
		using SubmeshAnimationBufferd = SubmeshAnimationBufferT< double >;

		template< typename T, typename U >
		void doConvert( std::vector< InterleavedVertexT< T > > const & p_in, std::vector< InterleavedVertexT< U > > & p_out )
		{
			p_out.resize( p_in.size() );
			auto it = p_out.begin();

			for ( auto & in : p_in )
			{
				auto & out = *it;
				out.m_pos[0] = U( in.m_pos[0] );
				out.m_pos[1] = U( in.m_pos[1] );
				out.m_pos[2] = U( in.m_pos[2] );
				out.m_bin[0] = U( in.m_bin[0] );
				out.m_bin[1] = U( in.m_bin[1] );
				out.m_bin[2] = U( in.m_bin[2] );
				out.m_nml[0] = U( in.m_nml[0] );
				out.m_nml[1] = U( in.m_nml[1] );
				out.m_nml[2] = U( in.m_nml[2] );
				out.m_tan[0] = U( in.m_tan[0] );
				out.m_tan[1] = U( in.m_tan[1] );
				out.m_tan[2] = U( in.m_tan[2] );
				out.m_tex[0] = U( in.m_tex[0] );
				out.m_tex[1] = U( in.m_tex[1] );
				out.m_tex[2] = U( in.m_tex[2] );
				++it;
			}
		}

		void doConvert( std::vector< SubmeshAnimationBufferd > const & p_in, std::vector< SubmeshAnimationBuffer > & p_out )
		{
			p_out.resize( p_in.size() );
			auto it = p_out.begin();

			for ( auto const & in : p_in )
			{
				it->m_timeIndex = float( in.m_timeIndex );
				doConvert( in.m_buffer, it->m_buffer );
				++it;
			}
		}

		void doConvert( std::vector< SubmeshAnimationBuffer > const & p_in, std::vector< SubmeshAnimationBufferd > & p_out )
		{
			p_out.resize( p_in.size() );
			auto it = p_out.begin();

			for ( auto const & in : p_in )
			{
				it->m_timeIndex = float( in.m_timeIndex );
				doConvert( in.m_buffer, it->m_buffer );
				++it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< MeshAnimationSubmesh >::doWrite( MeshAnimationSubmesh const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( real( p_obj.m_length.count() ) / 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( !p_obj.m_buffers.empty() )
		{
			if ( result )
			{
				result = doWriteChunk( uint32_t( p_obj.m_buffers.begin()->m_buffer.size() ), ChunkType::eSubmeshAnimationBufferSize, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( uint32_t( p_obj.m_buffers.size() ), ChunkType::eSubmeshAnimationBuffersCount, m_chunk );
			}

			if ( result )
			{
				std::vector< SubmeshAnimationBufferd > buffers;
				doConvert( p_obj.m_buffers, buffers );
				result = doWriteChunk( buffers, ChunkType::eSubmeshAnimationBuffers, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimationSubmesh >::doParse( MeshAnimationSubmesh & p_obj )
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
					doConvert( buffers, p_obj.m_buffers );
				}

				break;

			case ChunkType::eAnimLength:
				result = doParseChunk( length, chunk );
				p_obj.m_length = Milliseconds{ int64_t( length * 1000 ) };
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	MeshAnimationSubmesh::MeshAnimationSubmesh( MeshAnimation & p_animation, Submesh & p_submesh )
		: OwnedBy< MeshAnimation >{ p_animation }
		, m_submesh{ p_submesh }
	{
	}

	MeshAnimationSubmesh::~MeshAnimationSubmesh()
	{
	}

	bool MeshAnimationSubmesh::addBuffer( Milliseconds const & p_from
		, InterleavedVertexArray && p_buffer )
	{
		auto it = std::find_if( m_buffers.begin(), m_buffers.end(), [&p_from]( SubmeshAnimationBuffer const & p_keyframe )
		{
			return Milliseconds{ int64_t( p_keyframe.m_timeIndex ) } == p_from;
		} );

		bool result = false;

		if ( it == m_buffers.end() )
		{
			m_length = p_from;
			m_buffers.insert( m_buffers.end(), SubmeshAnimationBuffer{ float( p_from.count() ), std::move( p_buffer ) } );
			result = true;
		}

		return result;
	}

	//*************************************************************************************************
}

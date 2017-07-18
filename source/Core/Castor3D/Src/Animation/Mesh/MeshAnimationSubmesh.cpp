#include "MeshAnimationSubmesh.hpp"

#include "MeshAnimation.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		using InterleavedVertexd = InterleavedVertexT< double >;
		using SubmeshAnimationBufferd = SubmeshAnimationBufferT< double >;

		template< typename T, typename U >
		void DoConvert( std::vector< InterleavedVertexT< T > > const & p_in, std::vector< InterleavedVertexT< U > > & p_out )
		{
			p_out.resize( p_in.size() );
			auto l_it = p_out.begin();

			for ( auto & l_in : p_in )
			{
				auto & l_out = *l_it;
				l_out.m_pos[0] = U( l_in.m_pos[0] );
				l_out.m_pos[1] = U( l_in.m_pos[1] );
				l_out.m_pos[2] = U( l_in.m_pos[2] );
				l_out.m_bin[0] = U( l_in.m_bin[0] );
				l_out.m_bin[1] = U( l_in.m_bin[1] );
				l_out.m_bin[2] = U( l_in.m_bin[2] );
				l_out.m_nml[0] = U( l_in.m_nml[0] );
				l_out.m_nml[1] = U( l_in.m_nml[1] );
				l_out.m_nml[2] = U( l_in.m_nml[2] );
				l_out.m_tan[0] = U( l_in.m_tan[0] );
				l_out.m_tan[1] = U( l_in.m_tan[1] );
				l_out.m_tan[2] = U( l_in.m_tan[2] );
				l_out.m_tex[0] = U( l_in.m_tex[0] );
				l_out.m_tex[1] = U( l_in.m_tex[1] );
				l_out.m_tex[2] = U( l_in.m_tex[2] );
				++l_it;
			}
		}

		void DoConvert( std::vector< SubmeshAnimationBufferd > const & p_in, std::vector< SubmeshAnimationBuffer > & p_out )
		{
			p_out.resize( p_in.size() );
			auto l_it = p_out.begin();

			for ( auto const & l_in : p_in )
			{
				l_it->m_timeIndex = float( l_in.m_timeIndex );
				DoConvert( l_in.m_buffer, l_it->m_buffer );
				++l_it;
			}
		}

		void DoConvert( std::vector< SubmeshAnimationBuffer > const & p_in, std::vector< SubmeshAnimationBufferd > & p_out )
		{
			p_out.resize( p_in.size() );
			auto l_it = p_out.begin();

			for ( auto const & l_in : p_in )
			{
				l_it->m_timeIndex = float( l_in.m_timeIndex );
				DoConvert( l_in.m_buffer, l_it->m_buffer );
				++l_it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< MeshAnimationSubmesh >::DoWrite( MeshAnimationSubmesh const & p_obj )
	{
		bool l_result = true;

		if ( l_result )
		{
			l_result = DoWriteChunk( real( p_obj.m_length.count() ) / 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( !p_obj.m_buffers.empty() )
		{
			if ( l_result )
			{
				l_result = DoWriteChunk( uint32_t( p_obj.m_buffers.begin()->m_buffer.size() ), ChunkType::eSubmeshAnimationBufferSize, m_chunk );
			}

			if ( l_result )
			{
				l_result = DoWriteChunk( uint32_t( p_obj.m_buffers.size() ), ChunkType::eSubmeshAnimationBuffersCount, m_chunk );
			}

			if ( l_result )
			{
				std::vector< SubmeshAnimationBufferd > l_buffers;
				DoConvert( p_obj.m_buffers, l_buffers );
				l_result = DoWriteChunk( l_buffers, ChunkType::eSubmeshAnimationBuffers, m_chunk );
			}
		}

		return l_result;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimationSubmesh >::DoParse( MeshAnimationSubmesh & p_obj )
	{
		bool l_result = true;
		Matrix4x4r l_transform;
		std::vector< SubmeshAnimationBufferd > l_buffers;
		BinaryChunk l_chunk;
		uint32_t l_size{ 0 };
		uint32_t l_count{ 0 };
		real l_length{ 0.0_r };

		while ( l_result && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eSubmeshAnimationBufferSize:
				l_result = DoParseChunk( l_size, l_chunk );

				if ( l_result )
				{
					l_buffers.resize( l_size );
				}

				break;

			case ChunkType::eSubmeshAnimationBuffersCount:
				l_result = DoParseChunk( l_count, l_chunk );

				if ( l_result )
				{
					for ( auto & l_buffer : l_buffers )
					{
						l_buffer.m_buffer.resize( l_count );
					}
				}

				break;

			case ChunkType::eSubmeshAnimationBuffers:
				l_result = DoParseChunk( l_buffers, l_chunk );

				if ( l_result )
				{
					DoConvert( l_buffers, p_obj.m_buffers );
				}

				break;

			case ChunkType::eAnimLength:
				l_result = DoParseChunk( l_length, l_chunk );
				p_obj.m_length = std::chrono::milliseconds{ int64_t( l_length * 1000 ) };
				break;
			}
		}

		return l_result;
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

	bool MeshAnimationSubmesh::AddBuffer( std::chrono::milliseconds const & p_from
		, InterleavedVertexArray && p_buffer )
	{
		auto l_it = std::find_if( m_buffers.begin(), m_buffers.end(), [&p_from]( SubmeshAnimationBuffer const & p_keyframe )
		{
			return std::chrono::milliseconds{ int64_t( p_keyframe.m_timeIndex ) } == p_from;
		} );

		bool l_result = false;

		if ( l_it == m_buffers.end() )
		{
			m_length = p_from;
			m_buffers.insert( m_buffers.end(), SubmeshAnimationBuffer{ float( p_from.count() ), std::move( p_buffer ) } );
			l_result = true;
		}

		return l_result;
	}

	//*************************************************************************************************
}

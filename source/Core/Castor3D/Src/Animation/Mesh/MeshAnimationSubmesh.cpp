#include "MeshAnimationSubmesh.hpp"

#include "MeshAnimation.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		using InterleavedVertexd = InterleavedVertexT< double >;
		struct SubmeshAnimationBufferd
		{
			double m_timeIndex;
			std::vector< InterleavedVertexd > m_buffer;
		};

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
				l_it->m_timeIndex = real( l_in.m_timeIndex );
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
				l_it->m_timeIndex = double( l_in.m_timeIndex );
				DoConvert( l_in.m_buffer, l_it->m_buffer );
				++l_it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< MeshAnimationSubmesh >::DoWrite( MeshAnimationSubmesh const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.m_length, eCHUNK_TYPE_ANIM_LENGTH, m_chunk );
		}

		if ( !p_obj.m_buffers.empty() )
		{
			if ( l_return )
			{
				l_return = DoWriteChunk( uint32_t( p_obj.m_buffers.begin()->m_buffer.size() ), eCHUNK_TYPE_SUBMESH_ANIMATION_BUFFER_SIZE, m_chunk );
			}

			if ( l_return )
			{
				l_return = DoWriteChunk( uint32_t( p_obj.m_buffers.size() ), eCHUNK_TYPE_SUBMESH_ANIMATION_BUFFERS_COUNT, m_chunk );
			}

			if ( l_return )
			{
				std::vector< SubmeshAnimationBufferd > l_buffers;
				DoConvert( p_obj.m_buffers, l_buffers );
				l_return = DoWriteChunk( l_buffers, eCHUNK_TYPE_SUBMESH_ANIMATION_BUFFERS, m_chunk );
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimationSubmesh >::DoParse( MeshAnimationSubmesh & p_obj )
	{
		bool l_return = true;
		Matrix4x4r l_transform;
		std::vector< SubmeshAnimationBufferd > l_buffers;
		BinaryChunk l_chunk;
		uint32_t l_size{ 0 };
		uint32_t l_count{ 0 };

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_SUBMESH_ANIMATION_BUFFER_SIZE:
				l_return = DoParseChunk( l_size, l_chunk );

				if ( l_return )
				{
					l_buffers.resize( l_size );
				}

				break;

			case eCHUNK_TYPE_SUBMESH_ANIMATION_BUFFERS_COUNT:
				l_return = DoParseChunk( l_count, l_chunk );

				if ( l_return )
				{
					for ( auto & l_buffer : l_buffers )
					{
						l_buffer.m_buffer.resize( l_count );
					}
				}

				break;

			case eCHUNK_TYPE_SUBMESH_ANIMATION_BUFFERS:
				l_return = DoParseChunk( l_buffers, l_chunk );

				if ( l_return )
				{
					DoConvert( l_buffers, p_obj.m_buffers );
				}

				break;

			case eCHUNK_TYPE_ANIM_LENGTH:
				l_return = DoParseChunk( p_obj.m_length, l_chunk );
				break;
			}
		}

		return l_return;
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

	bool MeshAnimationSubmesh::AddBuffer( real p_from, InterleavedVertexArray && p_buffer )
	{
		auto l_it = std::find_if( m_buffers.begin(), m_buffers.end(), [&p_from]( SubmeshAnimationBuffer const & p_keyframe )
		{
			return p_keyframe.m_timeIndex == p_from;
		} );

		bool l_return = false;

		if ( l_it == m_buffers.end() )
		{
			m_length = p_from;
			m_buffers.insert( m_buffers.end(), SubmeshAnimationBuffer{ p_from, std::move( p_buffer ) } );
			l_return = true;
		}

		return l_return;
	}

	//*************************************************************************************************
}

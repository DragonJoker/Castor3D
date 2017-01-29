#include "GlES3UniformBufferBinding.hpp"

#include "Common/OpenGlES3.hpp"
#include "Shader/GlES3ShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	namespace
	{
		UIntArray DoListIndices( OpenGlES3 & p_gl, uint32_t p_program, UniformBuffer const & p_ubo )
		{
			std::vector< const char * > l_names;

			for ( auto l_variable : p_ubo )
			{
				char * l_szChar = new char[l_variable->GetName().size() + 1];
				l_szChar[l_variable->GetName().size()] = 0;
#if defined( _MSC_VER )
				strncpy_s( l_szChar, l_variable->GetName().size() + 1, string::string_cast< char >( l_variable->GetName() ).c_str(), l_variable->GetName().size() );
#else
				strncpy( l_szChar, string::string_cast< char >( l_variable->GetName() ).c_str(), l_variable->GetName().size() );
#endif
				l_names.push_back( l_szChar );
			}

			UIntArray l_return( l_names.size() );
			p_gl.GetUniformIndices( p_program, int( l_names.size() ), l_names.data(), l_return.data() );

			for ( auto l_name : l_names )
			{
				delete [] l_name;
			}

			return l_return;
		}

		void DoAdjustOffset( int p_align, uint32_t & p_offset )
		{
			if ( p_offset % p_align )
			{
				p_offset += p_align - ( p_offset % p_align );
			}
		}

		void DoRetrieveLayout( OpenGlES3 & p_gl, uint32_t p_program, UniformBuffer const & p_ubo, UIntArray const & p_indices, UniformBufferBinding::UniformInfoArray & p_infos )
		{
			uint32_t i = 0u;

			for ( auto l_variable : p_ubo )
			{
				uint32_t l_index = p_indices[i++];

				if ( l_index != GlES3InvalidIndex )
				{
					int l_offset = 0;
					p_gl.GetActiveUniformsiv( p_program, 1, &l_index, GlES3UniformValue::eOffset, &l_offset );
					int l_stride = 0;
					p_gl.GetActiveUniformsiv( p_program, 1, &l_index, GlES3UniformValue::eArrayStride, &l_stride );
					p_infos.push_back( { uint32_t( l_offset ), uint32_t( l_stride ) } );
				}
			}
		}
	}

	GlES3UniformBufferBinding::GlES3UniformBufferBinding( OpenGlES3 & p_gl
		, UniformBuffer & p_ubo
		, GlES3ShaderProgram const & p_program )
		: UniformBufferBinding{ p_ubo, p_program }
		, Holder{ p_gl }
		, m_blockIndex{ int( GlES3InvalidIndex ) }
	{
		m_blockIndex = GetOpenGlES3().GetUniformBlockIndex( p_program.GetGlES3Name(), string::string_cast< char >( GetOwner()->GetName() ).c_str() );

		if ( m_blockIndex != int( GlES3InvalidIndex ) )
		{
			auto l_indices = DoListIndices( GetOpenGlES3(), p_program.GetGlES3Name(), *GetOwner() );
			
			int l_size{ 0 };
			GetOpenGlES3().GetActiveUniformBlockiv( p_program.GetGlES3Name(), m_blockIndex, GlES3UniformBlockValue::eDataSize, &l_size );
			m_size = uint32_t( l_size );
			DoRetrieveLayout( GetOpenGlES3(), p_program.GetGlES3Name(), *GetOwner(), l_indices, m_variables );
		}
	}

	GlES3UniformBufferBinding::~GlES3UniformBufferBinding()
	{
	}

	void GlES3UniformBufferBinding::DoBind( uint32_t p_index )const
	{
		GetOpenGlES3().UniformBlockBinding( static_cast< GlES3ShaderProgram const & >( m_program ).GetGlES3Name(), m_blockIndex, p_index );
	}
}

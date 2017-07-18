#include "GlUniformBufferBinding.hpp"

#include "Common/OpenGl.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	namespace
	{
		UIntArray DoListIndices( OpenGl & p_gl, uint32_t p_program, UniformBuffer const & p_ubo )
		{
			std::vector< const char * > l_names;

			for ( auto l_variable : p_ubo )
			{
				char * l_szChar = new char[l_variable->GetName().size() + 1];
				l_szChar[l_variable->GetName().size()] = 0;
#if defined( CASTOR_COMPILER_MSVC )
				strncpy_s( l_szChar, l_variable->GetName().size() + 1, string::string_cast< char >( l_variable->GetName() ).c_str(), l_variable->GetName().size() );
#else
				strncpy( l_szChar, string::string_cast< char >( l_variable->GetName() ).c_str(), l_variable->GetName().size() );
#endif
				l_names.push_back( l_szChar );
			}

			UIntArray l_result( l_names.size() );
			p_gl.GetUniformIndices( p_program, int( l_names.size() ), l_names.data(), l_result.data() );

			for ( auto l_name : l_names )
			{
				delete [] l_name;
			}

			return l_result;
		}

		void DoAdjustOffset( int p_align, uint32_t & p_offset )
		{
			if ( p_offset % p_align )
			{
				p_offset += p_align - ( p_offset % p_align );
			}
		}

		void DoRetrieveLayout( OpenGl & p_gl, uint32_t p_program, UniformBuffer const & p_ubo, UIntArray const & p_indices, UniformBufferBinding::UniformInfoArray & p_infos )
		{
			uint32_t i = 0u;

			for ( auto l_variable : p_ubo )
			{
				uint32_t l_index = p_indices[i++];

				if ( l_index != GlInvalidIndex )
				{
					int l_offset = 0;
					p_gl.GetActiveUniformsiv( p_program, 1, &l_index, GlUniformValue::eOffset, &l_offset );
					int l_stride = 0;
					p_gl.GetActiveUniformsiv( p_program, 1, &l_index, GlUniformValue::eArrayStride, &l_stride );
					p_infos.push_back( { l_variable->GetName(), uint32_t( l_offset ), uint32_t( l_stride ) } );
				}
			}
		}
	}

	GlUniformBufferBinding::GlUniformBufferBinding( OpenGl & p_gl
		, UniformBuffer & p_ubo
		, GlShaderProgram const & p_program )
		: UniformBufferBinding{ p_ubo, p_program }
		, Holder{ p_gl }
		, m_blockIndex{ int( GlInvalidIndex ) }
	{
		m_blockIndex = GetOpenGl().GetUniformBlockIndex( p_program.GetGlName(), string::string_cast< char >( GetOwner()->GetName() ).c_str() );

		if ( m_blockIndex != int( GlInvalidIndex ) )
		{
			auto l_indices = DoListIndices( GetOpenGl(), p_program.GetGlName(), *GetOwner() );
			
			int l_size{ 0 };
			GetOpenGl().GetActiveUniformBlockiv( p_program.GetGlName(), m_blockIndex, GlUniformBlockValue::eDataSize, &l_size );
			m_size = uint32_t( l_size );
			DoRetrieveLayout( GetOpenGl(), p_program.GetGlName(), *GetOwner(), l_indices, m_variables );
		}
	}

	GlUniformBufferBinding::~GlUniformBufferBinding()
	{
	}

	void GlUniformBufferBinding::DoBind( uint32_t p_index )const
	{
		GetOpenGl().UniformBlockBinding( static_cast< GlShaderProgram const & >( m_program ).GetGlName(), m_blockIndex, p_index );
	}
}

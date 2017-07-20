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
			std::vector< const char * > names;

			for ( auto variable : p_ubo )
			{
				char * szChar = new char[variable->GetName().size() + 1];
				szChar[variable->GetName().size()] = 0;
#if defined( CASTOR_COMPILER_MSVC )
				strncpy_s( szChar, variable->GetName().size() + 1, string::string_cast< char >( variable->GetName() ).c_str(), variable->GetName().size() );
#else
				strncpy( szChar, string::string_cast< char >( variable->GetName() ).c_str(), variable->GetName().size() );
#endif
				names.push_back( szChar );
			}

			UIntArray result( names.size() );
			p_gl.GetUniformIndices( p_program, int( names.size() ), names.data(), result.data() );

			for ( auto name : names )
			{
				delete [] name;
			}

			return result;
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

			for ( auto variable : p_ubo )
			{
				uint32_t index = p_indices[i++];

				if ( index != GlInvalidIndex )
				{
					int offset = 0;
					p_gl.GetActiveUniformsiv( p_program, 1, &index, GlUniformValue::eOffset, &offset );
					int stride = 0;
					p_gl.GetActiveUniformsiv( p_program, 1, &index, GlUniformValue::eArrayStride, &stride );
					p_infos.push_back( { variable->GetName(), uint32_t( offset ), uint32_t( stride ) } );
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
			auto indices = DoListIndices( GetOpenGl(), p_program.GetGlName(), *GetOwner() );
			
			int size{ 0 };
			GetOpenGl().GetActiveUniformBlockiv( p_program.GetGlName(), m_blockIndex, GlUniformBlockValue::eDataSize, &size );
			m_size = uint32_t( size );
			DoRetrieveLayout( GetOpenGl(), p_program.GetGlName(), *GetOwner(), indices, m_variables );
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

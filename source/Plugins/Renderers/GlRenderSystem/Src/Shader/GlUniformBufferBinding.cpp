#include "GlUniformBufferBinding.hpp"

#include "Common/OpenGl.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	namespace
	{
		UIntArray doListIndices( OpenGl & p_gl, uint32_t p_program, UniformBuffer const & p_ubo )
		{
			std::vector< const char * > names;

			for ( auto variable : p_ubo )
			{
				char * szChar = new char[variable->getName().size() + 1];
				szChar[variable->getName().size()] = 0;
#if defined( CASTOR_COMPILER_MSVC )
				strncpy_s( szChar, variable->getName().size() + 1, string::stringCast< char >( variable->getName() ).c_str(), variable->getName().size() );
#else
				strncpy( szChar, string::stringCast< char >( variable->getName() ).c_str(), variable->getName().size() );
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

		void doAdjustOffset( int p_align, uint32_t & p_offset )
		{
			if ( p_offset % p_align )
			{
				p_offset += p_align - ( p_offset % p_align );
			}
		}

		void doRetrieveLayout( OpenGl & p_gl, uint32_t p_program, UniformBuffer const & p_ubo, UIntArray const & p_indices, UniformBufferBinding::UniformInfoArray & p_infos )
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
					p_infos.push_back( { variable->getName(), uint32_t( offset ), uint32_t( stride ) } );
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
		m_blockIndex = getOpenGl().GetUniformBlockIndex( p_program.getGlName(), string::stringCast< char >( getOwner()->getName() ).c_str() );

		if ( m_blockIndex != int( GlInvalidIndex ) )
		{
			auto indices = doListIndices( getOpenGl(), p_program.getGlName(), *getOwner() );
			
			int size{ 0 };
			getOpenGl().GetActiveUniformBlockiv( p_program.getGlName(), m_blockIndex, GlUniformBlockValue::eDataSize, &size );
			m_size = uint32_t( size );
			doRetrieveLayout( getOpenGl(), p_program.getGlName(), *getOwner(), indices, m_variables );
		}
	}

	GlUniformBufferBinding::~GlUniformBufferBinding()
	{
	}

	void GlUniformBufferBinding::doBind( uint32_t p_index )const
	{
		getOpenGl().UniformBlockBinding( static_cast< GlShaderProgram const & >( m_program ).getGlName(), m_blockIndex, p_index );
	}
}

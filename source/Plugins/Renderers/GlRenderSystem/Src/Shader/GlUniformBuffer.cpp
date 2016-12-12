#include "Shader/GlUniformBuffer.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlUniformBuffer::GlUniformBuffer( OpenGl & p_gl
		, String const & p_name
		, GlShaderProgram & p_program
		, ShaderTypeFlags const & p_flags
		, RenderSystem & p_renderSystem )
		: UniformBuffer( p_name, p_program, p_flags, p_renderSystem )
		, Holder( p_gl )
		, m_glBuffer( p_gl, GlBufferTarget::eUniform )
		, m_uniformBlockIndex( int( GlInvalidIndex ) )
		, m_uniformBlockSize( 0 )
	{
	}

	GlUniformBuffer::~GlUniformBuffer()
	{
	}

	void GlUniformBuffer::SetBindingUniform( uint32_t p_point )const
	{
		m_glBuffer.SetBindingPoint( p_point );
	}

	bool GlUniformBuffer::DoInitialise()
	{
		uint32_t l_index = m_glBuffer.GetGlName();
		bool l_return = false;
		GlShaderProgram & l_program = static_cast< GlShaderProgram & >( m_program );
		int l_max = 0;
		GetOpenGl().GetIntegerv( GlMax::eUniformBufferBindings, &l_max );
		GetOpenGl().UseProgram( l_program.GetGlName() );

		if ( int( m_index ) < l_max )
		{
			if ( GetOpenGl().HasUbo() && l_index == GlInvalidIndex )
			{
				m_uniformBlockIndex = GetOpenGl().GetUniformBlockIndex( l_program.GetGlName(), string::string_cast< char >( m_name ).c_str() );

				if ( m_uniformBlockIndex != int( GlInvalidIndex ) )
				{
					GetOpenGl().GetActiveUniformBlockiv( l_program.GetGlName(), m_uniformBlockIndex, GlUniformBlockValue::eDataSize, &m_uniformBlockSize );
					m_glBuffer.Create();
					m_glBuffer.InitialiseStorage( m_uniformBlockSize, BufferAccessType::eDynamic, BufferAccessNature::eDraw );
					m_glBuffer.Bind();
					GetOpenGl().BindBufferBase( GlBufferTarget::eUniform, m_index, m_glBuffer.GetGlName() );
					GetOpenGl().UniformBlockBinding( l_program.GetGlName(), m_uniformBlockIndex, m_index );
					m_buffer.resize( m_uniformBlockSize );
					std::vector< const char * > l_names;

					for ( auto l_variable : m_listVariables )
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

					UIntArray l_indices( l_names.size() );
					GetOpenGl().GetUniformIndices( l_program.GetGlName(), int( l_names.size() ), l_names.data(), l_indices.data() );

					for ( auto l_name : l_names )
					{
						delete [] l_name;
					}

					int i = 0;

					for ( auto l_variable : m_listVariables )
					{
						uint32_t l_index = l_indices[i++];
						int l_offset = 0;
						int l_stride = 0;

						if ( l_index != GlInvalidIndex )
						{
							GetOpenGl().GetActiveUniformsiv( l_program.GetGlName(), 1, &l_index, GlUniformValue::eOffset, &l_offset );
							GetOpenGl().GetActiveUniformsiv( l_program.GetGlName(), 1, &l_index, GlUniformValue::eArrayStride, &l_stride );
							l_variable->link( &m_buffer[l_offset], l_stride < 0 ? 0u : uint32_t( l_stride ) );
						}
					}

					m_glBuffer.Unbind();
					m_glBuffer.Upload( 0u, m_uniformBlockSize, m_buffer.data() );

					for ( auto & l_variable : m_listVariables )
					{
						l_variable->SetChanged( false );
					}

					l_return = true;
				}
			}
			else
			{
				l_return = false;
			}
		}
		else
		{
			DoCleanup();
			l_return = false;
		}

		return l_return;
	}

	void GlUniformBuffer::DoCleanup()
	{
		m_uniformBlockIndex = int( GlInvalidIndex );
		m_uniformBlockSize = 0;
		m_glBuffer.Destroy();
	}

	void GlUniformBuffer::DoBindTo( uint32_t p_index )
	{
		if ( m_uniformBlockIndex != int( GlInvalidIndex ) )
		{
			GlShaderProgram & l_program = static_cast< GlShaderProgram & >( m_program );
			GetOpenGl().UseProgram( l_program.GetGlName() );
			m_glBuffer.Bind();
			GetOpenGl().BindBufferBase( GlBufferTarget::eUniform, p_index, m_glBuffer.GetGlName() );
			GetOpenGl().UniformBlockBinding( l_program.GetGlName(), m_uniformBlockIndex, m_index );
			m_glBuffer.Unbind();
			GetOpenGl().UseProgram( 0 );
		}
	}

	void GlUniformBuffer::DoUpdate()
	{
		if ( m_uniformBlockIndex != int( GlInvalidIndex ) )
		{
			bool l_changed = m_listVariables.end() != std::find_if( m_listVariables.begin(), m_listVariables.end(), []( UniformSPtr p_variable )
			{
				return p_variable->IsChanged();
			} );

			if ( l_changed )
			{
				m_glBuffer.Upload( 0u, m_uniformBlockSize, m_buffer.data() );

				for ( auto & l_variable : m_listVariables )
				{
					l_variable->SetChanged( false );
				}
			}
		}
	}
}

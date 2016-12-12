#include "Shader/GlUniformBuffer.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlOneUniform.hpp"
#include "Shader/GlPointUniform.hpp"
#include "Shader/GlMatrixUniform.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	namespace
	{
		template< UniformType Type > UniformSPtr GlUniformCreator( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences );
		template<> UniformSPtr GlUniformCreator< UniformType::eInt >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneUniform< int > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eUInt >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneUniform< uint32_t > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eFloat >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneUniform< float > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eDouble >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneUniform< double > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eSampler >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneUniform< int > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec2i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< int, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec3i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< int, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec4i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< int, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec2ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< uint32_t, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec3ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< uint32_t, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec4ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< uint32_t, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< float, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< float, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< float, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< double, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< double, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eVec4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointUniform< double, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat2x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 2, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat2x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 2, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat2x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 2, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat3x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 3, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat3x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 3, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat3x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 3, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat4x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 4, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat4x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 4, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat4x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< float, 4, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat2x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 2, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat2x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 2, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat2x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 2, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat3x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 3, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat3x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 3, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat3x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 3, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat4x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 4, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat4x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 4, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> UniformSPtr GlUniformCreator< UniformType::eMat4x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixUniform< double, 4, 4 > >( p_gl, p_occurences, p_program );
		}
	}

	GlUniformBuffer::GlUniformBuffer( OpenGl & p_gl
		, String const & p_name
		, GlShaderProgram & p_program
		, FlagCombination< ShaderTypeFlag > const & p_flags
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
			m_glBuffer.Bind();
			GetOpenGl().BindBufferBase( GlBufferTarget::eUniform, p_index, m_glBuffer.GetGlName() );
			//GetOpenGl().UniformBlockBinding( l_program.GetGlName(), m_uniformBlockIndex, m_index );
			m_glBuffer.Unbind();
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

#include "Context.hpp"

#include "Engine.hpp"
#include "ShaderCache.hpp"

#include "Pipeline.hpp"
#include "RenderWindow.hpp"
#include "RenderSystem.hpp"
#include "Viewport.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Miscellaneous/GpuQuery.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/MatrixFrameVariable.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/DepthStencilState.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	Context::Context( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_window{ nullptr }
		, m_initialised{ false }
		, m_bMultiSampling{ false }
		, m_viewport{ *GetRenderSystem()->GetEngine() }
		, m_bufferVertex{ 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, }
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::Position ), ElementType::Vec2 },
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::TexCoords ), ElementType::Vec2 }
			}
		}
	{
		uint32_t i = 0;

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex )[i++ * m_declaration.GetStride()] );
		}

		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( false );
			m_texturePipeline = p_renderSystem.CreatePipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{} );
		}
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( true );
			l_dsState.SetDepthMask( WritingMask::All );
			m_depthPipeline = p_renderSystem.CreatePipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{} );
		}
	}

	Context::~Context()
	{
		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	bool Context::Initialise( RenderWindow * p_window )
	{
		m_viewport.Initialise();
		m_window = p_window;
		m_timerQuery[0] = GetRenderSystem()->CreateQuery( eQUERY_TYPE_TIME_ELAPSED );
		m_timerQuery[1] = GetRenderSystem()->CreateQuery( eQUERY_TYPE_TIME_ELAPSED );
		m_bMultiSampling = p_window->IsMultisampling();
		bool l_return = DoInitialise();

		if ( l_return )
		{
			DoSetCurrent();
			m_timerQuery[0]->Create();
			m_timerQuery[1]->Create();
			m_timerQuery[1 - m_queryIndex]->Begin();
			m_timerQuery[1 - m_queryIndex]->End();
			
			ShaderProgramSPtr l_textureProgram = DoCreateProgram( false );
			m_renderTextureProgram = l_textureProgram;
			l_textureProgram->Initialise();
			m_vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size() * m_declaration.GetStride() ) );
			m_vertexBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
			m_vertexBuffer->Create();
			m_vertexBuffer->Initialise( BufferAccessType::Static, BufferAccessNature::Draw );
			m_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, *l_textureProgram );
			m_geometryBuffers->Initialise( m_vertexBuffer, nullptr, nullptr, nullptr, nullptr );
			
			ShaderProgramSPtr l_depthProgram = DoCreateProgram( false );
			m_renderDepthProgram = l_depthProgram;
			l_depthProgram->Initialise();
			m_vertexBufferDepth = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBufferDepth->Resize( uint32_t( m_arrayVertex.size() * m_declaration.GetStride() ) );
			m_vertexBufferDepth->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
			m_vertexBufferDepth->Create();
			m_vertexBufferDepth->Initialise( BufferAccessType::Static, BufferAccessNature::Draw );
			m_geometryBuffersDepth = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, *l_depthProgram );
			m_geometryBuffersDepth->Initialise( m_vertexBufferDepth, nullptr, nullptr, nullptr, nullptr );
			DoEndCurrent();
		}

		return l_return;
	}

	void Context::Cleanup()
	{
		m_initialised = false;
		DoSetCurrent();
		DoCleanup();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer->Destroy();
		m_vertexBuffer.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		ShaderProgramSPtr l_program = m_renderTextureProgram.lock();

		if ( l_program )
		{
			l_program->Cleanup();
		}

		m_timerQuery[0]->Destroy();
		m_timerQuery[1]->Destroy();
		DoEndCurrent();
		DoDestroy();
		m_geometryBuffers.reset();
		m_bMultiSampling = false;
		m_renderTextureProgram.reset();
		m_timerQuery[0].reset();
		m_timerQuery[1].reset();
		m_window = nullptr;
		m_viewport.Cleanup();
	}

	void Context::SetCurrent()
	{
		DoSetCurrent();
		GetRenderSystem()->SetCurrentContext( this );
		m_timerQuery[m_queryIndex]->Begin();
	}

	void Context::EndCurrent()
	{
		m_timerQuery[m_queryIndex]->End();
		m_queryIndex = 1 - m_queryIndex;
		uint64_t l_time = 0;
		m_timerQuery[m_queryIndex]->GetInfos( eQUERY_INFO_RESULT, l_time );
		GetRenderSystem()->IncGpuTime( std::chrono::nanoseconds( l_time ) );
		GetRenderSystem()->SetCurrentContext( nullptr );
		DoEndCurrent();
	}

	void Context::SwapBuffers()
	{
		DoSwapBuffers();
	}

	void Context::RenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture )
	{
		DoRenderTexture( p_size, p_texture, *m_texturePipeline, *m_geometryBuffers, *m_renderTextureProgram.lock() );
	}

	void Context::RenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, ShaderProgramSPtr p_program )
	{
		if ( p_program )
		{
			DoRenderTexture( p_size, p_texture, *m_texturePipeline, *m_geometryBuffers, *p_program );
		}
	}

	void Context::RenderDepth( Castor::Size const & p_size, TextureLayout const & p_texture )
	{
		DoRenderTexture( p_size, p_texture, *m_depthPipeline, *m_geometryBuffersDepth, *m_renderDepthProgram.lock() );
	}

	void Context::DoRenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, Pipeline & p_pipeline, GeometryBuffers const & p_geometryBuffers, ShaderProgram const & p_program )
	{
		m_viewport.Resize( p_size );
		m_viewport.Update();
		p_pipeline.SetProjectionMatrix( m_viewport.GetProjection() );
		
		if ( p_program.GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			p_pipeline.Apply();
			FrameVariableBufferSPtr l_matrices = p_program.FindFrameVariableBuffer( ShaderProgram::BufferMatrix );

			if ( l_matrices )
			{
				p_pipeline.ApplyProjection( *l_matrices );
			}

			p_program.Bind();

			if ( p_texture.Bind( 0 ) )
			{
				p_geometryBuffers.Draw( uint32_t( m_arrayVertex.size() ), 0 );
				p_texture.Unbind( 0 );
			}

			p_program.Unbind();
		}
	}

	ShaderProgramSPtr Context::DoCreateProgram( bool p_depth )
	{
		using namespace GLSL;

		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderProgramSPtr l_program = l_cache.GetNewProgram();
		m_mapDiffuse = l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::Pixel );
		m_mapDiffuse->SetValue( 0 );
		l_cache.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );

		String l_strVtxShader;
		{
			// Vertex shader
			auto l_writer = GetRenderSystem()->CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			auto position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			l_strVtxShader = l_writer.Finalise();
		}

		String l_strPxlShader;

		if ( p_depth )
		{
			auto l_writer = GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto gl_FragDepth = l_writer.GetBuiltin< Float >( cuT( "gl_FragDepth" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				gl_FragDepth = texture2D( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).x();
			} );
			l_strPxlShader = l_writer.Finalise();
		}
		else
		{
			auto l_writer = GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = vec4( texture2D( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
			} );
			l_strPxlShader = l_writer.Finalise();
		}

		eSHADER_MODEL l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		l_program->SetSource( ShaderType::Vertex, l_model, l_strVtxShader );
		l_program->SetSource( ShaderType::Pixel, l_model, l_strPxlShader );
		return l_program;
	}
}

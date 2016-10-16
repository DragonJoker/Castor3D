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
		m_timerQuery[0] = GetRenderSystem()->CreateQuery( QueryType::TimeElapsed );
		m_timerQuery[1] = GetRenderSystem()->CreateQuery( QueryType::TimeElapsed );
		m_bMultiSampling = p_window->IsMultisampling();
		bool l_return = DoInitialise();

		if ( l_return )
		{
			DoSetCurrent();
			m_timerQuery[0]->Create();
			m_timerQuery[1]->Create();
			m_timerQuery[1 - m_queryIndex]->Begin();
			m_timerQuery[1 - m_queryIndex]->End();
			
			ShaderProgramSPtr l_textureProgram = DoCreateProgram( false, false );
			l_textureProgram->Initialise();
			m_vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size() * m_declaration.GetStride() ) );
			m_vertexBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
			m_vertexBuffer->Create();
			m_vertexBuffer->Initialise( BufferAccessType::Static, BufferAccessNature::Draw );
			m_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, *l_textureProgram );
			m_geometryBuffers->Initialise( m_vertexBuffer, nullptr, nullptr, nullptr, nullptr );
			{
				DepthStencilState l_dsState;
				l_dsState.SetDepthTest( false );
				m_texturePipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_textureProgram, PipelineFlags{} );
			}
			ShaderProgramSPtr l_textureProgramArray = DoCreateProgram( false, true );
			l_textureProgramArray->Initialise();
			m_vertexBufferArray = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBufferArray->Resize( uint32_t( m_arrayVertex.size() * m_declaration.GetStride() ) );
			m_vertexBufferArray->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
			m_vertexBufferArray->Create();
			m_vertexBufferArray->Initialise( BufferAccessType::Static, BufferAccessNature::Draw );
			m_geometryBuffersArray = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, *l_textureProgramArray );
			m_geometryBuffersArray->Initialise( m_vertexBufferArray, nullptr, nullptr, nullptr, nullptr );
			{
				DepthStencilState l_dsState;
				l_dsState.SetDepthTest( false );
				m_textureArrayPipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_textureProgramArray, PipelineFlags{} );
			}
			ShaderProgramSPtr l_depthProgram = DoCreateProgram( true, false );
			l_depthProgram->Initialise();
			m_vertexBufferDepth = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBufferDepth->Resize( uint32_t( m_arrayVertex.size() * m_declaration.GetStride() ) );
			m_vertexBufferDepth->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
			m_vertexBufferDepth->Create();
			m_vertexBufferDepth->Initialise( BufferAccessType::Static, BufferAccessNature::Draw );
			m_geometryBuffersDepth = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, *l_depthProgram );
			m_geometryBuffersDepth->Initialise( m_vertexBufferDepth, nullptr, nullptr, nullptr, nullptr );
			{
				DepthStencilState l_dsState;
				l_dsState.SetDepthTest( true );
				l_dsState.SetDepthMask( WritingMask::All );
				m_depthPipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_depthProgram, PipelineFlags{} );
			}
			ShaderProgramSPtr l_depthProgramArray = DoCreateProgram( true, true );
			l_depthProgramArray->Initialise();
			m_vertexBufferDepthArray = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBufferDepthArray->Resize( uint32_t( m_arrayVertex.size() * m_declaration.GetStride() ) );
			m_vertexBufferDepthArray->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
			m_vertexBufferDepthArray->Create();
			m_vertexBufferDepthArray->Initialise( BufferAccessType::Static, BufferAccessNature::Draw );
			m_geometryBuffersDepthArray = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, *l_depthProgramArray );
			m_geometryBuffersDepthArray->Initialise( m_vertexBufferDepthArray, nullptr, nullptr, nullptr, nullptr );
			{
				DepthStencilState l_dsState;
				l_dsState.SetDepthTest( true );
				l_dsState.SetDepthMask( WritingMask::All );
				m_depthArrayPipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_depthProgramArray, PipelineFlags{} );
			}
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
		m_texturePipeline->Cleanup();
		m_texturePipeline.reset();

		m_vertexBufferDepth->Cleanup();
		m_vertexBufferDepth->Destroy();
		m_vertexBufferDepth.reset();
		m_geometryBuffersDepth->Cleanup();
		m_geometryBuffersDepth.reset();
		m_depthPipeline->Cleanup();
		m_depthPipeline.reset();

		m_vertexBufferArray->Cleanup();
		m_vertexBufferArray->Destroy();
		m_vertexBufferArray.reset();
		m_geometryBuffersArray->Cleanup();
		m_geometryBuffersArray.reset();
		m_textureArrayPipeline->Cleanup();
		m_textureArrayPipeline.reset();

		m_vertexBufferDepthArray->Cleanup();
		m_vertexBufferDepthArray->Destroy();
		m_vertexBufferDepthArray.reset();
		m_geometryBuffersDepthArray->Cleanup();
		m_geometryBuffersDepthArray.reset();
		m_depthArrayPipeline->Cleanup();
		m_depthArrayPipeline.reset();

		m_timerQuery[0]->Destroy();
		m_timerQuery[1]->Destroy();
		DoEndCurrent();
		DoDestroy();

		m_bMultiSampling = false;
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
		m_timerQuery[m_queryIndex]->GetInfos( QueryInfo::Result, l_time );
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
		DoRenderTexture( p_size, p_texture, *m_texturePipeline, *m_geometryBuffers );
	}

	void Context::RenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, uint32_t p_index )
	{
		DoRenderTexture( p_size, p_texture, p_index, *m_textureArrayPipeline, *m_geometryBuffersArray );
	}

	void Context::RenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, Pipeline & p_pipeline )
	{
		DoRenderTexture( p_size, p_texture, p_pipeline, *m_geometryBuffers );
	}

	void Context::RenderDepth( Castor::Size const & p_size, TextureLayout const & p_texture )
	{
		DoRenderTexture( p_size, p_texture, *m_depthPipeline, *m_geometryBuffersDepth );
	}

	void Context::RenderDepth( Castor::Size const & p_size, TextureLayout const & p_texture, uint32_t p_index )
	{
		DoRenderTexture( p_size, p_texture, p_index, *m_depthArrayPipeline, *m_geometryBuffersDepthArray );
	}

	void Context::DoRenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, Pipeline & p_pipeline, GeometryBuffers const & p_geometryBuffers )
	{
		m_viewport.Resize( p_size );
		m_viewport.Update();
		m_viewport.Apply();
		p_pipeline.SetProjectionMatrix( m_viewport.GetProjection() );

		p_pipeline.Apply();
		FrameVariableBufferSPtr l_matrices = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferMatrix );

		if ( l_matrices )
		{
			p_pipeline.ApplyProjection( *l_matrices );
		}

		p_pipeline.GetProgram().BindUbos();

		if ( p_texture.Bind( 0 ) )
		{
			p_geometryBuffers.Draw( uint32_t( m_arrayVertex.size() ), 0 );
			p_texture.Unbind( 0 );
		}

		p_pipeline.GetProgram().UnbindUbos();
	}

	void Context::DoRenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, uint32_t p_index, Pipeline & p_pipeline, GeometryBuffers const & p_geometryBuffers )
	{
		REQUIRE( p_texture.GetLayersCount() > p_index );
		m_viewport.Resize( p_size );
		m_viewport.Update();
		m_viewport.Apply();
		p_pipeline.SetProjectionMatrix( m_viewport.GetProjection() );

		OneFloatFrameVariableSPtr l_variable = p_pipeline.GetProgram().FindFrameVariable< OneFloatFrameVariable >( cuT( "c3d_fIndex" ), ShaderType::Pixel );

		if ( l_variable )
		{
			l_variable->SetValue( p_index / float( p_texture.GetLayersCount() ) );
		}

		p_pipeline.Apply();
		FrameVariableBufferSPtr l_matrices = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferMatrix );

		if ( l_matrices )
		{
			p_pipeline.ApplyProjection( *l_matrices );
		}

		p_pipeline.GetProgram().BindUbos();

		if ( p_texture.Bind( 0 ) )
		{
			p_geometryBuffers.Draw( uint32_t( m_arrayVertex.size() ), 0 );
			p_texture.Unbind( 0 );
		}

		p_pipeline.GetProgram().UnbindUbos();
	}

	ShaderProgramSPtr Context::DoCreateProgram( bool p_depth, bool p_array )
	{
		using namespace GLSL;
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

		if ( p_array )
		{
			if ( p_depth )
			{
				auto l_writer = GetRenderSystem()->CreateGlslWriter();

				// Shader inputs
				auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2DArray >( ShaderProgram::MapDiffuse );
				auto c3d_fIndex = l_writer.GetUniform< Float >( cuT( "c3d_fIndex" ) );
				auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

				// Shader outputs
				auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					auto l_depth = l_writer.GetLocale< Float >( cuT( "l_depth" ), texture( c3d_mapDiffuse, vec3( vtx_texture, c3d_fIndex ) ).x() );
					l_depth = Float( 1.0f ) - l_writer.Paren( Float( 1.0f ) - l_depth ) * 25.0f;
					plx_v4FragColor = vec4( l_depth, l_depth, l_depth, 1.0 );
				} );
				l_strPxlShader = l_writer.Finalise();
			}
			else
			{
				auto l_writer = GetRenderSystem()->CreateGlslWriter();

				// Shader inputs
				auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2DArray >( ShaderProgram::MapDiffuse );
				auto c3d_fIndex = l_writer.GetUniform< Float >( cuT( "c3d_fIndex" ) );
				auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

				// Shader outputs
				auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					plx_v4FragColor = vec4( texture( c3d_mapDiffuse, vec3( vtx_texture, c3d_fIndex ) ).xyz(), 1.0 );
				} );
				l_strPxlShader = l_writer.Finalise();
			}
		}
		else
		{
			if ( p_depth )
			{
				auto l_writer = GetRenderSystem()->CreateGlslWriter();

				// Shader inputs
				auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
				auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

				// Shader outputs
				auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					auto l_depth = l_writer.GetLocale< Float >( cuT( "l_depth" ), texture( c3d_mapDiffuse, vtx_texture.xy() ).x() );
					l_depth = Float( 1.0f ) - l_writer.Paren( Float( 1.0f ) - l_depth ) * 25.0f;
					plx_v4FragColor = vec4( l_depth, l_depth, l_depth, 1.0 );
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
					plx_v4FragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
				} );
				l_strPxlShader = l_writer.Finalise();
			}
		}

		ShaderModel l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderProgramSPtr l_program = l_cache.GetNewProgram();
		l_cache.CreateMatrixBuffer( *l_program, 0u, MASK_SHADER_TYPE_VERTEX );
		l_program->SetSource( ShaderType::Vertex, l_model, l_strVtxShader );
		l_program->SetSource( ShaderType::Pixel, l_model, l_strPxlShader );
		l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::Pixel );

		if ( p_array )
		{
			l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "c3d_fIndex" ), ShaderType::Pixel );
		}

		return l_program;
	}
}

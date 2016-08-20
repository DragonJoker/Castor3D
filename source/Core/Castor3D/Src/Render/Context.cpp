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
				BufferElementDeclaration{ ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS },
				BufferElementDeclaration{ ShaderProgram::Texture, eELEMENT_USAGE_TEXCOORDS, eELEMENT_TYPE_2FLOATS }
			}
		}
	{
		uint32_t i = 0;

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex )[i++ * m_declaration.GetStride()] );
		}

		auto l_rsState = p_renderSystem.CreateRasteriserState();
		auto l_bdState = p_renderSystem.CreateBlendState();
		auto l_msState = p_renderSystem.CreateMultisampleState();
		m_pipeline = p_renderSystem.CreatePipeline( std::move( l_rsState ), std::move( l_bdState ), std::move( l_msState ) );
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
		m_dsStateNoDepth = GetRenderSystem()->CreateDepthStencilState();
		m_dsStateNoDepth->SetDepthTest( false );
		m_dsStateNoDepth->SetDepthMask( eWRITING_MASK_ZERO );
		m_dsStateNoDepthWrite = GetRenderSystem()->CreateDepthStencilState();
		m_dsStateNoDepthWrite->SetDepthMask( eWRITING_MASK_ZERO );
		bool l_return = DoInitialise();

		if ( l_return )
		{
			ShaderProgramSPtr l_program = DoCreateProgram();
			m_renderTextureProgram = l_program;

			DoSetCurrent();
			m_timerQuery[0]->Create();
			m_timerQuery[1]->Create();
			m_timerQuery[1 - m_queryIndex]->Begin();
			m_timerQuery[1 - m_queryIndex]->End();
			l_program->Initialise();

			m_vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size() * m_declaration.GetStride() ) );
			m_vertexBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
			m_vertexBuffer->Create();
			m_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
			m_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program );
			m_geometryBuffers->Initialise( m_vertexBuffer, nullptr, nullptr, nullptr, nullptr );
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
		m_dsStateNoDepth.reset();
		m_dsStateNoDepthWrite.reset();
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
		DoRenderTexture( p_size, p_texture, m_geometryBuffers, *m_renderTextureProgram.lock() );
	}

	void Context::RenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, ShaderProgramSPtr p_program )
	{
		if ( p_program )
		{
			DoRenderTexture( p_size, p_texture, m_geometryBuffers, *p_program );
		}
	}

	void Context::DoRenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, GeometryBuffersSPtr p_geometryBuffers, ShaderProgram const & p_program )
	{
		m_viewport.Resize( p_size );
		m_viewport.Update();
		m_pipeline->SetProjectionMatrix( m_viewport.GetProjection() );
		
		if ( p_program.GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_pipeline->Apply();
			FrameVariableBufferSPtr l_matrices = p_program.FindFrameVariableBuffer( ShaderProgram::BufferMatrix );

			if ( l_matrices )
			{
				m_pipeline->ApplyProjection( *l_matrices );
			}

			p_program.Bind();

			if ( p_texture.Bind( 0 ) )
			{
				p_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
				p_texture.Unbind( 0 );
			}

			p_program.Unbind();
		}
	}

	ShaderProgramSPtr Context::DoCreateProgram()
	{
		using namespace GLSL;

		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderProgramSPtr l_program = l_cache.GetNewProgram();
		m_mapDiffuse = l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, eSHADER_TYPE_PIXEL );
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
				gl_Position = c3d_mtxProjection * vec4( position.SWIZZLE_X, position.SWIZZLE_Y, 0.0, 1.0 );
			} );
			l_strVtxShader = l_writer.Finalise();
		}

		String l_strPxlShader;
		{
			auto l_writer = GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = vec4( texture2D( c3d_mapDiffuse, vec2( vtx_texture.SWIZZLE_X, vtx_texture.SWIZZLE_Y ) ).SWIZZLE_XYZ, 1.0 );
			} );
			l_strPxlShader = l_writer.Finalise();
		}

		eSHADER_MODEL l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_strVtxShader );
		l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_strPxlShader );
		return l_program;
	}
}

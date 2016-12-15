#include "Context.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "RenderPipeline.hpp"
#include "RenderWindow.hpp"
#include "RenderSystem.hpp"
#include "Viewport.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Miscellaneous/GpuQuery.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/DepthStencilState.hpp"
#include "Texture/Sampler.hpp"
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
		, m_matrixUbo{ ShaderProgram::BufferMatrix, p_renderSystem }
		, m_rtotPipelinePlane
		{
			Viewport{ *GetRenderSystem()->GetEngine() },
			{
				0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1
			},
			BufferDeclaration{
				{
					BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 },
					BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 }
				}
			}
		}
		, m_rtotPipelineCube
		{
			Viewport{ *GetRenderSystem()->GetEngine() },
			{
				-1, 1, -1, 1, -1, -1, -1, -1, -1, 1, -1, -1, -1, 1, -1, 1, 1, -1,
				-1, -1, 1, -1, 1, -1, -1, -1, -1, -1, 1, -1, -1, -1, 1, -1, 1, 1,
				1, -1, -1, 1, 1, 1, 1, -1, 1, 1, 1, 1, 1, -1, -1, 1, 1, -1,
				-1, -1, 1, 1, 1, 1, -1, 1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1,
				-1, 1, -1, 1, 1, 1, 1, 1, -1, 1, 1, 1, -1, 1, -1, -1, 1, 1,
				-1, -1, -1, 1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1, 1, -1, -1, 1
			},
			BufferDeclaration{
				{
					BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 }
				}
			}
		}
	{
		UniformBuffer::FillMatrixBuffer( m_matrixUbo );
		uint32_t i = 0;

		for ( auto & l_vertex : m_rtotPipelinePlane.m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_rtotPipelinePlane.m_bufferVertex.data() )[i++ * m_rtotPipelinePlane.m_declaration.stride()] );
		}

		i = 0;

		for ( auto & l_vertex : m_rtotPipelineCube.m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_rtotPipelineCube.m_bufferVertex.data() )[i++ * m_rtotPipelineCube.m_declaration.stride()] );
		}
	}

	Context::~Context()
	{
		for ( auto & l_vertex : m_rtotPipelineCube.m_arrayVertex )
		{
			l_vertex.reset();
		}

		for ( auto & l_vertex : m_rtotPipelinePlane.m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	bool Context::Initialise( RenderWindow * p_window )
	{
		m_rtotPipelinePlane.m_viewport.Initialise();
		m_rtotPipelineCube.m_viewport.Initialise();
		m_window = p_window;
		m_timerQuery[0] = GetRenderSystem()->CreateQuery( QueryType::eTimeElapsed );
		m_timerQuery[1] = GetRenderSystem()->CreateQuery( QueryType::eTimeElapsed );
		m_bMultiSampling = p_window->IsMultisampling();
		bool l_return = DoInitialise();

		if ( l_return )
		{
			DoSetCurrent();
			m_timerQuery[0]->Initialise();
			m_timerQuery[1]->Initialise();
			m_timerQuery[1 - m_queryIndex]->Begin();
			m_timerQuery[1 - m_queryIndex]->End();

			DoInitialiseRTOTPipelinePlane( m_rtotPipelinePlane.m_texture, *DoCreateProgram2D( false, false ), false );
			DoInitialiseRTOTPipelinePlane( m_rtotPipelinePlane.m_textureArray, *DoCreateProgram2D( false, true ), false );
			DoInitialiseRTOTPipelinePlane( m_rtotPipelinePlane.m_depth, *DoCreateProgram2D( true, false ), true );
			DoInitialiseRTOTPipelinePlane( m_rtotPipelinePlane.m_depthArray, *DoCreateProgram2D( true, true ), true );

			DoInitialiseRTOTPipelineCube( m_rtotPipelineCube.m_texture, *DoCreateProgramCube( false, false ), false );
			DoInitialiseRTOTPipelineCube( m_rtotPipelineCube.m_textureArray, *DoCreateProgramCube( false, true ), false );
			DoInitialiseRTOTPipelineCube( m_rtotPipelineCube.m_depth, *DoCreateProgramCube( true, false ), true );
			DoInitialiseRTOTPipelineCube( m_rtotPipelineCube.m_depthArray, *DoCreateProgramCube( true, true ), true );

			auto l_sampler = GetRenderSystem()->GetEngine()->GetSamplerCache().Add( cuT( "ContextCube" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_rtotPipelineCube.m_sampler = l_sampler;

			l_sampler = GetRenderSystem()->GetEngine()->GetSamplerCache().Add( cuT( "ContextPlace" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_rtotPipelinePlane.m_sampler = l_sampler;

			DoEndCurrent();
		}

		return l_return;
	}

	void Context::Cleanup()
	{
		m_initialised = false;
		DoSetCurrent();
		DoCleanup();

		DoCleanupRTOTPipelinePlane( m_rtotPipelinePlane.m_texture );
		DoCleanupRTOTPipelinePlane( m_rtotPipelinePlane.m_textureArray );
		DoCleanupRTOTPipelinePlane( m_rtotPipelinePlane.m_depth );
		DoCleanupRTOTPipelinePlane( m_rtotPipelinePlane.m_depthArray );

		DoCleanupRTOTPipelineCube( m_rtotPipelineCube.m_texture );
		DoCleanupRTOTPipelineCube( m_rtotPipelineCube.m_textureArray );
		DoCleanupRTOTPipelineCube( m_rtotPipelineCube.m_depth );
		DoCleanupRTOTPipelineCube( m_rtotPipelineCube.m_depthArray );

		m_timerQuery[0]->Cleanup();
		m_timerQuery[1]->Cleanup();
		DoEndCurrent();
		DoDestroy();

		m_bMultiSampling = false;
		m_timerQuery[0].reset();
		m_timerQuery[1].reset();
		m_window = nullptr;
		m_rtotPipelinePlane.m_viewport.Cleanup();
		m_rtotPipelineCube.m_viewport.Cleanup();
		m_matrixUbo.Cleanup();
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
		m_timerQuery[m_queryIndex]->GetInfos( QueryInfo::eResult, l_time );
		GetRenderSystem()->IncGpuTime( std::chrono::nanoseconds( l_time ) );
		GetRenderSystem()->SetCurrentContext( nullptr );
		DoEndCurrent();
	}

	void Context::SwapBuffers()
	{
		DoSwapBuffers();
	}

	void Context::RenderTexture( Size const & p_size
		, TextureLayout const & p_texture
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo )
	{
		DoRenderTexture( p_size
			, p_texture
			, p_pipeline
			, p_matrixUbo
			, *m_rtotPipelinePlane.m_texture.m_geometryBuffers );
	}

	void Context::RenderTexture( Size const & p_size
		, TextureLayout const & p_texture )
	{
		DoRenderTexture( p_size
			, p_texture
			, *m_rtotPipelinePlane.m_texture.m_pipeline
			, m_matrixUbo
			, *m_rtotPipelinePlane.m_texture.m_geometryBuffers );
	}

	void Context::RenderTexture( Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		DoRenderTexture( p_size
			, p_texture
			, p_index
			, *m_rtotPipelinePlane.m_textureArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipelinePlane.m_textureArray.m_geometryBuffers );
	}

	void Context::RenderDepth( Size const & p_size
		, TextureLayout const & p_texture )
	{
		DoRenderTexture( p_size
			, p_texture
			, *m_rtotPipelinePlane.m_depth.m_pipeline
			, m_matrixUbo
			, *m_rtotPipelinePlane.m_depth.m_geometryBuffers );
	}

	void Context::RenderDepth( Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		DoRenderTexture( p_size
			, p_texture
			, p_index
			, *m_rtotPipelinePlane.m_depthArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipelinePlane.m_depthArray.m_geometryBuffers );
	}

	void Context::RenderTexture( Point3r const & p_position
		, Quaternion const & p_orientation
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		DoRenderTexture( p_position
			, p_orientation
			, p_size
			, p_texture
			, *m_rtotPipelineCube.m_texture.m_pipeline
			, m_matrixUbo
			, *m_rtotPipelineCube.m_texture.m_geometryBuffers );
	}

	void Context::RenderTexture( Point3r const & p_position
		, Quaternion const & p_orientation
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		DoRenderTexture( p_position
			, p_orientation
			, p_size
			, p_texture
			, p_index
			, *m_rtotPipelineCube.m_textureArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipelineCube.m_textureArray.m_geometryBuffers );
	}

	void Context::RenderDepth( Point3r const & p_position
		, Quaternion const & p_orientation
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		DoRenderTexture( p_position
			, p_orientation
			, p_size
			, p_texture
			, *m_rtotPipelineCube.m_depth.m_pipeline
			, m_matrixUbo
			, *m_rtotPipelineCube.m_depth.m_geometryBuffers );
	}

	void Context::RenderDepth( Point3r const & p_position
		, Quaternion const & p_orientation
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index )
	{
		DoRenderTexture( p_position
			, p_orientation
			, p_size
			, p_texture
			, p_index
			, *m_rtotPipelineCube.m_depthArray.m_pipeline
			, m_matrixUbo
			, *m_rtotPipelineCube.m_depthArray.m_geometryBuffers );
	}

	void Context::DoRenderTexture( Size const & p_size
		, TextureLayout const & p_texture
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers )
	{
		m_rtotPipelinePlane.m_viewport.Resize( p_size );
		m_rtotPipelinePlane.m_viewport.Update();
		m_rtotPipelinePlane.m_viewport.Apply();
		p_pipeline.SetProjectionMatrix( m_rtotPipelinePlane.m_viewport.GetProjection() );
		
		p_pipeline.ApplyProjection( p_matrixUbo );
		p_matrixUbo.Update();
		p_pipeline.Apply();

		p_texture.Bind( 0u );
		m_rtotPipelinePlane.m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_rtotPipelinePlane.m_arrayVertex.size() ), 0 );
		m_rtotPipelinePlane.m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

	void Context::DoRenderTexture( Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers )
	{
		REQUIRE( p_texture.GetLayersCount() > p_index );
		m_rtotPipelinePlane.m_viewport.Resize( p_size );
		m_rtotPipelinePlane.m_viewport.Update();
		m_rtotPipelinePlane.m_viewport.Apply();
		p_pipeline.SetProjectionMatrix( m_rtotPipelinePlane.m_viewport.GetProjection() );

		auto l_variable = p_pipeline.GetProgram().FindUniform< UniformType::eFloat >( cuT( "c3d_fIndex" ), ShaderType::ePixel );

		if ( l_variable )
		{
			l_variable->SetValue( p_index / float( p_texture.GetLayersCount() ) );
		}

		p_pipeline.ApplyProjection( p_matrixUbo );
		p_matrixUbo.Update();
		p_pipeline.Apply();

		p_texture.Bind( 0u );
		m_rtotPipelinePlane.m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_rtotPipelinePlane.m_arrayVertex.size() ), 0 );
		m_rtotPipelinePlane.m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

	void Context::DoRenderTexture( Point3r const & p_position
		, Quaternion const & p_orientation
		, Size const & p_size
		, TextureLayout const & p_texture
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers )
	{
		m_rtotPipelineCube.m_viewport.SetPerspective( Angle::from_degrees( 90 ), real( p_size.width() ) / p_size.height(), 0.5, 2.0 );
		m_rtotPipelineCube.m_viewport.Resize( p_size );
		m_rtotPipelineCube.m_viewport.Update();
		m_rtotPipelineCube.m_viewport.Apply();
		p_pipeline.SetProjectionMatrix( m_rtotPipelineCube.m_viewport.GetProjection() );

		Matrix4x4r l_mtx;
		matrix::set_translate( l_mtx, p_position );
		p_pipeline.SetModelMatrix( l_mtx );

		Point3r l_front{ 0, 0, 1 };
		Point3r l_up{ 0, 1, 0 };
		p_orientation.transform( l_front, l_front );
		p_orientation.transform( l_up, l_up );
		matrix::look_at( l_mtx, p_position, p_position + l_front, l_up );
		p_pipeline.SetViewMatrix( l_mtx );
		
		p_pipeline.ApplyMatrices( p_matrixUbo, ~0u );
		p_matrixUbo.Update();
		p_pipeline.Apply();

		p_texture.Bind( 0u );
		m_rtotPipelineCube.m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_rtotPipelineCube.m_arrayVertex.size() ), 0 );
		m_rtotPipelineCube.m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

	void Context::DoRenderTexture( Point3r const & p_position
		, Quaternion const & p_orientation
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_index
		, RenderPipeline & p_pipeline
		, UniformBuffer & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers )
	{
		REQUIRE( p_texture.GetLayersCount() > p_index );
		m_rtotPipelineCube.m_viewport.SetPerspective( Angle::from_degrees( 90 ), real( p_size.width() ) / p_size.height(), 0.5, 2.0 );
		m_rtotPipelineCube.m_viewport.Resize( p_size );
		m_rtotPipelineCube.m_viewport.Update();
		m_rtotPipelineCube.m_viewport.Apply();
		p_pipeline.SetProjectionMatrix( m_rtotPipelineCube.m_viewport.GetProjection() );

		Matrix4x4r l_mtx;
		matrix::set_translate( l_mtx, p_position );
		p_pipeline.SetModelMatrix( l_mtx );

		Point3r l_front{ 0, 0, 1 };
		Point3r l_up{ 0, 1, 0 };
		p_orientation.transform( l_front, l_front );
		p_orientation.transform( l_up, l_up );
		matrix::look_at( l_mtx, p_position, p_position + l_front, l_up );
		p_pipeline.SetViewMatrix( l_mtx );

		auto l_variable = p_pipeline.GetProgram().FindUniform< UniformType::eFloat >( cuT( "c3d_fIndex" ), ShaderType::ePixel );

		if ( l_variable )
		{
			l_variable->SetValue( p_index / float( p_texture.GetLayersCount() ) );
		}
		
		p_pipeline.ApplyMatrices( p_matrixUbo, ~0u );
		p_matrixUbo.Update();
		p_pipeline.Apply();

		p_texture.Bind( 0u );
		m_rtotPipelineCube.m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_rtotPipelineCube.m_arrayVertex.size() ), 0 );
		m_rtotPipelineCube.m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

	ShaderProgramSPtr Context::DoCreateProgram2D( bool p_depth, bool p_array )
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
					auto l_depth = l_writer.GetLocale( cuT( "l_depth" ), texture( c3d_mapDiffuse, vec3( vtx_texture, c3d_fIndex ) ).x() );
					l_depth = 1.0_f - l_writer.Paren( 1.0_f - l_depth ) * 25.0f;
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
					auto l_depth = l_writer.GetLocale( cuT( "l_depth" ), texture( c3d_mapDiffuse, vtx_texture.xy() ).x() );
					l_depth = 1.0_f - l_writer.Paren( 1.0_f - l_depth ) * 25.0f;
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

		auto l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		auto l_program = l_cache.GetNewProgram( false );
		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_model, l_strVtxShader );
		l_program->SetSource( ShaderType::ePixel, l_model, l_strPxlShader );
		l_program->CreateUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );

		if ( p_array )
		{
			l_program->CreateUniform< UniformType::eFloat >( cuT( "c3d_fIndex" ), ShaderType::ePixel );
		}

		l_program->Initialise();
		return l_program;
	}

	ShaderProgramSPtr Context::DoCreateProgramCube( bool p_depth, bool p_array )
	{
		using namespace GLSL;
		String l_vtx;
		{
			GlslWriter l_writer{ GetRenderSystem()->CreateGlslWriter() };

			// Inputs
			auto position = l_writer.GetAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( l_writer );
			UBO_MODEL_MATRIX( l_writer );

			// Outputs
			auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > l_main = [&]()
			{
				gl_Position = l_writer.Paren( c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
				vtx_texture = position;
			};

			l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
			l_vtx = l_writer.Finalise();
		}

		String l_pxl;

		if ( p_array )
		{
			if ( p_depth )
			{
				GlslWriter l_writer{ GetRenderSystem()->CreateGlslWriter() };

				// Inputs
				auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
				auto c3d_fIndex = l_writer.GetUniform< Float >( cuT( "c3d_fIndex" ) );
				auto c3d_mapDiffuse = l_writer.GetUniform< SamplerCubeArray >( ShaderProgram::MapDiffuse );

				// Outputs
				auto plx_v4FragColor = l_writer.GetOutput< Vec4 >( cuT( "pxl_FragColor" ) );

				std::function< void() > l_main = [&]()
				{
					auto l_depth = l_writer.GetLocale( cuT( "l_depth" ), texture( c3d_mapDiffuse, vec4( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z(), c3d_fIndex ) ).x() );
					l_depth = 1.0_f - l_writer.Paren( 1.0_f - l_depth ) * 25.0f;
					plx_v4FragColor = vec4( l_depth, l_depth, l_depth, 1.0 );
				};

				l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
				l_pxl = l_writer.Finalise();
			}
			else
			{
				GlslWriter l_writer{ GetRenderSystem()->CreateGlslWriter() };

				// Inputs
				auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
				auto c3d_fIndex = l_writer.GetUniform< Float >( cuT( "c3d_fIndex" ) );
				auto c3d_mapDiffuse = l_writer.GetUniform< SamplerCubeArray >( ShaderProgram::MapDiffuse );

				// Outputs
				auto plx_v4FragColor = l_writer.GetOutput< Vec4 >( cuT( "pxl_FragColor" ) );

				std::function< void() > l_main = [&]()
				{
					plx_v4FragColor = texture( c3d_mapDiffuse, vec4( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z(), c3d_fIndex ) );
				};

				l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
				l_pxl = l_writer.Finalise();
			}
		}
		else
		{
			if ( p_depth )
			{
				GlslWriter l_writer{ GetRenderSystem()->CreateGlslWriter() };

				// Inputs
				auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
				auto c3d_mapDiffuse = l_writer.GetUniform< SamplerCube >( ShaderProgram::MapDiffuse );

				// Outputs
				auto plx_v4FragColor = l_writer.GetOutput< Vec4 >( cuT( "pxl_FragColor" ) );

				std::function< void() > l_main = [&]()
				{
					auto l_depth = l_writer.GetLocale( cuT( "l_depth" ), texture( c3d_mapDiffuse, vec3( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z() ) ).x() );
					l_depth = 1.0_f - l_writer.Paren( 1.0_f - l_depth ) * 25.0f;
					plx_v4FragColor = vec4( l_depth, l_depth, l_depth, 1.0 );
				};

				l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
				l_pxl = l_writer.Finalise();
			}
			else
			{
				GlslWriter l_writer{ GetRenderSystem()->CreateGlslWriter() };

				// Inputs
				auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
				auto c3d_mapDiffuse = l_writer.GetUniform< SamplerCube >( ShaderProgram::MapDiffuse );

				// Outputs
				auto plx_v4FragColor = l_writer.GetOutput< Vec4 >( cuT( "pxl_FragColor" ) );

				std::function< void() > l_main = [&]()
				{
					plx_v4FragColor = texture( c3d_mapDiffuse, vec3( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z() ) );
				};

				l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
				l_pxl = l_writer.Finalise();
			}
		}

		auto l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		auto l_program = l_cache.GetNewProgram( false );
		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
		l_program->SetSource( ShaderType::ePixel, l_model, l_pxl );
		l_program->CreateUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );

		if ( p_array )
		{
			l_program->CreateUniform< UniformType::eFloat >( cuT( "c3d_fIndex" ), ShaderType::ePixel );
		}

		l_program->Initialise();
		return l_program;
	}

	void Context::DoInitialiseRTOTPipelinePlane( RTOTPipeline & p_pipeline, ShaderProgram & p_program, bool p_depth )
	{
		p_program.Initialise();
		p_pipeline.m_vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_rtotPipelinePlane.m_declaration );
		p_pipeline.m_vertexBuffer->Resize( uint32_t( m_rtotPipelinePlane.m_arrayVertex.size() * m_rtotPipelinePlane.m_declaration.stride() ) );
		p_pipeline.m_vertexBuffer->LinkCoords( m_rtotPipelinePlane.m_arrayVertex.begin(), m_rtotPipelinePlane.m_arrayVertex.end() );
		p_pipeline.m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		p_pipeline.m_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, p_program );
		p_pipeline.m_geometryBuffers->Initialise( { *p_pipeline.m_vertexBuffer }, nullptr );

		if ( p_depth )
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( true );
			l_dsState.SetDepthMask( WritingMask::eAll );
			p_pipeline.m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, p_program, PipelineFlags{} );
			p_pipeline.m_pipeline->AddUniformBuffer( m_matrixUbo );
		}
		else
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( false );
			p_pipeline.m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, p_program, PipelineFlags{} );
			p_pipeline.m_pipeline->AddUniformBuffer( m_matrixUbo );
		}
	}

	void Context::DoCleanupRTOTPipelinePlane( RTOTPipeline & p_pipeline )
	{
		p_pipeline.m_pipeline->Cleanup();
		p_pipeline.m_pipeline.reset();
		p_pipeline.m_vertexBuffer->Cleanup();
		p_pipeline.m_vertexBuffer.reset();
		p_pipeline.m_geometryBuffers->Cleanup();
		p_pipeline.m_geometryBuffers.reset();
	}

	void Context::DoInitialiseRTOTPipelineCube( RTOTPipeline & p_pipeline, ShaderProgram & p_program, bool p_depth )
	{
		p_program.Initialise();
		p_pipeline.m_vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_rtotPipelineCube.m_declaration );
		p_pipeline.m_vertexBuffer->Resize( uint32_t( m_rtotPipelineCube.m_arrayVertex.size() * m_rtotPipelineCube.m_declaration.stride() ) );
		p_pipeline.m_vertexBuffer->LinkCoords( m_rtotPipelineCube.m_arrayVertex.begin(), m_rtotPipelineCube.m_arrayVertex.end() );
		p_pipeline.m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		p_pipeline.m_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, p_program );
		p_pipeline.m_geometryBuffers->Initialise( { *p_pipeline.m_vertexBuffer }, nullptr );

		if ( p_depth )
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthFunc( DepthFunc::eLEqual );
			l_dsState.SetDepthTest( true );
			l_dsState.SetDepthMask( WritingMask::eAll );

			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eFront );

			p_pipeline.m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState ), std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, PipelineFlags{} );
			p_pipeline.m_pipeline->AddUniformBuffer( m_matrixUbo );
		}
		else
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( false );
			l_dsState.SetDepthMask( WritingMask::eAll );

			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eFront );

			p_pipeline.m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState ), std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, PipelineFlags{} );
			p_pipeline.m_pipeline->AddUniformBuffer( m_matrixUbo );
		}
	}

	void Context::DoCleanupRTOTPipelineCube( RTOTPipeline & p_pipeline )
	{
		p_pipeline.m_pipeline->Cleanup();
		p_pipeline.m_pipeline.reset();
		p_pipeline.m_vertexBuffer->Cleanup();
		p_pipeline.m_vertexBuffer.reset();
		p_pipeline.m_geometryBuffers->Cleanup();
		p_pipeline.m_geometryBuffers.reset();
	}
}

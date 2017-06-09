#include "FinalCombinePass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/SsaoConfig.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Fog.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

#include <random>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		VertexBufferSPtr DoCreateVbo( Engine & p_engine )
		{
			auto l_declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float l_data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_vertexBuffer = std::make_shared< VertexBuffer >( p_engine, l_declaration );
			uint32_t l_stride = l_declaration.stride();
			l_vertexBuffer->Resize( uint32_t( sizeof( l_data ) ) );
			uint8_t * l_buffer = l_vertexBuffer->data();
			std::memcpy( l_buffer, l_data, sizeof( l_data ) );
			l_vertexBuffer->Initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return l_vertexBuffer;
		}

		GeometryBuffersSPtr DoCreateVao( Engine & p_engine
			, ShaderProgram & p_program
			, VertexBuffer & p_vbo )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_result = l_renderSystem.CreateGeometryBuffers( Topology::eTriangles
				, p_program );
			l_result->Initialise( { p_vbo }, nullptr );
			return l_result;
		}

		String DoGetVertexProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			String l_vtx;
			{
				using namespace GLSL;
				auto l_writer = l_renderSystem.CreateGlslWriter();

				// Shader inputs
				UBO_MATRIX( l_writer );
				auto position = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
				auto texture = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

				// Shader outputs
				auto vtx_texture = l_writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
				auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

				l_writer.ImplementFunction< void >( cuT( "main" )
					, [&]()
					{
						vtx_texture = texture;
						gl_Position = c3d_mtxProjection * vec4( position, 0.0, 1.0 );
					} );
				l_vtx = l_writer.Finalise();
			}

			return l_vtx;
		}
		
		String DoGetPixelProgram( Engine & p_engine
			, GLSL::FogType p_fogType )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( l_writer );
			UBO_GPINFO( l_writer );
			auto c3d_mapDepth = l_writer.DeclUniform< Sampler2D >( GetTextureName( WbTexture::eDepth ) );
			auto c3d_mapAccumulation = l_writer.DeclUniform< Sampler2D >( GetTextureName( WbTexture::eAccumulation ) );
			auto c3d_mapRevealage = l_writer.DeclUniform< Sampler2D >( GetTextureName( WbTexture::eRevealage ) );
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_FragCoord = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = l_writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcVSPosition();

			GLSL::Fog l_fog{ p_fogType, l_writer };

			auto l_maxComponent = l_writer.ImplementFunction< Float >( cuT( "maxComponent" )
				, [&]( Vec3 const & v )
			{
				l_writer.Return( max( max( v.x(), v.y() ), v.z() ) );
			}, InVec3{ &l_writer, cuT( "v" ) } );

			l_writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
				auto l_coord = l_writer.DeclLocale( cuT( "l_coord" )
					, ivec2( gl_FragCoord.xy() ) );
					auto l_revealage = l_writer.DeclLocale( cuT( "l_revealage" )
						, texelFetch( c3d_mapRevealage, l_coord, 0 ).r() );

					IF( l_writer, l_revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						l_writer.Discard();
					}
					FI;

					auto l_accum = l_writer.DeclLocale( cuT( "l_accum" )
						, texelFetch( c3d_mapAccumulation, l_coord, 0 ) );

					// Suppress overflow
					IF( l_writer, GLSL::isinf( l_maxComponent( GLSL::abs( l_accum.rgb() ) ) ) )
					{
						l_accum.rgb() = vec3( l_accum.a() );
					}
					FI;

					auto l_averageColor = l_writer.DeclLocale( cuT( "l_averageColor" )
						, l_accum.rgb() / max( l_accum.a(), 0.00001_f ) );

					pxl_fragColor = vec4( l_averageColor.rgb(), 1.0_f - l_revealage );

					if ( p_fogType != FogType::eDisabled )
					{
						auto l_position = l_writer.DeclLocale( cuT( "l_position" ), l_utils.CalcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						l_fog.ApplyFog( pxl_fragColor, length( l_position ), l_position.z() );
					}
				} );
			return l_writer.Finalise();
		}
		
		ShaderProgramSPtr DoCreateProgram( Engine & p_engine
			, GLSL::FogType p_fogType )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			String l_vtx = DoGetVertexProgram( p_engine );
			String l_pxl = DoGetPixelProgram( p_engine, p_fogType );
			ShaderProgramSPtr l_program = p_engine.GetShaderProgramCache().GetNewProgram( false );
			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::ePixel );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( WbTexture::eDepth ), ShaderType::ePixel )->SetValue( 0u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( WbTexture::eAccumulation ), ShaderType::ePixel )->SetValue( 1u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( WbTexture::eRevealage ), ShaderType::ePixel )->SetValue( 2u );

			auto const l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::ePixel, l_model, l_pxl );
			l_program->Initialise();
			return l_program;
		}

		RenderPipelineUPtr DoCreateRenderPipeline( Engine & p_engine
			, ShaderProgram & p_program
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfoUbo )
		{
			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			RasteriserState l_rsstate;
			l_rsstate.SetCulledFaces( Culling::eNone );
			BlendState l_bdState;
			l_bdState.EnableBlend( true );
			l_bdState.SetAlphaBlend( BlendOperation::eAdd, BlendOperand::eSrcAlpha, BlendOperand::eInvSrcAlpha );
			l_bdState.SetRgbBlend( BlendOperation::eAdd, BlendOperand::eSrcAlpha, BlendOperand::eInvSrcAlpha );
			auto l_pipeline = p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate )
				, std::move( l_rsstate )
				, std::move( l_bdState )
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
			l_pipeline->AddUniformBuffer( p_matrixUbo.GetUbo() );
			l_pipeline->AddUniformBuffer( p_sceneUbo.GetUbo() );
			l_pipeline->AddUniformBuffer( p_gpInfoUbo.GetUbo() );
			return l_pipeline;
		}
	}

	//*********************************************************************************************

	FinalCombineProgram::FinalCombineProgram( Engine & p_engine
		, VertexBuffer & p_vbo
		, MatrixUbo & p_matrixUbo
		, SceneUbo & p_sceneUbo
		, GpInfoUbo & p_gpInfoUbo
		, GLSL::FogType p_fogType )
		: m_program{ DoCreateProgram( p_engine, p_fogType ) }
		, m_geometryBuffers{ DoCreateVao( p_engine, *m_program, p_vbo ) }
		, m_pipeline{ DoCreateRenderPipeline( p_engine, *m_program, p_matrixUbo, p_sceneUbo, p_gpInfoUbo ) }
	{
	}

	FinalCombineProgram::~FinalCombineProgram()
	{
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void FinalCombineProgram::Render()const
	{
		m_pipeline->Apply();
		m_geometryBuffers->Draw( 6u, 0 );
	}

	//*********************************************************************************************

	FinalCombinePass::FinalCombinePass( Engine & p_engine
		, Size const & p_size )
		: m_size{ p_size }
		, m_viewport{ p_engine }
		, m_vertexBuffer{ DoCreateVbo( p_engine ) }
		, m_matrixUbo{ p_engine }
		, m_sceneUbo{ p_engine }
		, m_gpInfo{ p_engine }
		, m_programs
		{
			FinalCombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eDisabled },
			FinalCombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eLinear },
			FinalCombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eExponential },
			FinalCombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eSquaredExponential }
		}
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		m_matrixUbo.Update( m_viewport.GetProjection() );
	}

	FinalCombinePass::~FinalCombinePass()
	{
		m_matrixUbo.GetUbo().Cleanup();
		m_sceneUbo.GetUbo().Cleanup();
		m_gpInfo.GetUbo().Cleanup();
		m_vertexBuffer->Cleanup();
	}

	void FinalCombinePass::Render( WeightedBlendPassResult const & p_r
		, FrameBuffer const & p_frameBuffer
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj
		, Fog const & p_fog )
	{
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		m_sceneUbo.Update( p_camera, p_fog );
		p_frameBuffer.Bind( FrameBufferTarget::eDraw );
		p_frameBuffer.SetDrawBuffers();

		auto & l_program = m_programs[size_t( p_fog.GetType() )];

		m_viewport.Apply();
		p_r[size_t( WbTexture::eDepth )]->GetTexture()->Bind( 0u );
		p_r[size_t( WbTexture::eDepth )]->GetSampler()->Bind( 0u );
		p_r[size_t( WbTexture::eAccumulation )]->GetTexture()->Bind( 1u );
		p_r[size_t( WbTexture::eAccumulation )]->GetSampler()->Bind( 1u );
		p_r[size_t( WbTexture::eRevealage )]->GetTexture()->Bind( 2u );
		p_r[size_t( WbTexture::eRevealage )]->GetSampler()->Bind( 2u );
		l_program.Render();
		p_r[size_t( WbTexture::eRevealage )]->GetTexture()->Unbind( 2u );
		p_r[size_t( WbTexture::eRevealage )]->GetSampler()->Unbind( 2u );
		p_r[size_t( WbTexture::eAccumulation )]->GetTexture()->Unbind( 1u );
		p_r[size_t( WbTexture::eAccumulation )]->GetSampler()->Unbind( 1u );
		p_r[size_t( WbTexture::eDepth )]->GetTexture()->Unbind( 0u );
		p_r[size_t( WbTexture::eDepth )]->GetSampler()->Unbind( 0u );
	}

	//*********************************************************************************************
}

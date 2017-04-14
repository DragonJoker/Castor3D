#include "FogPass.hpp"

#include "LightPass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
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

namespace deferred_common
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
				UBO_GPINFO( l_writer );
				auto position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );
				auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture );

				// Shader outputs
				auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
				auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

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
		
		String DoGetPixelProgram( Engine & p_engine, GLSL::FogType p_fogType )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( l_writer );
			UBO_GPINFO( l_writer );
			auto c3d_mapColour = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapColour" ) );
			auto c3d_mapDepth = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
			auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
			auto c3d_mapNormal = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormal ) );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcVSPosition();
			Declare_DecodeMaterial( l_writer );

			GLSL::Fog l_fog{ p_fogType, l_writer };

			// Shader outputs
			auto pxl_fragColor = l_writer.GetOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			l_writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto l_colour = l_writer.GetLocale( cuT( "l_colour" ), texture( c3d_mapColour, vtx_texture ).xyz() );
					auto l_diffuse = l_writer.GetLocale( cuT( "l_diffuse" ), texture( c3d_mapDiffuse, vtx_texture ).xyz() );
					auto l_ambient = l_writer.GetLocale( cuT( "l_ambient" ), c3d_v4AmbientLight.xyz() );
					auto l_emissive = l_writer.GetLocale( cuT( "l_emissive" ), texture( c3d_mapEmissive, vtx_texture ).xyz() );
					auto l_flags = l_writer.GetLocale( cuT( "l_flags" ), texture( c3d_mapNormal, vtx_texture ).w() );
					auto l_envMapIndex = l_writer.GetLocale( cuT( "l_envMapIndex" ), 0_i );
					auto l_receiver = l_writer.GetLocale( cuT( "l_receiver" ), 0_i );
					auto l_reflection = l_writer.GetLocale( cuT( "l_reflection" ), 0_i );
					auto l_refraction = l_writer.GetLocale( cuT( "l_refraction" ), 0_i );
					DecodeMaterial( l_writer
						, l_flags
						, l_receiver
						, l_reflection
						, l_refraction
						, l_envMapIndex );

					IF( l_writer, cuT( "l_envMapIndex < 1 || ( l_reflection == 0 && l_refraction == 0 )" ) )
					{
						//if ( m_ssao )
						//{
						//	l_ambient *= texture( c3d_mapSsao, l_texCoord ).r();
						//}
					}
					ELSE
					{
						l_ambient = vec3( 0.0_f );
					}
					FI;

					pxl_fragColor = vec4( l_colour + l_emissive + l_diffuse * l_ambient, 1.0 );

					if ( p_fogType != FogType::eDisabled )
					{
						auto l_position = l_writer.GetLocale( cuT( "l_position" ), l_utils.CalcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						l_fog.ApplyFog( pxl_fragColor, length( l_position ), l_position.z() );
					}
				} );
			return l_writer.Finalise();
		}
		
		ShaderProgramSPtr DoCreateProgram( Engine & p_engine, GLSL::FogType p_fogType )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			String l_vtx = DoGetVertexProgram( p_engine );
			String l_pxl = DoGetPixelProgram( p_engine, p_fogType );
			ShaderProgramSPtr l_program = p_engine.GetShaderProgramCache().GetNewProgram( false );
			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::ePixel );
			l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapColour" ), ShaderType::ePixel )->SetValue( 0u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eDepth ), ShaderType::ePixel )->SetValue( 1u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eDiffuse ), ShaderType::ePixel )->SetValue( 2u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eEmissive ), ShaderType::ePixel )->SetValue( 3u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eNormal ), ShaderType::ePixel )->SetValue( 4u );
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
			, GpInfo & p_gpInfo )
		{
			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			RasteriserState l_rsstate;
			l_rsstate.SetCulledFaces( Culling::eNone );
			BlendState l_blState;
			auto l_pipeline = p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate )
				, std::move( l_rsstate )
				, std::move( l_blState )
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
			l_pipeline->AddUniformBuffer( p_matrixUbo.GetUbo() );
			l_pipeline->AddUniformBuffer( p_sceneUbo.GetUbo() );
			l_pipeline->AddUniformBuffer( p_gpInfo.GetUbo() );
			return l_pipeline;
		}
	}

	//*********************************************************************************************

	FogProgram::FogProgram( Engine & p_engine
		, VertexBuffer & p_vbo
		, MatrixUbo & p_matrixUbo
		, SceneUbo & p_sceneUbo
		, GpInfo & p_gpInfo
		, GLSL::FogType p_fogType )
		: m_program{ DoCreateProgram( p_engine, p_fogType ) }
		, m_geometryBuffers{ DoCreateVao( p_engine, *m_program, p_vbo ) }
		, m_pipeline{ DoCreateRenderPipeline( p_engine, *m_program, p_matrixUbo, p_sceneUbo, p_gpInfo ) }
	{
	}

	FogProgram::~FogProgram()
	{
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void FogProgram::Render()const
	{
		m_pipeline->Apply();
		m_geometryBuffers->Draw( 6u, 0 );
	}

	//*********************************************************************************************

	FogPass::FogPass( Engine & p_engine
		, Size const & p_size )
		: m_size{ p_size }
		, m_viewport{ p_engine }
		, m_vertexBuffer{ DoCreateVbo( p_engine ) }
		, m_matrixUbo{ p_engine }
		, m_sceneUbo{ p_engine }
		, m_gpInfo{ p_engine }
		, m_programs
		{
			{
				FogProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eDisabled },
				FogProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eLinear },
				FogProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eExponential },
				FogProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eSquaredExponential }
			}
		}
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		m_matrixUbo.Update( m_viewport.GetProjection() );
	}

	FogPass::~FogPass()
	{
		m_sceneUbo.GetUbo().Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
		m_vertexBuffer->Cleanup();
	}

	void FogPass::Render( GeometryPassResult const & p_gp
		, TextureUnit const & p_lp
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj
		, Fog const & p_fog )
	{
		m_viewport.Apply();
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		m_sceneUbo.Update( p_camera, p_fog );
		p_lp.Bind();
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( 1u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( 1u );
		p_gp[size_t( DsTexture::eDiffuse )]->GetTexture()->Bind( 2u );
		p_gp[size_t( DsTexture::eDiffuse )]->GetSampler()->Bind( 2u );
		p_gp[size_t( DsTexture::eEmissive )]->GetTexture()->Bind( 3u );
		p_gp[size_t( DsTexture::eEmissive )]->GetSampler()->Bind( 3u );
		p_gp[size_t( DsTexture::eNormal )]->GetTexture()->Bind( 4u );
		p_gp[size_t( DsTexture::eNormal )]->GetSampler()->Bind( 4u );
		m_programs[size_t( p_fog.GetType() )].Render();
		p_gp[size_t( DsTexture::eNormal )]->GetTexture()->Unbind( 4u );
		p_gp[size_t( DsTexture::eNormal )]->GetSampler()->Unbind( 4u );
		p_gp[size_t( DsTexture::eEmissive )]->GetTexture()->Unbind( 3u );
		p_gp[size_t( DsTexture::eEmissive )]->GetSampler()->Unbind( 3u );
		p_gp[size_t( DsTexture::eDiffuse )]->GetTexture()->Unbind( 2u );
		p_gp[size_t( DsTexture::eDiffuse )]->GetSampler()->Unbind( 2u );
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( 1u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( 1u );
		p_lp.Unbind();
	}

	//*********************************************************************************************
}

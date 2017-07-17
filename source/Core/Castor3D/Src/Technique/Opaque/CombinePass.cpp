#include "CombinePass.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Miscellaneous/SsaoConfig.hpp"
#include "PBR/IblTextures.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Fog.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Technique/Opaque/LightPass.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

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
			uint8_t * l_buffer = l_vertexBuffer->GetData();
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

		GLSL::Shader DoGetVertexProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( l_writer );
			UBO_GPINFO( l_writer );
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
			return l_writer.Finalise();
		}
		
		GLSL::Shader DoGetLegacyPixelProgram( Engine & p_engine
			, GLSL::FogType p_fogType
			, bool p_ssao )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( l_writer );
			UBO_GPINFO( l_writer );
			auto c3d_mapReflection = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapReflection" ) );
			auto c3d_mapRefraction = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapRefraction" ) );
			auto c3d_mapDepth = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
			auto c3d_mapSsao = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapSsao" ), p_ssao );
			auto c3d_mapPostLight = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcVSPosition();
			l_utils.DeclareCalcWSPosition();

			Declare_DecodeMaterial( l_writer );

			GLSL::Fog l_fog{ p_fogType, l_writer };

			// Shader outputs
			auto pxl_fragColor = l_writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			l_writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto l_ambient = l_writer.DeclLocale( cuT( "l_ambient" )
						, c3d_v4AmbientLight.xyz() );
					auto l_postLight = l_writer.DeclLocale( cuT( "l_light" )
						, texture( c3d_mapPostLight, vtx_texture ).xyz() );
					auto l_data1 = l_writer.DeclLocale( cuT( "l_data1" )
						, texture( c3d_mapData1, vtx_texture ) );
					auto l_data4 = l_writer.DeclLocale( cuT( "l_data4" )
						, texture( c3d_mapData4, vtx_texture ) );
					auto l_emissive = l_writer.DeclLocale( cuT( "l_emissive" )
						, l_data4.xyz() );
					auto l_flags = l_writer.DeclLocale( cuT( "l_flags" )
						, l_data1.w() );
					auto l_envMapIndex = l_writer.DeclLocale( cuT( "l_envMapIndex" )
						, 0_i );
					auto l_receiver = l_writer.DeclLocale( cuT( "l_receiver" )
						, 0_i );
					auto l_reflection = l_writer.DeclLocale( cuT( "l_reflection" )
						, 0_i );
					auto l_refraction = l_writer.DeclLocale( cuT( "l_refraction" )
						, 0_i );
					DecodeMaterial( l_writer
						, l_flags
						, l_receiver
						, l_reflection
						, l_refraction
						, l_envMapIndex );
					auto l_data2 = l_writer.DeclLocale( cuT( "l_data2" )
						, texture( c3d_mapData2, vtx_texture ) );
					auto l_diffuse = l_writer.DeclLocale( cuT( "l_diffuse" )
						, l_data2.xyz() );

					IF( l_writer, "l_envMapIndex >= 1 && l_reflection != 0" )
					{
						auto l_reflect = l_writer.DeclLocale( cuT( "l_reflect" )
							, texture( c3d_mapReflection, vtx_texture ).xyz() );
						l_diffuse = l_reflect;
						l_ambient = vec3( 0.0_f );
					}
					ELSE
					{
						if ( p_ssao )
						{
							l_ambient *= texture( c3d_mapSsao, vtx_texture ).r();
						}

						l_ambient *= l_diffuse;
						l_diffuse = l_postLight;
					}
					FI;

					IF( l_writer, "l_envMapIndex >= 1 && l_refraction != 0" )
					{
						auto l_refract = l_writer.DeclLocale( cuT( "l_refract" )
							, texture( c3d_mapRefraction, vtx_texture ) );

						IF( l_writer, l_reflection != 0_i )
						{
							l_ambient = mix( l_refract.xyz(), l_diffuse, l_refract.w() );
							l_diffuse = vec3( 0.0_f );
						}
						ELSE
						{
							l_ambient = l_refract.xyz();
							l_diffuse = vec3( 0.0_f );
						}
						FI;
					}
					FI;

					pxl_fragColor = vec4( l_diffuse + l_emissive + l_ambient, 1.0 );

					if ( p_fogType != FogType::eDisabled )
					{
						auto l_position = l_writer.DeclLocale( cuT( "l_position" )
							, l_utils.CalcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						l_fog.ApplyFog( pxl_fragColor, length( l_position ), l_position.z() );
					}
				} );
			return l_writer.Finalise();
		}
		
		GLSL::Shader DoGetPbrPixelProgram( Engine & p_engine
			, GLSL::FogType p_fogType
			, bool p_ssao )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( l_writer );
			UBO_GPINFO( l_writer );
			auto c3d_mapReflection = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapReflection" ) );
			auto c3d_mapRefraction = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapRefraction" ) );
			auto c3d_mapDepth = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
			auto c3d_mapSsao = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapSsao" ), p_ssao );
			auto c3d_mapPostLight = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto c3d_mapIrradiance = l_writer.DeclUniform< SamplerCube >( ShaderProgram::MapIrradiance );
			auto c3d_mapPrefiltered = l_writer.DeclUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
			auto c3d_mapBrdf = l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapBrdf );
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcVSPosition();
			l_utils.DeclareCalcWSPosition();

			Declare_DecodeMaterial( l_writer );

			GLSL::Fog l_fog{ p_fogType, l_writer };

			// Shader outputs
			auto pxl_fragColor = l_writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			l_writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto l_ambient = l_writer.DeclLocale( cuT( "l_ambient" )
						, texture( c3d_mapReflection, vtx_texture ).xyz() );
					auto l_light = l_writer.DeclLocale( cuT( "l_light" )
						, texture( c3d_mapPostLight, vtx_texture ).xyz() );
					auto l_data1 = l_writer.DeclLocale( cuT( "l_data1" )
						, texture( c3d_mapData1, vtx_texture ) );
					auto l_data4 = l_writer.DeclLocale( cuT( "l_data4" )
						, texture( c3d_mapData4, vtx_texture ) );
					auto l_emissive = l_writer.DeclLocale( cuT( "l_emissive" )
						, l_data4.xyz() );
					auto l_flags = l_writer.DeclLocale( cuT( "l_flags" )
						, l_data1.w() );
					auto l_envMapIndex = l_writer.DeclLocale( cuT( "l_envMapIndex" )
						, 0_i );
					auto l_receiver = l_writer.DeclLocale( cuT( "l_receiver" )
						, 0_i );
					auto l_reflection = l_writer.DeclLocale( cuT( "l_reflection" )
						, 0_i );
					auto l_refraction = l_writer.DeclLocale( cuT( "l_refraction" )
						, 0_i );
					DecodeMaterial( l_writer
						, l_flags
						, l_receiver
						, l_reflection
						, l_refraction
						, l_envMapIndex );
					auto l_occlusion = l_writer.DeclLocale( cuT( "l_occlusion" )
						, 1.0_f );

					if ( p_ssao )
					{
						l_occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
					}

					l_ambient *= c3d_v4AmbientLight.xyz() * l_occlusion;

					auto l_refract = l_writer.DeclLocale( cuT( "l_refract" )
						, texture( c3d_mapRefraction, vtx_texture ) );
					
					IF( l_writer, l_refract != vec4( -1.0_f ) )
					{
						l_ambient = mix( l_refract.xyz(), l_ambient, l_refract.w() );
					}
					FI;

					pxl_fragColor = vec4( l_light + l_emissive + l_ambient, 1.0 );

					if ( p_fogType != FogType::eDisabled )
					{
						auto l_position = l_writer.DeclLocale( cuT( "l_position" )
							, l_utils.CalcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						l_fog.ApplyFog( pxl_fragColor, length( l_position ), l_position.z() );
					}
				} );
			return l_writer.Finalise();
		}
		
		ShaderProgramSPtr DoCreateProgram( Engine & p_engine
			, GLSL::FogType p_fogType
			, bool p_ssao
			, bool p_pbr )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_vtx = DoGetVertexProgram( p_engine );
			auto l_pxl = p_pbr
				? DoGetPbrPixelProgram( p_engine, p_fogType, p_ssao )
				: DoGetLegacyPixelProgram( p_engine, p_fogType, p_ssao );
			ShaderProgramSPtr l_program = p_engine.GetShaderProgramCache().GetNewProgram( false );
			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::ePixel );

			l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapReflection" ), ShaderType::ePixel )->SetValue( 0u );
			l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapRefraction" ), ShaderType::ePixel )->SetValue( 1u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eDepth ), ShaderType::ePixel )->SetValue( 2u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData1 ), ShaderType::ePixel )->SetValue( 3u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData2 ), ShaderType::ePixel )->SetValue( 4u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData3 ), ShaderType::ePixel )->SetValue( 5u );
			l_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData4 ), ShaderType::ePixel )->SetValue( 6u );

			if ( p_ssao )
			{
				l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapSsao" ), ShaderType::ePixel )->SetValue( 7u );
			}

			l_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapPostLight" ), ShaderType::ePixel )->SetValue( 8u );

			if ( p_pbr )
			{
				l_program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance, ShaderType::ePixel )->SetValue( 9u );
				l_program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered, ShaderType::ePixel )->SetValue( 10u );
				l_program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapBrdf, ShaderType::ePixel )->SetValue( 11u );
			}

			l_program->SetSource( ShaderType::eVertex, l_vtx );
			l_program->SetSource( ShaderType::ePixel, l_pxl );
			l_program->Initialise();
			return l_program;
		}

		RenderPipelineUPtr DoCreateRenderPipeline( Engine & p_engine
			, ShaderProgram & p_program
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfo )
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

	CombineProgram::CombineProgram( Engine & p_engine
		, VertexBuffer & p_vbo
		, MatrixUbo & p_matrixUbo
		, SceneUbo & p_sceneUbo
		, GpInfoUbo & p_gpInfo
		, bool p_ssao
		, bool p_pbr
		, GLSL::FogType p_fogType )
		: m_program{ DoCreateProgram( p_engine, p_fogType, p_ssao, p_pbr ) }
		, m_geometryBuffers{ DoCreateVao( p_engine, *m_program, p_vbo ) }
		, m_pipeline{ DoCreateRenderPipeline( p_engine, *m_program, p_matrixUbo, p_sceneUbo, p_gpInfo ) }
	{
	}

	CombineProgram::~CombineProgram()
	{
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void CombineProgram::Render()const
	{
		m_pipeline->Apply();
		m_geometryBuffers->Draw( 6u, 0 );
	}

	//*********************************************************************************************

	CombinePass::CombinePass( Engine & p_engine
		, Size const & p_size
		, SceneUbo & p_sceneUbo
		, SsaoConfig const & p_config )
		: m_size{ p_size }
		, m_viewport{ p_engine }
		, m_vertexBuffer{ DoCreateVbo( p_engine ) }
		, m_matrixUbo{ p_engine }
		, m_gpInfo{ p_engine }
		, m_programs
		{
			{
				CombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, p_config.m_enabled, false, GLSL::FogType::eDisabled },
				CombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, p_config.m_enabled, false, GLSL::FogType::eLinear },
				CombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, p_config.m_enabled, false, GLSL::FogType::eExponential },
				CombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, p_config.m_enabled, false, GLSL::FogType::eSquaredExponential },
				CombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, p_config.m_enabled, true, GLSL::FogType::eDisabled },
				CombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, p_config.m_enabled, true, GLSL::FogType::eLinear },
				CombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, p_config.m_enabled, true, GLSL::FogType::eExponential },
				CombineProgram{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, p_config.m_enabled, true, GLSL::FogType::eSquaredExponential }
			}
		}
		, m_ssaoEnabled{ p_config.m_enabled }
		, m_ssao{ p_engine, p_size, p_config }
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		m_matrixUbo.Update( m_viewport.GetProjection() );
	}

	CombinePass::~CombinePass()
	{
		m_matrixUbo.GetUbo().Cleanup();
		m_vertexBuffer->Cleanup();
	}

	void CombinePass::Render( GeometryPassResult const & p_gp
		, TextureUnit const & p_light
		, TextureUnit const & p_reflection
		, TextureUnit const & p_refraction
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj
		, Fog const & p_fog
		, FrameBuffer const & p_frameBuffer )
	{
		TextureUnit const * l_ssao = nullptr;

		if ( m_ssaoEnabled )
		{
			m_ssao.Render( p_gp
				, p_camera
				, p_invViewProj
				, p_invView
				, p_invProj );
			l_ssao = &m_ssao.GetResult();
		}

		p_frameBuffer.Bind( FrameBufferTarget::eDraw );
		p_frameBuffer.SetDrawBuffers();

		m_viewport.Apply();
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		uint32_t l_index{ 0u };
		p_reflection.GetTexture()->Bind( l_index );
		p_reflection.GetSampler()->Bind( l_index );
		++l_index;
		p_refraction.GetTexture()->Bind( l_index );
		p_refraction.GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Bind( l_index );
		++l_index;

		if ( m_ssaoEnabled )
		{
			l_ssao->GetSampler()->Bind( l_index );
			l_ssao->GetTexture()->Bind( l_index );
		}

		++l_index;
		p_light.GetTexture()->Bind( l_index );
		p_light.GetSampler()->Bind( l_index );

		m_programs[size_t( p_fog.GetType() )].Render();

		p_light.GetTexture()->Unbind( l_index );
		p_light.GetSampler()->Unbind( l_index );
		--l_index;

		if ( m_ssaoEnabled )
		{
			l_ssao->GetSampler()->Unbind( l_index );
			l_ssao->GetTexture()->Unbind( l_index );
		}

		--l_index;
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_refraction.GetTexture()->Bind( l_index );
		p_refraction.GetSampler()->Bind( l_index );
		--l_index;
		p_reflection.GetTexture()->Bind( l_index );
		p_reflection.GetSampler()->Bind( l_index );
		ENSURE( l_index == 0u );
	}

	void CombinePass::Render( GeometryPassResult const & p_gp
		, TextureUnit const & p_light
		, TextureUnit const & p_reflection
		, TextureUnit const & p_refraction
		, IblTextures const & p_ibl
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj
		, Fog const & p_fog
		, FrameBuffer const & p_frameBuffer )
	{
		TextureUnit const * l_ssao = nullptr;

		if ( m_ssaoEnabled )
		{
			m_ssao.Render( p_gp
				, p_camera
				, p_invViewProj
				, p_invView
				, p_invProj );
			l_ssao = &m_ssao.GetResult();
		}

		p_frameBuffer.Bind( FrameBufferTarget::eDraw );
		p_frameBuffer.SetDrawBuffers();

		m_viewport.Apply();
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		uint32_t l_index{ 0u };
		p_reflection.GetTexture()->Bind( l_index );
		p_reflection.GetSampler()->Bind( l_index );
		++l_index;
		p_refraction.GetTexture()->Bind( l_index );
		p_refraction.GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Bind( l_index );
		++l_index;
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Bind( l_index );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Bind( l_index );
		++l_index;

		if ( m_ssaoEnabled )
		{
			l_ssao->GetSampler()->Bind( l_index );
			l_ssao->GetTexture()->Bind( l_index );
		}

		++l_index;
		p_light.GetTexture()->Bind( l_index );
		p_light.GetSampler()->Bind( l_index );
		++l_index;
		p_ibl.GetIrradiance().GetTexture()->Bind( l_index );
		p_ibl.GetIrradiance().GetSampler()->Bind( l_index );
		++l_index;
		p_ibl.GetPrefilteredEnvironment().GetTexture()->Bind( l_index );
		p_ibl.GetPrefilteredEnvironment().GetSampler()->Bind( l_index );
		++l_index;
		p_ibl.GetPrefilteredBrdf().GetTexture()->Bind( l_index );
		p_ibl.GetPrefilteredBrdf().GetSampler()->Bind( l_index );

		m_programs[size_t( p_fog.GetType() ) + size_t( GLSL::FogType::eCount )].Render();

		p_ibl.GetPrefilteredBrdf().GetSampler()->Unbind( l_index );
		p_ibl.GetPrefilteredBrdf().GetTexture()->Unbind( l_index );
		--l_index;
		p_ibl.GetPrefilteredEnvironment().GetSampler()->Unbind( l_index );
		p_ibl.GetPrefilteredEnvironment().GetTexture()->Unbind( l_index );
		--l_index;
		p_ibl.GetIrradiance().GetSampler()->Unbind( l_index );
		p_ibl.GetIrradiance().GetTexture()->Unbind( l_index );
		--l_index;
		p_light.GetSampler()->Unbind( l_index );
		p_light.GetTexture()->Unbind( l_index );
		--l_index;

		if ( m_ssaoEnabled )
		{
			l_ssao->GetSampler()->Unbind( l_index );
			l_ssao->GetTexture()->Unbind( l_index );
		}

		--l_index;
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( l_index );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( l_index );
		--l_index;
		p_refraction.GetTexture()->Unbind( l_index );
		p_refraction.GetSampler()->Unbind( l_index );
		--l_index;
		p_reflection.GetTexture()->Unbind( l_index );
		p_reflection.GetSampler()->Unbind( l_index );
		ENSURE( l_index == 0u );
	}

	//*********************************************************************************************
}

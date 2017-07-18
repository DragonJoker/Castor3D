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
			auto declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & renderSystem = *p_engine.GetRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( p_engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->Resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->GetData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->Initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		GeometryBuffersSPtr DoCreateVao( Engine & p_engine
			, ShaderProgram & p_program
			, VertexBuffer & p_vbo )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			auto result = renderSystem.CreateGeometryBuffers( Topology::eTriangles
				, p_program );
			result->Initialise( { p_vbo }, nullptr );
			return result;
		}

		GLSL::Shader DoGetVertexProgram( Engine & p_engine )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_GPINFO( writer );
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position, 0.0, 1.0 );
				} );
			return writer.Finalise();
		}
		
		GLSL::Shader DoGetLegacyPixelProgram( Engine & p_engine
			, GLSL::FogType p_fogType
			, bool p_ssao )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto c3d_mapReflection = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapReflection" ) );
			auto c3d_mapRefraction = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapRefraction" ) );
			auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
			auto c3d_mapSsao = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapSsao" ), p_ssao );
			auto c3d_mapPostLight = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			GLSL::Utils utils{ writer };
			utils.DeclareCalcVSPosition();
			utils.DeclareCalcWSPosition();

			Declare_DecodeMaterial( writer );

			GLSL::Fog fog{ p_fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ambient = writer.DeclLocale( cuT( "ambient" )
						, c3d_v4AmbientLight.xyz() );
					auto postLight = writer.DeclLocale( cuT( "light" )
						, texture( c3d_mapPostLight, vtx_texture ).xyz() );
					auto data1 = writer.DeclLocale( cuT( "data1" )
						, texture( c3d_mapData1, vtx_texture ) );
					auto data4 = writer.DeclLocale( cuT( "data4" )
						, texture( c3d_mapData4, vtx_texture ) );
					auto emissive = writer.DeclLocale( cuT( "emissive" )
						, data4.xyz() );
					auto flags = writer.DeclLocale( cuT( "flags" )
						, data1.w() );
					auto envMapIndex = writer.DeclLocale( cuT( "envMapIndex" )
						, 0_i );
					auto receiver = writer.DeclLocale( cuT( "receiver" )
						, 0_i );
					auto reflection = writer.DeclLocale( cuT( "reflection" )
						, 0_i );
					auto refraction = writer.DeclLocale( cuT( "refraction" )
						, 0_i );
					DecodeMaterial( writer
						, flags
						, receiver
						, reflection
						, refraction
						, envMapIndex );
					auto data2 = writer.DeclLocale( cuT( "data2" )
						, texture( c3d_mapData2, vtx_texture ) );
					auto diffuse = writer.DeclLocale( cuT( "diffuse" )
						, data2.xyz() );

					IF( writer, "envMapIndex >= 1 && reflection != 0" )
					{
						auto reflect = writer.DeclLocale( cuT( "reflect" )
							, texture( c3d_mapReflection, vtx_texture ).xyz() );
						diffuse = reflect;
						ambient = vec3( 0.0_f );
					}
					ELSE
					{
						if ( p_ssao )
						{
							ambient *= texture( c3d_mapSsao, vtx_texture ).r();
						}

						ambient *= diffuse;
						diffuse = postLight;
					}
					FI;

					IF( writer, "envMapIndex >= 1 && refraction != 0" )
					{
						auto refract = writer.DeclLocale( cuT( "refract" )
							, texture( c3d_mapRefraction, vtx_texture ) );

						IF( writer, reflection != 0_i )
						{
							ambient = mix( refract.xyz(), diffuse, refract.w() );
							diffuse = vec3( 0.0_f );
						}
						ELSE
						{
							ambient = refract.xyz();
							diffuse = vec3( 0.0_f );
						}
						FI;
					}
					FI;

					pxl_fragColor = vec4( diffuse + emissive + ambient, 1.0 );

					if ( p_fogType != FogType::eDisabled )
					{
						auto position = writer.DeclLocale( cuT( "position" )
							, utils.CalcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						fog.ApplyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.Finalise();
		}
		
		GLSL::Shader DoGetPbrPixelProgram( Engine & p_engine
			, GLSL::FogType p_fogType
			, bool p_ssao )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto c3d_mapReflection = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapReflection" ) );
			auto c3d_mapRefraction = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapRefraction" ) );
			auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
			auto c3d_mapSsao = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapSsao" ), p_ssao );
			auto c3d_mapPostLight = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto c3d_mapIrradiance = writer.DeclUniform< SamplerCube >( ShaderProgram::MapIrradiance );
			auto c3d_mapPrefiltered = writer.DeclUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
			auto c3d_mapBrdf = writer.DeclUniform< Sampler2D >( ShaderProgram::MapBrdf );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			GLSL::Utils utils{ writer };
			utils.DeclareCalcVSPosition();
			utils.DeclareCalcWSPosition();

			Declare_DecodeMaterial( writer );

			GLSL::Fog fog{ p_fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ambient = writer.DeclLocale( cuT( "ambient" )
						, texture( c3d_mapReflection, vtx_texture ).xyz() );
					auto light = writer.DeclLocale( cuT( "light" )
						, texture( c3d_mapPostLight, vtx_texture ).xyz() );
					auto data1 = writer.DeclLocale( cuT( "data1" )
						, texture( c3d_mapData1, vtx_texture ) );
					auto data4 = writer.DeclLocale( cuT( "data4" )
						, texture( c3d_mapData4, vtx_texture ) );
					auto emissive = writer.DeclLocale( cuT( "emissive" )
						, data4.xyz() );
					auto flags = writer.DeclLocale( cuT( "flags" )
						, data1.w() );
					auto envMapIndex = writer.DeclLocale( cuT( "envMapIndex" )
						, 0_i );
					auto receiver = writer.DeclLocale( cuT( "receiver" )
						, 0_i );
					auto reflection = writer.DeclLocale( cuT( "reflection" )
						, 0_i );
					auto refraction = writer.DeclLocale( cuT( "refraction" )
						, 0_i );
					DecodeMaterial( writer
						, flags
						, receiver
						, reflection
						, refraction
						, envMapIndex );
					auto occlusion = writer.DeclLocale( cuT( "occlusion" )
						, 1.0_f );

					if ( p_ssao )
					{
						occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
					}

					ambient *= c3d_v4AmbientLight.xyz() * occlusion;

					auto refract = writer.DeclLocale( cuT( "refract" )
						, texture( c3d_mapRefraction, vtx_texture ) );
					
					IF( writer, refract != vec4( -1.0_f ) )
					{
						ambient = mix( refract.xyz(), ambient, refract.w() );
					}
					FI;

					pxl_fragColor = vec4( light + emissive + ambient, 1.0 );

					if ( p_fogType != FogType::eDisabled )
					{
						auto position = writer.DeclLocale( cuT( "position" )
							, utils.CalcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						fog.ApplyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.Finalise();
		}
		
		ShaderProgramSPtr DoCreateProgram( Engine & p_engine
			, GLSL::FogType p_fogType
			, bool p_ssao
			, bool p_pbr )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			auto vtx = DoGetVertexProgram( p_engine );
			auto pxl = p_pbr
				? DoGetPbrPixelProgram( p_engine, p_fogType, p_ssao )
				: DoGetLegacyPixelProgram( p_engine, p_fogType, p_ssao );
			ShaderProgramSPtr program = p_engine.GetShaderProgramCache().GetNewProgram( false );
			program->CreateObject( ShaderType::eVertex );
			program->CreateObject( ShaderType::ePixel );

			program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapReflection" ), ShaderType::ePixel )->SetValue( 0u );
			program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapRefraction" ), ShaderType::ePixel )->SetValue( 1u );
			program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eDepth ), ShaderType::ePixel )->SetValue( 2u );
			program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData1 ), ShaderType::ePixel )->SetValue( 3u );
			program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData2 ), ShaderType::ePixel )->SetValue( 4u );
			program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData3 ), ShaderType::ePixel )->SetValue( 5u );
			program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData4 ), ShaderType::ePixel )->SetValue( 6u );

			if ( p_ssao )
			{
				program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapSsao" ), ShaderType::ePixel )->SetValue( 7u );
			}

			program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapPostLight" ), ShaderType::ePixel )->SetValue( 8u );

			if ( p_pbr )
			{
				program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance, ShaderType::ePixel )->SetValue( 9u );
				program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered, ShaderType::ePixel )->SetValue( 10u );
				program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapBrdf, ShaderType::ePixel )->SetValue( 11u );
			}

			program->SetSource( ShaderType::eVertex, vtx );
			program->SetSource( ShaderType::ePixel, pxl );
			program->Initialise();
			return program;
		}

		RenderPipelineUPtr DoCreateRenderPipeline( Engine & p_engine
			, ShaderProgram & p_program
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfo )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.SetCulledFaces( Culling::eNone );
			BlendState blState;
			auto pipeline = p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, std::move( blState )
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
			pipeline->AddUniformBuffer( p_matrixUbo.GetUbo() );
			pipeline->AddUniformBuffer( p_sceneUbo.GetUbo() );
			pipeline->AddUniformBuffer( p_gpInfo.GetUbo() );
			return pipeline;
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
		TextureUnit const * ssao = nullptr;

		if ( m_ssaoEnabled )
		{
			m_ssao.Render( p_gp
				, p_camera
				, p_invViewProj
				, p_invView
				, p_invProj );
			ssao = &m_ssao.GetResult();
		}

		p_frameBuffer.Bind( FrameBufferTarget::eDraw );
		p_frameBuffer.SetDrawBuffers();

		m_viewport.Apply();
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		uint32_t index{ 0u };
		p_reflection.GetTexture()->Bind( index );
		p_reflection.GetSampler()->Bind( index );
		++index;
		p_refraction.GetTexture()->Bind( index );
		p_refraction.GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Bind( index );
		++index;

		if ( m_ssaoEnabled )
		{
			ssao->GetSampler()->Bind( index );
			ssao->GetTexture()->Bind( index );
		}

		++index;
		p_light.GetTexture()->Bind( index );
		p_light.GetSampler()->Bind( index );

		m_programs[size_t( p_fog.GetType() )].Render();

		p_light.GetTexture()->Unbind( index );
		p_light.GetSampler()->Unbind( index );
		--index;

		if ( m_ssaoEnabled )
		{
			ssao->GetSampler()->Unbind( index );
			ssao->GetTexture()->Unbind( index );
		}

		--index;
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Unbind( index );
		--index;
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Unbind( index );
		--index;
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Unbind( index );
		--index;
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Unbind( index );
		--index;
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( index );
		--index;
		p_refraction.GetTexture()->Bind( index );
		p_refraction.GetSampler()->Bind( index );
		--index;
		p_reflection.GetTexture()->Bind( index );
		p_reflection.GetSampler()->Bind( index );
		ENSURE( index == 0u );
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
		TextureUnit const * ssao = nullptr;

		if ( m_ssaoEnabled )
		{
			m_ssao.Render( p_gp
				, p_camera
				, p_invViewProj
				, p_invView
				, p_invProj );
			ssao = &m_ssao.GetResult();
		}

		p_frameBuffer.Bind( FrameBufferTarget::eDraw );
		p_frameBuffer.SetDrawBuffers();

		m_viewport.Apply();
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		uint32_t index{ 0u };
		p_reflection.GetTexture()->Bind( index );
		p_reflection.GetSampler()->Bind( index );
		++index;
		p_refraction.GetTexture()->Bind( index );
		p_refraction.GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Bind( index );
		++index;
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Bind( index );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Bind( index );
		++index;

		if ( m_ssaoEnabled )
		{
			ssao->GetSampler()->Bind( index );
			ssao->GetTexture()->Bind( index );
		}

		++index;
		p_light.GetTexture()->Bind( index );
		p_light.GetSampler()->Bind( index );
		++index;
		p_ibl.GetIrradiance().GetTexture()->Bind( index );
		p_ibl.GetIrradiance().GetSampler()->Bind( index );
		++index;
		p_ibl.GetPrefilteredEnvironment().GetTexture()->Bind( index );
		p_ibl.GetPrefilteredEnvironment().GetSampler()->Bind( index );
		++index;
		p_ibl.GetPrefilteredBrdf().GetTexture()->Bind( index );
		p_ibl.GetPrefilteredBrdf().GetSampler()->Bind( index );

		m_programs[size_t( p_fog.GetType() ) + size_t( GLSL::FogType::eCount )].Render();

		p_ibl.GetPrefilteredBrdf().GetSampler()->Unbind( index );
		p_ibl.GetPrefilteredBrdf().GetTexture()->Unbind( index );
		--index;
		p_ibl.GetPrefilteredEnvironment().GetSampler()->Unbind( index );
		p_ibl.GetPrefilteredEnvironment().GetTexture()->Unbind( index );
		--index;
		p_ibl.GetIrradiance().GetSampler()->Unbind( index );
		p_ibl.GetIrradiance().GetTexture()->Unbind( index );
		--index;
		p_light.GetSampler()->Unbind( index );
		p_light.GetTexture()->Unbind( index );
		--index;

		if ( m_ssaoEnabled )
		{
			ssao->GetSampler()->Unbind( index );
			ssao->GetTexture()->Unbind( index );
		}

		--index;
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Unbind( index );
		--index;
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Unbind( index );
		--index;
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Unbind( index );
		--index;
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Unbind( index );
		--index;
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( index );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( index );
		--index;
		p_refraction.GetTexture()->Unbind( index );
		p_refraction.GetSampler()->Unbind( index );
		--index;
		p_reflection.GetTexture()->Unbind( index );
		p_reflection.GetSampler()->Unbind( index );
		ENSURE( index == 0u );
	}

	//*********************************************************************************************
}

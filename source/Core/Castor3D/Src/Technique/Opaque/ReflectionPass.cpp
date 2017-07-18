#include "ReflectionPass.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Skybox.hpp>
#include <Shader/MatrixUbo.hpp>
#include <Shader/SceneUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	namespace
	{
		static uint32_t constexpr c_environmentStart = 6u;
		static uint32_t constexpr c_pbrEnvironmentCount = 12u;
		static uint32_t constexpr c_legEnvironmentCount = 16u;

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

		GLSL::Shader DoCreateVertexProgram( RenderSystem & p_renderSystem )
		{
			using namespace GLSL;
			auto writer = p_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			return writer.Finalise();
		}

		GLSL::Shader DoCreateLegacyPixelProgram( RenderSystem & p_renderSystem )
		{
			using namespace GLSL;
			auto writer = p_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			UBO_HDR_CONFIG( writer );
			auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
			auto c3d_mapPostLight = writer.DeclUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto c3d_mapEnvironment = writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment, c_legEnvironmentCount );
			auto c3d_fresnelBias = writer.DeclUniform< Float >( cuT( "c3d_fresnelBias" ), 0.10_f );
			auto c3d_fresnelScale = writer.DeclUniform< Float >( cuT( "c3d_fresnelScale" ), 0.25_f );
			auto c3d_fresnelPower = writer.DeclUniform< Float >( cuT( "c3d_fresnelPower" ), 0.30_f );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_reflection = writer.DeclFragData< Vec3 >( cuT( "pxl_reflection" ), 0 );
			auto pxl_refraction = writer.DeclFragData< Vec4 >( cuT( "pxl_refraction" ), 1 );

			GLSL::Utils utils{ writer };
			utils.DeclareCalcWSPosition();
			Declare_DecodeMaterial( writer );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.DeclLocale( cuT( "data1" ), texture( c3d_mapData1, vtx_texture ) );
				auto flags = writer.DeclLocale( cuT( "flags" ), data1.w() );
				auto envMapIndex = writer.DeclLocale( cuT( "envMapIndex" ), 0_i );
				auto receiver = writer.DeclLocale( cuT( "receiver" ), 0_i );
				auto reflection = writer.DeclLocale( cuT( "reflection" ), 0_i );
				auto refraction = writer.DeclLocale( cuT( "refraction" ), 0_i );
				DecodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );
				auto postLight = writer.DeclLocale( cuT( "postLight" ), texture( c3d_mapPostLight, vtx_texture ).xyz() );

				IF( writer, "envMapIndex < 1 || ( reflection + refraction == 0 )" )
				{
					writer.Discard();
				}
				FI;

				pxl_reflection = vec3( 0.0_f );
				pxl_refraction = vec4( -1.0_f );
				auto position = writer.DeclLocale( cuT( "position" )
					, utils.CalcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );
				auto normal = writer.DeclLocale( cuT( "normal" )
					, normalize( data1.xyz() ) );
				auto incident = writer.DeclLocale( cuT( "incident" )
					, normalize( position - c3d_v3CameraPosition ) );
				envMapIndex = envMapIndex - 1_i;
				auto diffuse = writer.DeclLocale( cuT( "diffuse" ), texture( c3d_mapData2, vtx_texture ).xyz() );

				IF( writer, reflection != 0_i )
				{
					auto reflected = writer.DeclLocale( cuT( "reflected" )
						, reflect( incident, normal ) );
					pxl_reflection = texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();
				}
				FI;

				IF( writer, refraction != 0_i )
				{
					auto ratio = writer.DeclLocale( cuT( "ratio" )
						, texture( c3d_mapData4, vtx_texture ).w() );
					auto subRatio = writer.DeclLocale( cuT( "subRatio" )
						, 1.0_f - ratio );
					auto addRatio = writer.DeclLocale( cuT( "addRatio" )
						, 1.0_f + ratio );
					auto reflectance = writer.DeclLocale( cuT( "reflectance" )
						, writer.Paren( subRatio * subRatio ) / writer.Paren( addRatio * addRatio ) );
					auto product = writer.DeclLocale( cuT( "product" )
						, max( 0.0_f, dot( -incident, normal ) ) );
					auto fresnel = writer.DeclLocale( cuT( "fresnel" )
						, reflectance + writer.Paren( 1.0_f - reflectance ) * pow( 1.0_f - product, 5.0 ) );
					auto refracted = writer.DeclLocale( cuT( "refracted" )
						, refract( incident, normal, ratio ) );
					pxl_refraction.xyz() = texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz();
					pxl_refraction.xyz() *= diffuse / length( diffuse );
					pxl_refraction.w() = fresnel;
				}
				FI;

				IF( writer, cuT( "reflection != 0 && refraction == 0" ) )
				{
					pxl_reflection *= diffuse / length( diffuse );
				}
				FI;
			} );
			return writer.Finalise();
		}

		GLSL::Shader DoCreatePbrMRPixelProgram( RenderSystem & p_renderSystem )
		{
			using namespace GLSL;
			auto writer = p_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			UBO_HDR_CONFIG( writer );
			auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
			auto c3d_mapBrdf = writer.DeclUniform< Sampler2D >( ShaderProgram::MapBrdf );
			auto c3d_mapIrradiance = writer.DeclUniform< SamplerCube >( ShaderProgram::MapIrradiance, c_pbrEnvironmentCount );
			auto c3d_mapPrefiltered = writer.DeclUniform< SamplerCube >( ShaderProgram::MapPrefiltered, c_pbrEnvironmentCount );
			auto c3d_fresnelBias = writer.DeclUniform< Float >( cuT( "c3d_fresnelBias" ), 0.10_f );
			auto c3d_fresnelScale = writer.DeclUniform< Float >( cuT( "c3d_fresnelScale" ), 0.25_f );
			auto c3d_fresnelPower = writer.DeclUniform< Float >( cuT( "c3d_fresnelPower" ), 0.30_f );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_reflection = writer.DeclFragData< Vec3 >( cuT( "pxl_reflection" ), 0 );
			auto pxl_refraction = writer.DeclFragData< Vec4 >( cuT( "pxl_refraction" ), 1 );

			GLSL::Utils utils{ writer };
			utils.DeclareCalcWSPosition();
			utils.DeclareFresnelSchlick();
			utils.DeclareComputeMetallicIBL();
			Declare_DecodeMaterial( writer );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.DeclLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
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

				pxl_reflection = vec3( 0.0_f );
				pxl_refraction = vec4( -1.0_f );
				auto data2 = writer.DeclLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.DeclLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto albedo = writer.DeclLocale( cuT( "albedo" )
					, data2.xyz() );
				auto metalness = writer.DeclLocale( cuT( "metalness" )
					, data3.r() );
				auto roughness = writer.DeclLocale( cuT( "roughness" )
					, data3.g() );
				auto occlusion = writer.DeclLocale( cuT( "occlusion" )
					, data3.b() );
				auto normal = writer.DeclLocale( cuT( "normal" )
					, data1.xyz() );
				auto position = writer.DeclLocale( cuT( "position" )
					, utils.CalcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );
				auto incident = writer.DeclLocale( cuT( "incident" )
					, normalize( position - c3d_v3CameraPosition ) );

				pxl_reflection = occlusion * utils.ComputeMetallicIBL( normal
					, position
					, albedo
					, metalness
					, roughness
					, c3d_v3CameraPosition
					, c3d_mapIrradiance[envMapIndex]
					, c3d_mapPrefiltered[envMapIndex]
					, c3d_mapBrdf
					, envMapIndex );

				auto ratio = writer.DeclLocale( cuT( "ratio" )
					, texture( c3d_mapData4, vtx_texture ).w() );

				IF( writer, ratio != 0.0_f )
				{
					auto subRatio = writer.DeclLocale( cuT( "subRatio" )
						, 1.0_f - ratio );
					auto addRatio = writer.DeclLocale( cuT( "addRatio" )
						, 1.0_f + ratio );
					auto reflectance = writer.DeclLocale( cuT( "reflectance" )
						, writer.Paren( subRatio * subRatio ) / writer.Paren( addRatio * addRatio ) );
					auto product = writer.DeclLocale( cuT( "product" )
						, max( 0.0_f, dot( -incident, normal ) ) );
					auto fresnel = writer.DeclLocale( cuT( "fresnel" )
						, reflectance + writer.Paren( max( 1.0_f - roughness, reflectance ) - reflectance ) * pow( 1.0_f - product, 5.0 ) );
					auto refracted = writer.DeclLocale( cuT( "refracted" )
						, refract( incident, normal, ratio ) );
					refracted.y() = writer.Ternary( envMapIndex != 0_i, refracted.y(), -refracted.y() );
					pxl_refraction.xyz() = texture( c3d_mapPrefiltered[envMapIndex], refracted, roughness * Utils::MaxIblReflectionLod ).xyz();
					pxl_refraction.xyz() *= albedo / length( albedo );
					pxl_refraction.w() = fresnel;
				}
				FI;
			} );
			return writer.Finalise();
		}

		GLSL::Shader DoCreatePbrSGPixelProgram( RenderSystem & p_renderSystem )
		{
			using namespace GLSL;
			auto writer = p_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			UBO_HDR_CONFIG( writer );
			auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
			auto c3d_mapBrdf = writer.DeclUniform< Sampler2D >( ShaderProgram::MapBrdf );
			auto c3d_mapIrradiance = writer.DeclUniform< SamplerCube >( ShaderProgram::MapIrradiance, c_pbrEnvironmentCount );
			auto c3d_mapPrefiltered = writer.DeclUniform< SamplerCube >( ShaderProgram::MapPrefiltered, c_pbrEnvironmentCount );
			auto c3d_fresnelBias = writer.DeclUniform< Float >( cuT( "c3d_fresnelBias" ), 0.10_f );
			auto c3d_fresnelScale = writer.DeclUniform< Float >( cuT( "c3d_fresnelScale" ), 0.25_f );
			auto c3d_fresnelPower = writer.DeclUniform< Float >( cuT( "c3d_fresnelPower" ), 0.30_f );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_reflection = writer.DeclFragData< Vec3 >( cuT( "pxl_reflection" ), 0 );
			auto pxl_refraction = writer.DeclFragData< Vec4 >( cuT( "pxl_refraction" ), 1 );

			GLSL::Utils utils{ writer };
			utils.DeclareCalcWSPosition();
			utils.DeclareFresnelSchlick();
			utils.DeclareComputeSpecularIBL();
			Declare_DecodeMaterial( writer );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.DeclLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
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

				pxl_reflection = vec3( 0.0_f );
				pxl_refraction = vec4( -1.0_f );
				auto data2 = writer.DeclLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.DeclLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto diffuse = writer.DeclLocale( cuT( "diffuse" )
					, data2.xyz() );
				auto specular = writer.DeclLocale( cuT( "specular" )
					, data3.xyz() );
				auto glossiness = writer.DeclLocale( cuT( "glossiness" )
					, data2.w() );
				auto occlusion = writer.DeclLocale( cuT( "occlusion" )
					, data3.w() );
				auto normal = writer.DeclLocale( cuT( "normal" )
					, data1.xyz() );
				auto position = writer.DeclLocale( cuT( "position" )
					, utils.CalcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );

				pxl_reflection = occlusion * utils.ComputeSpecularIBL( normal
					, position
					, diffuse
					, specular
					, glossiness
					, c3d_v3CameraPosition
					, c3d_mapIrradiance[envMapIndex]
					, c3d_mapPrefiltered[envMapIndex]
					, c3d_mapBrdf
					, envMapIndex );

				auto ratio = writer.DeclLocale( cuT( "ratio" )
					, texture( c3d_mapData4, vtx_texture ).w() );

				IF( writer, ratio != 0.0_f )
				{
					auto incident = writer.DeclLocale( cuT( "incident" )
						, normalize( position - c3d_v3CameraPosition ) );
					auto roughness = writer.DeclLocale( cuT( "roughness" )
						, 1.0_f - glossiness );
					auto subRatio = writer.DeclLocale( cuT( "subRatio" )
						, 1.0_f - ratio );
					auto addRatio = writer.DeclLocale( cuT( "addRatio" )
						, 1.0_f + ratio );
					auto reflectance = writer.DeclLocale( cuT( "reflectance" )
						, writer.Paren( subRatio * subRatio ) / writer.Paren( addRatio * addRatio ) );
					auto product = writer.DeclLocale( cuT( "product" )
						, max( 0.0_f, dot( -incident, normal ) ) );
					auto fresnel = writer.DeclLocale( cuT( "fresnel" )
						, reflectance + writer.Paren( max( 1.0_f - roughness, reflectance ) - reflectance ) * pow( 1.0_f - product, 5.0 ) );
					auto refracted = writer.DeclLocale( cuT( "refract" )
						, refract( incident, normal, ratio ) );
					refracted.y() = writer.Ternary( envMapIndex != 0_i, refracted.y(), -refracted.y() );
					pxl_refraction.xyz() = texture( c3d_mapPrefiltered[envMapIndex], refracted, roughness * Utils::MaxIblReflectionLod ).xyz();
					pxl_refraction.xyz() *= diffuse / length( diffuse );
					pxl_refraction.w() = fresnel;
				}
				FI;
			} );
			return writer.Finalise();
		}

		ShaderProgramSPtr DoCreateProgram( Engine & p_engine
			, bool p_pbr
			, bool p_isMR )
		{
			auto & renderSystem = *p_engine.GetRenderSystem();
			auto vtx = DoCreateVertexProgram( renderSystem );
			auto pxl = p_pbr
				? p_isMR
					? DoCreatePbrMRPixelProgram( renderSystem )
					: DoCreatePbrSGPixelProgram( renderSystem )
				: DoCreateLegacyPixelProgram( renderSystem );
			auto result = p_engine.GetShaderProgramCache().GetNewProgram( false );
			result->CreateObject( ShaderType::eVertex );
			result->CreateObject( ShaderType::ePixel );
			result->SetSource( ShaderType::eVertex, vtx );
			result->SetSource( ShaderType::ePixel, pxl );
			result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eDepth ), ShaderType::ePixel )->SetValue( 0u );
			result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData1 ), ShaderType::ePixel )->SetValue( 1u );
			result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData2 ), ShaderType::ePixel )->SetValue( 2u );
			result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData3 ), ShaderType::ePixel )->SetValue( 3u );
			result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData4 ), ShaderType::ePixel )->SetValue( 4u );
			result->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapPostLight" ), ShaderType::ePixel )->SetValue( 5u );
			int c = int( c_environmentStart );

			if ( p_pbr )
			{
				result->CreateUniform< UniformType::eSampler >( ShaderProgram::MapBrdf, ShaderType::ePixel )->SetValue( c );
				++c;
				result->CreateUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance, ShaderType::ePixel, c_pbrEnvironmentCount )->SetValues(
				{
					c + 0, c + 1, c + 2, c + 3, c + 4, c + 5, c + 6, c + 7, c + 8, c + 9, c + 10, c + 11,
				} );
				c += c_pbrEnvironmentCount;
				result->CreateUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered, ShaderType::ePixel, c_pbrEnvironmentCount )->SetValues(
				{
					c + 0, c + 1, c + 2, c + 3, c + 4, c + 5, c + 6, c + 7, c + 8, c + 9, c + 10, c + 11,
				} );
			}
			else
			{
				result->CreateUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel, c_legEnvironmentCount )->SetValues(
				{
					c + 0, c + 1, c + 2, c + 3, c + 4, c + 5, c + 6, c + 7,
					c + 8, c + 9, c + 10, c + 11, c + 12, c + 13, c + 14, c + 15,
				} );
			}

			result->Initialise();
			return result;
		}

		RenderPipelineUPtr DoCreateRenderPipeline( Engine & p_engine
			, ShaderProgram & p_program
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfoUbo
			, HdrConfigUbo & p_configUbo )
		{
			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eNone );
			DepthStencilState dsState;
			dsState.SetDepthTest( false );
			dsState.SetDepthMask( WritingMask::eZero );
			BlendState blState;
			auto result = p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
				, std::move( rsState )
				, std::move( blState )
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
			result->AddUniformBuffer( p_matrixUbo.GetUbo() );
			result->AddUniformBuffer( p_sceneUbo.GetUbo() );
			result->AddUniformBuffer( p_gpInfoUbo.GetUbo() );
			result->AddUniformBuffer( p_configUbo.GetUbo() );
			return result;
		}
	}

	ReflectionPass::ProgramPipeline::ProgramPipeline( Engine & p_engine
		, VertexBuffer & p_vbo
		, MatrixUbo & p_matrixUbo
		, SceneUbo & p_sceneUbo
		, GpInfoUbo & p_gpInfo
		, HdrConfigUbo & p_configUbo
		, bool p_pbr
		, bool p_isMR )
		: m_program{ DoCreateProgram( p_engine, p_pbr, p_isMR ) }
		, m_geometryBuffers{ DoCreateVao( p_engine, *m_program, p_vbo ) }
		, m_pipeline{ DoCreateRenderPipeline( p_engine, *m_program, p_matrixUbo, p_sceneUbo, p_gpInfo, p_configUbo ) }
	{
	}

	ReflectionPass::ProgramPipeline::~ProgramPipeline()
	{
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void ReflectionPass::ProgramPipeline::Render()
	{
		m_pipeline->Apply();
		m_geometryBuffers->Draw( 6u, 0 );
	}

	ReflectionPass::ReflectionPass( Engine & p_engine
		, Size const & p_size
		, SceneUbo & p_sceneUbo )
		: OwnedBy< Engine >{ p_engine }
		, m_size{ p_size }
		, m_reflection{ p_engine }
		, m_refraction{ p_engine }
		, m_frameBuffer{ p_engine.GetRenderSystem()->CreateFrameBuffer() }
		, m_viewport{ p_engine }
		, m_vertexBuffer{ DoCreateVbo( p_engine ) }
		, m_matrixUbo{ p_engine }
		, m_gpInfo{ p_engine }
		, m_configUbo{ p_engine }
		, m_programs
		{
			{
				ProgramPipeline{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, m_configUbo, false, false },
				ProgramPipeline{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, m_configUbo, true, false },
				ProgramPipeline{ p_engine, *m_vertexBuffer, m_matrixUbo, p_sceneUbo, m_gpInfo, m_configUbo, true, true },
			}
		}
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		m_matrixUbo.Update( m_viewport.GetProjection() );

		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eTransparentBlack ) );
		bool result = m_frameBuffer->Create();

		if ( result )
		{
			result = m_frameBuffer->Initialise( p_size );
		}

		if ( result )
		{
			auto texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB32F
				, p_size );
			texture->GetImage().InitialiseSource();

			m_reflection.SetIndex( 0u );
			m_reflection.SetTexture( texture );
			m_reflection.SetSampler( p_engine.GetLightsSampler() );
			result = m_reflection.Initialise();
			ENSURE( result );
			m_reflectAttach = m_frameBuffer->CreateAttachment( texture );

			texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA32F
				, p_size );
			texture->GetImage().InitialiseSource();

			m_refraction.SetIndex( 1u );
			m_refraction.SetTexture( texture );
			m_refraction.SetSampler( p_engine.GetLightsSampler() );
			result = m_refraction.Initialise();
			ENSURE( result );
			m_refractAttach = m_frameBuffer->CreateAttachment( texture );

			m_frameBuffer->Bind();
			m_frameBuffer->Attach( AttachmentPoint::eColour
				, 0u
				, m_reflectAttach
				, texture->GetType() );
			m_frameBuffer->Attach( AttachmentPoint::eColour
				, 1u
				, m_refractAttach
				, texture->GetType() );
			ENSURE( m_frameBuffer->IsComplete() );
			m_frameBuffer->SetDrawBuffers();
			m_frameBuffer->Unbind();
		}
	}

	ReflectionPass::~ReflectionPass()
	{
		m_frameBuffer->Bind();
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_frameBuffer->Destroy();
		m_refractAttach.reset();
		m_reflectAttach.reset();
		m_reflection.Cleanup();
		m_refraction.Cleanup();

		m_configUbo.GetUbo().Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
		m_vertexBuffer->Cleanup();
	}

	void ReflectionPass::Render( GeometryPassResult & p_gp
		, Castor3D::TextureUnit const & p_lp
		, Scene const & p_scene
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Clear( BufferComponent::eColour );
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		m_configUbo.Update( p_scene.GetHdrConfig() );
		auto & maps = p_scene.GetEnvironmentMaps();
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( 0u );
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Bind( 1u );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Bind( 1u );
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Bind( 2u );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Bind( 2u );
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Bind( 3u );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Bind( 3u );
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Bind( 4u );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Bind( 4u );
		p_lp.GetTexture()->Bind( 5u );
		p_lp.GetSampler()->Bind( 5u );

		if ( p_scene.GetMaterialsType() == MaterialType::ePbrMetallicRoughness
			|| p_scene.GetMaterialsType() == MaterialType::ePbrSpecularGlossiness )
		{
			auto index = c_environmentStart;
			auto & skyboxIbl = p_scene.GetSkybox().GetIbl();
			skyboxIbl.GetPrefilteredBrdf().GetTexture()->Bind( index );
			skyboxIbl.GetPrefilteredBrdf().GetSampler()->Bind( index );
			++index;
			skyboxIbl.GetIrradiance().GetTexture()->Bind( index );
			skyboxIbl.GetIrradiance().GetSampler()->Bind( index );
			skyboxIbl.GetPrefilteredEnvironment().GetTexture()->Bind( index + c_pbrEnvironmentCount );
			skyboxIbl.GetPrefilteredEnvironment().GetSampler()->Bind( index + c_pbrEnvironmentCount );
			++index;

			for ( auto & map : maps )
			{
				auto & ibl = map.get().GetIbl();
				ibl.GetIrradiance().GetTexture()->Bind( index );
				ibl.GetIrradiance().GetSampler()->Bind( index );
				ibl.GetPrefilteredEnvironment().GetTexture()->Bind( index + c_pbrEnvironmentCount );
				ibl.GetPrefilteredEnvironment().GetSampler()->Bind( index + c_pbrEnvironmentCount );
				++index;
			}

			auto program = p_scene.GetMaterialsType() == MaterialType::ePbrMetallicRoughness
				? 2u
				: 1u;
			m_programs[program].Render();
			index = c_environmentStart;
			skyboxIbl.GetPrefilteredBrdf().GetTexture()->Unbind( index );
			skyboxIbl.GetPrefilteredBrdf().GetSampler()->Unbind( index );
			++index;
			skyboxIbl.GetIrradiance().GetTexture()->Unbind( index );
			skyboxIbl.GetIrradiance().GetSampler()->Unbind( index );
			skyboxIbl.GetPrefilteredEnvironment().GetTexture()->Unbind( index + c_pbrEnvironmentCount );
			skyboxIbl.GetPrefilteredEnvironment().GetSampler()->Unbind( index + c_pbrEnvironmentCount );
			++index;

			for ( auto & map : maps )
			{
				auto & ibl = map.get().GetIbl();
				ibl.GetIrradiance().GetTexture()->Unbind( index );
				ibl.GetIrradiance().GetSampler()->Unbind( index );
				ibl.GetPrefilteredEnvironment().GetTexture()->Unbind( index + c_pbrEnvironmentCount );
				ibl.GetPrefilteredEnvironment().GetSampler()->Unbind( index + c_pbrEnvironmentCount );
				++index;
			}
		}
		else
		{
			auto index = c_environmentStart;

			for ( auto & map : maps )
			{
				map.get().GetTexture().GetTexture()->Bind( index );
				map.get().GetTexture().GetSampler()->Bind( index );
				++index;
			}

			m_programs[0].Render();
			index = c_environmentStart;

			for ( auto & map : maps )
			{
				map.get().GetTexture().GetTexture()->Unbind( index );
				map.get().GetTexture().GetSampler()->Unbind( index );
				++index;
			}
		}

		p_lp.GetTexture()->Unbind( 5u );
		p_lp.GetSampler()->Unbind( 5u );
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Unbind( 4u );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Unbind( 4u );
		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Unbind( 3u );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Unbind( 3u );
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Unbind( 2u );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Unbind( 2u );
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Unbind( 1u );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Unbind( 1u );
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( 0u );
		m_frameBuffer->Unbind();
	}
}

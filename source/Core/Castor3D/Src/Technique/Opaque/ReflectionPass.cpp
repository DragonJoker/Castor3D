#include "ReflectionPass.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslPhongReflection.hpp"
#include "Shader/Shaders/GlslSssTransmittance.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <Graphics/Image.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr c_environmentCount = 16u;

		VertexBufferSPtr doCreateVbo( Engine & engine )
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

			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			vertexBuffer->resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->getData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		GeometryBuffersSPtr doCreateVao( Engine & engine
			, ShaderProgram & program
			, VertexBuffer & vbo )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto result = renderSystem.createGeometryBuffers( Topology::eTriangles
				, program );
			result->initialise( { vbo }, nullptr );
			return result;
		}

		glsl::Shader doCreateVertexProgram( RenderSystem & renderSystem )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_projection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader doCreateLegacyPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto index = MinTextureIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++ );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++ );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++ );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++ );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++ );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++ );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++ );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++ );
			auto c3d_mapEnvironment = writer.declSampler< SamplerCube >( ShaderProgram::MapEnvironment, index++, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			shader::LegacyMaterials materials{ writer };
			materials.declare();

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			shader::PhongReflectionModel reflections{ writer };
			declareDecodeMaterial( writer );
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, vtx_texture ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, vtx_texture ).xyz() );
				auto depth = writer.declLocale( cuT( "depth" )
					, texture( c3d_mapDepth, vtx_texture, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, depth, c3d_mtxInvViewProj ) );
				auto normal = writer.declLocale( cuT( "normal" )
					, normalize( data1.xyz() ) );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, data2.xyz() );
				auto occlusion = writer.declLocale( cuT( "occlusion" )
					, data3.a() );
				auto emissive = writer.declLocale( cuT( "emissive" )
					, data4.xyz() );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, clamp( c3d_ambientLight.xyz() + material.m_ambient() * material.m_diffuse()
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );

				if ( hasSsao )
				{
					occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
				}

				IF( writer, envMapIndex > 0_i && writer.paren( reflection != 0_i || refraction != 0_i ) )
				{
					auto incident = writer.declLocale( cuT( "incident" )
						, reflections.computeIncident( position, c3d_cameraPosition ) );
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i && refraction != 0_i )
					{
						ambient = reflections.computeReflRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex]
							, material.m_refractionRatio()
							, diffuse );
						diffuse = vec3( 0.0_f );
					}
					ELSEIF( writer, reflection != 0_i )
					{
						diffuse = reflections.computeRefl( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex] );
						ambient = vec3( 0.0_f );
					}
					ELSE
					{
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex]
							, material.m_refractionRatio()
							, diffuse );
						diffuse = vec3( 0.0_f );
					}
					FI;
				}
				ELSE
				{
					ambient *= occlusion * diffuse;
					diffuse *= lightDiffuse;
				}
				FI;

				pxl_fragColor = vec4( diffuse + lightSpecular + emissive + ambient, 1.0 );

				if ( fogType != FogType::eDisabled )
				{
					position = utils.calcVSPosition( vtx_texture
							, texture( c3d_mapDepth, vtx_texture ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}
			} );
			return writer.finalise();
		}

		glsl::Shader doCreatePbrMRPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto index = MinTextureIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++ );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++ );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++ );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++ );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++ );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++ );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++ );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++ );
			auto c3d_mapBrdf = writer.declSampler< Sampler2D >( ShaderProgram::MapBrdf, index++ );
			auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( ShaderProgram::MapIrradiance, index++ );
			auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( ShaderProgram::MapPrefiltered, index++ );
			auto c3d_mapEnvironment = writer.declSampler< SamplerCube >( ShaderProgram::MapEnvironment, index++, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			shader::PbrMRMaterials materials{ writer };
			materials.declare();

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeMetallicIBL();
			declareDecodeMaterial( writer );
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );

				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto normal = writer.declLocale( cuT( "normal" )
					, data1.xyz() );
				auto albedo = writer.declLocale( cuT( "albedo" )
					, data2.xyz() );
				auto metalness = writer.declLocale( cuT( "metalness" )
					, data3.r() );
				auto roughness = writer.declLocale( cuT( "roughness" )
					, data3.g() );
				auto occlusion = writer.declLocale( cuT( "occlusion" )
					, data3.a() );
				auto emissive = writer.declLocale( cuT( "emissive" )
					, data4.xyz() );
				auto depth = writer.declLocale( cuT( "depth" )
					, texture( c3d_mapDepth, vtx_texture, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, depth, c3d_mtxInvViewProj ) );
				auto incident = writer.declLocale( cuT( "incident" )
					, normalize( position - c3d_cameraPosition ) );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, c3d_ambientLight.xyz() );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, vtx_texture ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, vtx_texture ).xyz() );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio() );

				if ( hasSsao )
				{
					occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
				}

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i && refraction != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						ambient = mix( texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz() * albedo / length( albedo )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, reflection != 0_i && ratio != 0.0_f )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * albedo / length( albedo )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, reflection != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz()
							* albedo / length( albedo );
					}
					ELSEIF( writer, refraction != 0_i )
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeMetallicIBL( normal
								, position
								, albedo
								, metalness
								, roughness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						ambient = mix( texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz() * albedo / length( albedo )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, ratio != 0.0_f )
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeMetallicIBL( normal
								, position
								, albedo
								, metalness
								, roughness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * albedo / length( albedo )
							, ambient
							, fresnel );
					}
					ELSE
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeMetallicIBL( normal
								, position
								, albedo
								, metalness
								, roughness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
					}
					FI;
				}
				ELSEIF( writer, ratio != 0.0_f )
				{
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeMetallicIBL( normal
							, position
							, albedo
							, metalness
							, roughness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					auto subRatio = writer.declLocale( cuT( "subRatio" )
						, 1.0_f - ratio );
					auto addRatio = writer.declLocale( cuT( "addRatio" )
						, 1.0_f + ratio );
					auto reflectance = writer.declLocale( cuT( "reflectance" )
						, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
					auto product = writer.declLocale( cuT( "product" )
						, max( 0.0_f, dot( -incident, normal ) ) );
					auto fresnel = writer.declLocale( cuT( "fresnel" )
						, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
					auto refracted = writer.declLocale( cuT( "refracted" )
						, refract( incident, normal, ratio ) );
					refracted.y() = -refracted.y();
					ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * albedo / length( albedo )
						, ambient
						, fresnel );
				}
				ELSE
				{
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeMetallicIBL( normal
							, position
							, albedo
							, metalness
							, roughness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
				}
				FI;

#if !C3D_DEBUG_SSS_TRANSMITTANCE
				pxl_fragColor = vec4( lightDiffuse * albedo + lightSpecular + emissive + ambient, 1.0 );
#else
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#endif

				if ( fogType != FogType::eDisabled )
				{
					position = utils.calcVSPosition( vtx_texture
							, texture( c3d_mapDepth, vtx_texture ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}
			} );
			return writer.finalise();
		}

		glsl::Shader doCreatePbrSGPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto index = MinTextureIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++ );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++ );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++ );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++ );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++ );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++ );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++ );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++ );
			auto c3d_mapBrdf = writer.declSampler< Sampler2D >( ShaderProgram::MapBrdf, index++ );
			auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( ShaderProgram::MapIrradiance, index++ );
			auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( ShaderProgram::MapPrefiltered, index++ );
			auto c3d_mapEnvironment = writer.declSampler< SamplerCube >( ShaderProgram::MapEnvironment, index++, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			shader::PbrSGMaterials materials{ writer };
			materials.declare();
			
			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeSpecularIBL();
			declareDecodeMaterial( writer );
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );

				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, vtx_texture ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );
				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, vtx_texture ) );
				auto normal = writer.declLocale( cuT( "normal" )
					, data1.xyz() );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, data2.xyz() );
				auto glossiness = writer.declLocale( cuT( "glossiness" )
					, data2.w() );
				auto specular = writer.declLocale( cuT( "specular" )
					, data3.xyz() );
				auto occlusion = writer.declLocale( cuT( "occlusion" )
					, data3.a() );
				auto emissive = writer.declLocale( cuT( "emissive" )
					, data4.xyz() );
				auto depth = writer.declLocale( cuT( "depth" )
					, texture( c3d_mapDepth, vtx_texture, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, depth, c3d_mtxInvViewProj ) );
				auto incident = writer.declLocale( cuT( "incident" )
					, normalize( position - c3d_cameraPosition ) );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, c3d_ambientLight.xyz() );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, vtx_texture ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, vtx_texture ).xyz() );
				auto ratio = writer.declLocale( cuT( "ratio" )
					, material.m_refractionRatio() );

				if ( hasSsao )
				{
					occlusion *= texture( c3d_mapSsao, vtx_texture ).r();
				}

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i && refraction != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz() * diffuse / length( diffuse );
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						ambient = mix( texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz() * diffuse / length( diffuse )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, reflection != 0_i && ratio != 0.0_f )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz() * diffuse / length( diffuse );
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * diffuse / length( diffuse )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, reflection != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();
					}
					ELSEIF( writer, refraction != 0_i )
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeSpecularIBL( normal
								, position
								, diffuse
								, specular
								, glossiness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						ambient = mix( texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz() * diffuse / length( diffuse )
							, ambient
							, fresnel );
					}
					ELSEIF( writer, ratio != 0.0_f )
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeSpecularIBL( normal
								, position
								, diffuse
								, specular
								, glossiness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * diffuse / length( diffuse )
							, ambient
							, fresnel );
					}
					ELSE
					{
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeSpecularIBL( normal
								, position
								, diffuse
								, specular
								, glossiness
								, c3d_cameraPosition
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
					}
					FI;
				}
				ELSEIF( writer, ratio != 0.0_f )
				{
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeSpecularIBL( normal
							, position
							, diffuse
							, specular
							, glossiness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio() );
					auto roughness = writer.declLocale( cuT( "roughness" )
						, 1.0_f - glossiness );
					auto subRatio = writer.declLocale( cuT( "subRatio" )
						, 1.0_f - ratio );
					auto addRatio = writer.declLocale( cuT( "addRatio" )
						, 1.0_f + ratio );
					auto reflectance = writer.declLocale( cuT( "reflectance" )
						, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
					auto product = writer.declLocale( cuT( "product" )
						, max( 0.0_f, dot( -incident, normal ) ) );
					auto fresnel = writer.declLocale( cuT( "fresnel" )
						, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
					auto refracted = writer.declLocale( cuT( "refract" )
						, refract( incident, normal, ratio ) );
					refracted.y() = writer.ternary( envMapIndex != 0_i, refracted.y(), -refracted.y() );
					ambient = mix( texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz() * diffuse / length( diffuse )
						, ambient
						, fresnel );
				}
				ELSE
				{
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeSpecularIBL( normal
							, position
							, diffuse
							, specular
							, glossiness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio() );
				}
				FI;

#if !C3D_DEBUG_SSS_TRANSMITTANCE
				pxl_fragColor = vec4( lightDiffuse * diffuse + lightSpecular + emissive + ambient, 1.0 );
#else
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#endif

				if ( fogType != FogType::eDisabled )
				{
					position = utils.calcVSPosition( vtx_texture
							, texture( c3d_mapDepth, vtx_texture ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}
			} );
			return writer.finalise();
		}

		ShaderProgramSPtr doCreateProgram( Engine & engine
			, FogType fogType
			, bool hasSsao
			, MaterialType matType )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto vtx = doCreateVertexProgram( renderSystem );
			auto pxl = matType == MaterialType::eLegacy
				? doCreateLegacyPixelProgram( renderSystem, fogType, hasSsao )
				: matType == MaterialType::ePbrMetallicRoughness
					? doCreatePbrMRPixelProgram( renderSystem, fogType, hasSsao )
					: doCreatePbrSGPixelProgram( renderSystem, fogType, hasSsao );
			auto result = engine.getShaderProgramCache().getNewProgram( false );
			result->createObject( ShaderType::eVertex );
			result->createObject( ShaderType::ePixel );
			result->setSource( ShaderType::eVertex, vtx );
			result->setSource( ShaderType::ePixel, pxl );
			result->initialise();
			return result;
		}

		RenderPipelineUPtr doCreateRenderPipeline( Engine & engine
			, ShaderProgram & program
			, MatrixUbo & matrixUbo
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo )
		{
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eNone );
			DepthStencilState dsState;
			dsState.setDepthTest( false );
			dsState.setDepthMask( WritingMask::eZero );
			BlendState blState;
			auto result = engine.getRenderSystem()->createRenderPipeline( std::move( dsState )
				, std::move( rsState )
				, std::move( blState )
				, MultisampleState{}
				, program
				, PipelineFlags{} );
			result->addUniformBuffer( matrixUbo.getUbo() );
			result->addUniformBuffer( sceneUbo.getUbo() );
			result->addUniformBuffer( gpInfoUbo.getUbo() );
			return result;
		}

		template< typename T >
		struct Reverse
		{
			explicit Reverse( T const & collection )
				: m_collection{ collection }
			{
			}

			auto begin()const
			{
				return m_collection.rbegin();
			}

			auto end()const
			{
				return m_collection.rend();
			}

		private:
			T const & m_collection;
		};

		template< typename T >
		Reverse< T > makeReverse( T const & collection )
		{
			return Reverse< T >( collection );
		}
	}

	ReflectionPass::ProgramPipeline::ProgramPipeline( Engine & engine
		, VertexBuffer & vbo
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, bool hasSsao
		, FogType fogType
		, MaterialType matType )
		: m_program{ doCreateProgram( engine, fogType, hasSsao, matType ) }
		, m_geometryBuffers{ doCreateVao( engine, *m_program, vbo ) }
		, m_pipeline{ doCreateRenderPipeline( engine, *m_program, matrixUbo, sceneUbo, gpInfoUbo ) }
	{
	}

	ReflectionPass::ProgramPipeline::~ProgramPipeline()
	{
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void ReflectionPass::ProgramPipeline::render( VertexBuffer const & vbo )
	{
		m_pipeline->apply();
		m_geometryBuffers->draw( 6u, 0u );
	}

	ReflectionPass::ReflectionPass( Engine & engine
		, Size const & size
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, SsaoConfig const & config )
		: OwnedBy< Engine >{ engine }
		, m_size{ size }
		, m_viewport{ engine }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_matrixUbo{ engine }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_programs
		{
			{
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eDisabled, MaterialType::eLegacy },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eLinear, MaterialType::eLegacy },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eExponential, MaterialType::eLegacy },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eSquaredExponential, MaterialType::eLegacy },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eDisabled, MaterialType::ePbrMetallicRoughness },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eLinear, MaterialType::ePbrMetallicRoughness },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eExponential, MaterialType::ePbrMetallicRoughness },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eSquaredExponential, MaterialType::ePbrMetallicRoughness },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eDisabled, MaterialType::ePbrSpecularGlossiness },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eLinear, MaterialType::ePbrSpecularGlossiness },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eExponential, MaterialType::ePbrSpecularGlossiness },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, config.m_enabled, FogType::eSquaredExponential, MaterialType::ePbrSpecularGlossiness },
			}
		}
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Reflection" ), cuT( "Reflection" ) ) }
		, m_ssaoEnabled{ config.m_enabled }
		, m_ssao{ engine, size, config }
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.initialise();
		m_viewport.resize( m_size );
		m_viewport.update();
		m_matrixUbo.update( m_viewport.getProjection() );
	}

	ReflectionPass::~ReflectionPass()
	{
		m_matrixUbo.getUbo().cleanup();
		m_vertexBuffer->cleanup();
	}

	void ReflectionPass::render( GeometryPassResult & gp
		, TextureUnit const & lightDiffuse
		, TextureUnit const & lightSpecular
		, Scene const & scene
		, Camera const & camera
		, FrameBuffer const & frameBuffer
		, RenderInfo & info )
	{
		TextureUnit const * ssao = nullptr;

		if ( m_ssaoEnabled )
		{
			m_ssao.render( gp
				, camera
				, info );
			ssao = &m_ssao.getResult();
		}

		m_timer->start();
		frameBuffer.bind( FrameBufferTarget::eDraw );
		frameBuffer.setDrawBuffers();
		m_viewport.apply();

		auto & maps = scene.getEnvironmentMaps();

		auto index = MinTextureIndex;
		gp[size_t( DsTexture::eDepth )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->bind( index++ );
		gp[size_t( DsTexture::eData1 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData1 )]->getSampler()->bind( index++ );
		gp[size_t( DsTexture::eData2 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData2 )]->getSampler()->bind( index++ );
		gp[size_t( DsTexture::eData3 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData3 )]->getSampler()->bind( index++ );
		gp[size_t( DsTexture::eData4 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->bind( index++ );
		gp[size_t( DsTexture::eData5 )]->getTexture()->bind( index );
		gp[size_t( DsTexture::eData5 )]->getSampler()->bind( index++ );

		if ( m_ssaoEnabled )
		{
			ssao->getSampler()->bind( index );
			ssao->getTexture()->bind( index++ );
		}

		lightDiffuse.getTexture()->bind( index );
		lightDiffuse.getSampler()->bind( index++ );
		lightSpecular.getTexture()->bind( index );
		lightSpecular.getSampler()->bind( index++ );
		auto program = size_t( scene.getMaterialsType() )
			* size_t( FogType::eCount )
			+ size_t( scene.getFog().getType() );

		if ( scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness
			|| scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
		{
			auto & skyboxIbl = scene.getSkybox().getIbl();
			skyboxIbl.getPrefilteredBrdf().getTexture()->bind( index );
			skyboxIbl.getPrefilteredBrdf().getSampler()->bind( index++ );
			skyboxIbl.getIrradiance().getTexture()->bind( index );
			skyboxIbl.getIrradiance().getSampler()->bind( index++ );
			skyboxIbl.getPrefilteredEnvironment().getTexture()->bind( index );
			skyboxIbl.getPrefilteredEnvironment().getSampler()->bind( index++ );

			for ( auto & map : maps )
			{
				map.get().getTexture().getTexture()->bind( index );
				map.get().getTexture().getSampler()->bind( index++ );
			}

			m_programs[program].render( *m_vertexBuffer );

			for ( auto & map : makeReverse( maps ) )
			{
				map.get().getTexture().getTexture()->unbind( --index );
				map.get().getTexture().getSampler()->unbind( index );
			}

			skyboxIbl.getPrefilteredBrdf().getTexture()->unbind( --index );
			skyboxIbl.getPrefilteredBrdf().getSampler()->unbind( index );
			skyboxIbl.getIrradiance().getTexture()->unbind( --index );
			skyboxIbl.getIrradiance().getSampler()->unbind( index );
			skyboxIbl.getPrefilteredEnvironment().getTexture()->unbind( --index );
			skyboxIbl.getPrefilteredEnvironment().getSampler()->unbind( index );
		}
		else
		{
			for ( auto & map : maps )
			{
				map.get().getTexture().getTexture()->bind( index );
				map.get().getTexture().getSampler()->bind( index++ );
			}

			m_programs[program].render( *m_vertexBuffer );

			for ( auto & map : makeReverse( maps ) )
			{
				map.get().getTexture().getTexture()->unbind( --index );
				map.get().getTexture().getSampler()->unbind( index );
			}
		}

		lightSpecular.getTexture()->bind( --index );
		lightSpecular.getSampler()->bind( index );
		lightDiffuse.getTexture()->bind( --index );
		lightDiffuse.getSampler()->bind( index );

		if ( m_ssaoEnabled )
		{
			ssao->getSampler()->bind( --index );
			ssao->getTexture()->bind( index );
		}

		gp[size_t( DsTexture::eData5 )]->getTexture()->unbind( --index );
		gp[size_t( DsTexture::eData5 )]->getSampler()->unbind( index );
		gp[size_t( DsTexture::eData4 )]->getTexture()->unbind( --index );
		gp[size_t( DsTexture::eData4 )]->getSampler()->unbind( index );
		gp[size_t( DsTexture::eData3 )]->getTexture()->unbind( --index );
		gp[size_t( DsTexture::eData3 )]->getSampler()->unbind( index );
		gp[size_t( DsTexture::eDepth )]->getTexture()->unbind( --index );
		gp[size_t( DsTexture::eDepth )]->getSampler()->unbind( index );
		gp[size_t( DsTexture::eData2 )]->getTexture()->unbind( --index );
		gp[size_t( DsTexture::eData2 )]->getSampler()->unbind( index );
		gp[size_t( DsTexture::eData1 )]->getTexture()->unbind( --index );
		gp[size_t( DsTexture::eData1 )]->getSampler()->unbind( index );
		frameBuffer.unbind();

		m_timer->stop();
	}
}

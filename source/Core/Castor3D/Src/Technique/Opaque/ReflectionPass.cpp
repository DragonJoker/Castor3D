#include "ReflectionPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Background.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslMetallicPbrReflection.hpp"
#include "Shader/Shaders/GlslPhongReflection.hpp"
#include "Shader/Shaders/GlslSpecularPbrReflection.hpp"
#include "Shader/Shaders/GlslSssTransmittance.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Technique/Opaque/GeometryPassResult.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/GlslToSpv.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <Graphics/Image.hpp>

using namespace castor;
using namespace castor3d;

#define C3D_DebugSSAO 0
#define C3D_DebugDiffuseLighting 0
#define C3D_DebugSpecularLighting 0

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr c_environmentCount = 16u;

		ashes::VertexLayoutPtr doCreateVertexLayout()
		{
			auto result = std::make_unique< ashes::VertexLayout >( 0u, 16u, ashes::VertexInputRate::eVertex );
			result->createAttribute( 0u, ashes::Format::eR32G32_SFLOAT, 0u );
			result->createAttribute( 1u, ashes::Format::eR32G32_SFLOAT, 8u );
			return result;
		}

		ashes::VertexBufferBasePtr doCreateVbo( Engine & engine )
		{
			float data[]
			{
				-1, -1, 0, 0,
				+1, +1, 1, 1,
				-1, +1, 0, 1,
				-1, -1, 0, 0,
				+1, -1, 1, 0,
				+1, +1, 1, 1
			};

			auto vertexBuffer = std::make_unique< ashes::VertexBufferBase >( getCurrentDevice( engine )
				, uint32_t( sizeof( data ) )
				, 0u
				, ashes::MemoryPropertyFlag::eHostVisible );
			if ( auto * buffer = vertexBuffer->getBuffer().lock( 0u, vertexBuffer->getSize(), ashes::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, data, sizeof( data ) );
				vertexBuffer->getBuffer().flush( 0u, vertexBuffer->getSize() );
				vertexBuffer->getBuffer().unlock();
			}

			return vertexBuffer;
		}

		glsl::Shader doCreateVertexProgram( RenderSystem & renderSystem )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0 );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1 );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				vtx_texture = texture;
				out.gl_Position() = vec4( position.x(), position.y(), 0.0, 1.0 );
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
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			uint32_t index = MinBufferIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSamplerArray< SamplerCube >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::LegacyMaterials materials{ writer };
			materials.declare();

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			shader::PhongReflectionModel reflections{ writer };
			declareDecodeMaterial( writer );
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto lightResultTexcoord = writer.declLocale( cuT( "lightResultTexcoord" )
					, vtx_texture );
				auto texcoord = writer.declLocale( cuT( "texcoord" )
					, lightResultTexcoord );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, texcoord ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );

				IF( writer, materialId == 0_i )
				{
					writer.discard();
				}
				FI;

				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, texcoord ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, texcoord ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, texcoord ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, texcoord ) );
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
				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, lightResultTexcoord ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, lightResultTexcoord ).xyz() );
				auto depth = writer.declLocale( cuT( "depth" )
					, texture( c3d_mapDepth, texcoord, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( texcoord, depth, c3d_mtxInvViewProj ) );
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
					occlusion *= texture( c3d_mapSsao, texcoord ).r();
				}

				IF( writer, envMapIndex > 0_i && writer.paren( reflection != 0_i || refraction != 0_i ) )
				{
					auto incident = writer.declLocale( cuT( "incident" )
						, reflections.computeIncident( position, c3d_cameraPosition.xyz() ) );
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
						diffuse *= reflections.computeRefl( incident
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
					position = utils.calcVSPosition( texcoord
							, texture( c3d_mapDepth, texcoord ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}

#if C3D_DebugDiffuseLighting
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#elif C3D_DebugSpecularLighting
				pxl_fragColor = vec4( lightSpecular, 1.0 );
#elif C3D_DebugSSAO
				pxl_fragColor = vec4( vec3( occlusion ), 1.0 );
#elif C3D_DebugSSSTransmittance
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#endif
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
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			auto index = MinBufferIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapBrdf = writer.declSampler< Sampler2D >( cuT( "c3d_mapBrdf" ), index++, 1u );
			auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( cuT( "c3d_mapIrradiance" ), index++, 1u );
			auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( cuT( "c3d_mapPrefiltered" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSamplerArray< SamplerCube >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::PbrMRMaterials materials{ writer };
			materials.declare();

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeIBL();
			declareDecodeMaterial( writer );
			shader::MetallicPbrReflectionModel reflections{ writer };
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto lightResultTexcoord = writer.declLocale( cuT( "lightResultTexcoord" )
					, vtx_texture );
				auto texcoord = writer.declLocale( cuT( "texcoord" )
					, lightResultTexcoord );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, texcoord ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );

				IF( writer, materialId == 0_i )
				{
					writer.discard();
				}
				FI;

				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, texcoord ) );
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

				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, texcoord ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, texcoord ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, texcoord ) );
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
					, texture( c3d_mapDepth, texcoord, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( texcoord, depth, c3d_mtxInvViewProj ) );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, c3d_ambientLight.xyz() );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, lightResultTexcoord ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, lightResultTexcoord ).xyz() );

				if ( hasSsao )
				{
					occlusion *= texture( c3d_mapSsao, texcoord ).r();
				}

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;
					auto incident = writer.declLocale( cuT( "incident" )
						, normalize( position - c3d_cameraPosition.xyz() ) );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio() );

					IF( writer, reflection != 0_i )
					{
						// Reflection from environment map.
						ambient = reflections.computeRefl( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex]
							, c3d_ambientLight.xyz()
							, albedo );
					}
					ELSE
					{
						// Reflection from background skybox.
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeMetallicIBL( normal
								, position
								, albedo
								, metalness
								, roughness
								, c3d_cameraPosition.xyz()
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
					}
					FI;

					IF ( writer, refraction != 0_i )
					{
						// Refraction from environment map.
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex]
							, material.m_refractionRatio()
							, ambient
							, albedo
							, roughness );
					}
					ELSEIF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio()
							, ambient
							, albedo
							, roughness );
					}
					FI;
				}
				ELSE
				{
					// Reflection from background skybox.
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeMetallicIBL( normal
							, position
							, albedo
							, metalness
							, roughness
							, c3d_cameraPosition.xyz()
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio() );

					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						auto incident = writer.declLocale( cuT( "incident" )
							, normalize( position - c3d_cameraPosition.xyz() ) );
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio()
							, ambient
							, albedo
							, roughness );
					}
					FI;
				}
				FI;

				pxl_fragColor = vec4( lightDiffuse * albedo + lightSpecular + emissive + ambient, 1.0 );

				if ( fogType != FogType::eDisabled )
				{
					position = utils.calcVSPosition( texcoord
							, texture( c3d_mapDepth, texcoord ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}

#if C3D_DebugDiffuseLighting
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#elif C3D_DebugSpecularLighting
				pxl_fragColor = vec4( lightSpecular, 1.0 );
#elif C3D_DebugSSAO
				pxl_fragColor = vec4( vec3( occlusion ), 1.0 );
#elif C3D_DebugSSSTransmittance
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#endif
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
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			auto index = MinBufferIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampler< Sampler2D >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampler< Sampler2D >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampler< Sampler2D >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapBrdf = writer.declSampler< Sampler2D >( cuT( "c3d_mapBrdf" ), index++, 1u );
			auto c3d_mapIrradiance = writer.declSampler< SamplerCube >( cuT( "c3d_mapIrradiance" ), index++, 1u );
			auto c3d_mapPrefiltered = writer.declSampler< SamplerCube >( cuT( "c3d_mapPrefiltered" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSamplerArray< SamplerCube >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::PbrSGMaterials materials{ writer };
			materials.declare();
			
			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeIBL();
			declareDecodeMaterial( writer );
			shader::SpecularPbrReflectionModel reflections{ writer };
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto lightResultTexcoord = writer.declLocale( cuT( "lightResultTexcoord" )
					, vtx_texture );
				auto texcoord = writer.declLocale( cuT( "texcoord" )
					, lightResultTexcoord );
				auto data5 = writer.declLocale( cuT( "data5" )
					, texture( c3d_mapData5, texcoord ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );

				IF( writer, materialId == 0_i )
				{
					writer.discard();
				}
				FI;

				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, texcoord ) );
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

				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, texcoord ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, texcoord ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, texture( c3d_mapData4, texcoord ) );
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
					, texture( c3d_mapDepth, texcoord, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( texcoord, depth, c3d_mtxInvViewProj ) );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, c3d_ambientLight.xyz() );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, texture( c3d_mapLightDiffuse, lightResultTexcoord ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, texture( c3d_mapLightSpecular, lightResultTexcoord ).xyz() );

				if ( hasSsao )
				{
					occlusion *= texture( c3d_mapSsao, texcoord ).r();
				}

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;
					auto incident = writer.declLocale( cuT( "incident" )
						, normalize( position - c3d_cameraPosition.xyz() ) );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio() );

					IF( writer, reflection != 0_i )
					{
						// Reflection from environment map.
						ambient = reflections.computeRefl( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex]
							, c3d_ambientLight.xyz()
							, diffuse );
					}
					ELSE
					{
						// Reflection from background skybox.
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeSpecularIBL( normal
								, position
								, diffuse
								, specular
								, glossiness
								, c3d_cameraPosition.xyz()
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
					}
					FI;

					IF ( writer, refraction != 0_i )
					{
						// Refraction from environment map.
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment[envMapIndex]
							, material.m_refractionRatio()
							, ambient
							, diffuse
							, glossiness );
					}
					ELSEIF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio()
							, ambient
							, diffuse
							, glossiness );
					}
					FI;
				}
				ELSE
				{
					// Reflection from background skybox.
					ambient = c3d_ambientLight.xyz()
						* occlusion
						* utils.computeSpecularIBL( normal
							, position
							, diffuse
							, specular
							, glossiness
							, c3d_cameraPosition.xyz()
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio() );

					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						auto incident = writer.declLocale( cuT( "incident" )
							, normalize( position - c3d_cameraPosition.xyz() ) );
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio()
							, ambient
							, diffuse
							, glossiness );
					}
					FI;
				}
				FI;

				pxl_fragColor = vec4( lightDiffuse * diffuse + lightSpecular + emissive + ambient, 1.0 );

				if ( fogType != FogType::eDisabled )
				{
					position = utils.calcVSPosition( texcoord
							, texture( c3d_mapDepth, texcoord ).r()
							, c3d_mtxInvProj );
					fog.applyFog( pxl_fragColor, length( position ), position.z() );
				}

#if C3D_DebugDiffuseLighting
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#elif C3D_DebugSpecularLighting
				pxl_fragColor = vec4( lightSpecular, 1.0 );
#elif C3D_DebugSSAO
				pxl_fragColor = vec4( vec3( occlusion ), 1.0 );
#elif C3D_DebugSSSTransmittance
				pxl_fragColor = vec4( lightDiffuse, 1.0 );
#endif
			} );
			return writer.finalise();
		}

		ashes::ShaderStageStateArray doCreateProgram( Engine & engine
			, FogType fogType
			, bool hasSsao
			, MaterialType matType
			, glsl::Shader & vertexShader
			, glsl::Shader & pixelShader )
		{
			auto & renderSystem = *engine.getRenderSystem();
			vertexShader = doCreateVertexProgram( renderSystem );
			pixelShader = matType == MaterialType::eLegacy
				? doCreateLegacyPixelProgram( renderSystem, fogType, hasSsao )
				: matType == MaterialType::ePbrMetallicRoughness
					? doCreatePbrMRPixelProgram( renderSystem, fogType, hasSsao )
					: doCreatePbrSGPixelProgram( renderSystem, fogType, hasSsao );

			auto & device = getCurrentDevice( renderSystem );
			ashes::ShaderStageStateArray result
			{
				{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) }
			};
			result[0].module->loadShader( compileGlslToSpv( device
				, ashes::ShaderStageFlag::eVertex
				, vertexShader.getSource() ) );
			result[1].module->loadShader( compileGlslToSpv( device
				, ashes::ShaderStageFlag::eFragment
				, pixelShader.getSource() ) );
			return result;
		}

		inline ashes::DescriptorSetLayoutPtr doCreateUboDescriptorLayout( Engine & engine )
		{
			auto & passBuffer = engine.getMaterialCache().getPassBuffer();
			ashes::DescriptorSetLayoutBindingArray bindings
			{
				passBuffer.createLayoutBinding(),
				{ SceneUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
				{ GpInfoUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
			};
			return getCurrentDevice( engine ).createDescriptorSetLayout( std::move( bindings ) );
		}

		inline ashes::DescriptorSetPtr doCreateUboDescriptorSet( Engine & engine
			, ashes::DescriptorSetPool & pool
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo )
		{
			auto & passBuffer = engine.getMaterialCache().getPassBuffer();
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 0u );
			passBuffer.createBinding( *result, layout.getBinding( PassBufferIndex ) );
			result->createBinding( layout.getBinding( SceneUbo::BindingPoint )
				, sceneUbo.getUbo() );
			result->createBinding( layout.getBinding( GpInfoUbo::BindingPoint )
				, gpInfoUbo.getUbo() );
			result->update();
			return result;
		}

		inline ashes::RenderPassPtr doCreateRenderPass( Engine & engine
			, ashes::Format format )
		{
			ashes::RenderPassCreateInfo renderPass{};
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eColourAttachmentOptimal;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

			ashes::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = ashes::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].colorAttachments = { colourReference };

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return getCurrentDevice( engine ).createRenderPass( renderPass );
		}

		inline ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, ashes::Extent2D const size
			, ashes::TextureView const & view )
		{
			ashes::FrameBufferAttachmentArray attaches
			{
				{ *renderPass.getAttachments().begin(), view }
			};
			return renderPass.createFrameBuffer( size, std::move( attaches ) );
		}

		inline ashes::DescriptorSetLayoutPtr doCreateTexDescriptorLayout( Engine & engine
			, bool hasSsao
			, MaterialType matType )
		{
			uint32_t index = MinBufferIndex;
			ashes::DescriptorSetLayoutBindingArray bindings
			{
				{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// DsTexture::eDepth
				{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// DsTexture::eData1
				{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// DsTexture::eData2
				{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// DsTexture::eData3
				{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// DsTexture::eData4
				{ index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },	// DsTexture::eData5
			};

			if ( hasSsao )
			{
				bindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapSsao
			}

			bindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapLightDiffuse
			bindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapLightSpecular

			if ( matType != MaterialType::eLegacy )
			{
				bindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapBrdf
				bindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapIrradiance
				bindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );	// c3d_mapPrefiltered
			}

			bindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment, c_environmentCount );	// c3d_mapEnvironment
			return getCurrentDevice( engine ).createDescriptorSetLayout( std::move( bindings ) );
		}

		inline ashes::DescriptorSetPtr doCreateTexDescriptorSet( ashes::DescriptorSetPool & pool
			, SamplerSPtr sampler )
		{
			sampler->initialise();
			return pool.createDescriptorSet( 1u );
		}

		inline ashes::WriteDescriptorSetArray doCreateTexDescriptorWrites( ashes::DescriptorSetLayout const & layout
			, GeometryPassResult const & gp
			, ashes::TextureView const & lightDiffuse
			, ashes::TextureView const & lightSpecular
			, ashes::TextureView const * ssao
			, SamplerSPtr sampler
			, Scene const & scene
			, MaterialType matType
			, std::vector< std::reference_wrapper< EnvironmentMap > > const & envMaps )
		{
			uint32_t index = MinBufferIndex;
			auto imgLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;
			ashes::WriteDescriptorSetArray result;
			result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { gp.getSampler(), *gp.getViews()[size_t( DsTexture::eDepth )], imgLayout } } } );
			result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { gp.getSampler(), *gp.getViews()[size_t( DsTexture::eData1 )], imgLayout } } } );
			result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { gp.getSampler(), *gp.getViews()[size_t( DsTexture::eData2 )], imgLayout } } } );
			result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { gp.getSampler(), *gp.getViews()[size_t( DsTexture::eData3 )], imgLayout } } } );
			result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { gp.getSampler(), *gp.getViews()[size_t( DsTexture::eData4 )], imgLayout } } } );
			result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { gp.getSampler(), *gp.getViews()[size_t( DsTexture::eData5 )], imgLayout } } } );

			if ( ssao )
			{
				result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { sampler->getSampler(), *ssao, imgLayout } } } );
			}

			result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { sampler->getSampler(), lightDiffuse, imgLayout } } } );
			result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { sampler->getSampler(), lightSpecular, imgLayout } } } );
			auto & background = *scene.getBackground();

			if ( matType != MaterialType::eLegacy
				&& background.hasIbl() )
			{
				auto & ibl = background.getIbl();
				result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { ibl.getPrefilteredBrdfSampler(), ibl.getPrefilteredBrdfTexture(), imgLayout } } } );
				result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { ibl.getIrradianceSampler(), ibl.getIrradianceTexture(), imgLayout } } } );
				result.push_back( { index++, 0u, 1u, ashes::DescriptorType::eCombinedImageSampler, { { ibl.getPrefilteredEnvironmentSampler(), ibl.getPrefilteredEnvironmentTexture(), imgLayout } } } );
			}

			result.push_back( { index++, 0u, c_environmentCount, ashes::DescriptorType::eCombinedImageSampler } );
			result.back().imageInfo.reserve( c_environmentCount );

			auto & envWrites = result.back();
			auto it = envMaps.begin();
			uint32_t i = 0u;

			while ( it != envMaps.end() && i < c_environmentCount )
			{
				envWrites.imageInfo.push_back( { it->get().getTexture().getSampler()->getSampler()
					, it->get().getTexture().getTexture()->getDefaultView()
					, ashes::ImageLayout::eShaderReadOnlyOptimal } );
				++i;
				++it;
			}

			while ( i < c_environmentCount )
			{
				envWrites.imageInfo.push_back( { sampler->getSampler()
					, background.getView()
					, ashes::ImageLayout::eShaderReadOnlyOptimal } );
				++i;
			}

			return result;
		}

		ashes::PipelinePtr doCreateRenderPipeline( ashes::PipelineLayout const & pipelineLayout
			, ashes::ShaderStageStateArray program
			, ashes::RenderPass const & renderPass
			, ashes::Extent2D const & size )
		{
			ashes::RasterisationState rsState;
			rsState.cullMode = ashes::CullModeFlag::eNone;
			ashes::DepthStencilState dsState{ 0u, false, false };
			ashes::ColourBlendState blState = ashes::ColourBlendState::createDefault();
			return pipelineLayout.createPipeline(
			{
				program,
				renderPass,
				ashes::VertexInputState::create( *doCreateVertexLayout() ),
				ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
				rsState,
				ashes::MultisampleState{},
				blState,
				{},
				dsState,
				std::nullopt,
				ashes::Viewport{ size.width, size.height, 0, 0 },
				ashes::Scissor{ 0, 0, size.width, size.height }
			} );
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

	//*********************************************************************************************

	ReflectionPass::ProgramPipeline::ProgramPipeline( Engine & engine
		, ashes::DescriptorSetLayout const & uboLayout
		, ashes::DescriptorSetLayout const & texLayout
		, ashes::RenderPass const & renderPass
		, ashes::TextureView const * ssao
		, ashes::Extent2D const & size
		, FogType fogType
		, MaterialType matType )
		: m_program{ doCreateProgram( engine, fogType, ssao != nullptr, matType, m_vertexShader, m_pixelShader ) }
		, m_pipelineLayout{ getCurrentDevice( engine ).createPipelineLayout( { uboLayout, texLayout } ) }
		, m_pipeline{ doCreateRenderPipeline( *m_pipelineLayout, m_program, renderPass, size ) }
		, m_commandBuffer{ getCurrentDevice( engine ).getGraphicsCommandPool().createCommandBuffer( true ) }
		, m_renderPass{ &renderPass }
	{
	}

	void ReflectionPass::ProgramPipeline::updateCommandBuffer( ashes::VertexBufferBase & vbo
		, ashes::DescriptorSet const & uboSet
		, ashes::DescriptorSet const & texSet
		, ashes::FrameBuffer const & frameBuffer
		, RenderPassTimer & timer )
	{
		static ashes::ClearColorValue const clear{ 0.0, 0.0, 0.0, 0.0 };

		m_commandBuffer->begin();
		timer.beginPass( *m_commandBuffer );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, frameBuffer
			, { clear }
			, ashes::SubpassContents::eInline );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->bindDescriptorSets( { uboSet, texSet }, *m_pipelineLayout );
		m_commandBuffer->bindVertexBuffer( 0u, vbo.getBuffer(), 0u );
		m_commandBuffer->draw( 6u );
		m_commandBuffer->endRenderPass();
		timer.endPass( *m_commandBuffer );
		m_commandBuffer->end();
	}

	void ReflectionPass::ProgramPipeline::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "Reflection" )
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "Reflection" )
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader );
	}

	//*********************************************************************************************

	ReflectionPass::ReflectionPass( Engine & engine
		, Scene & scene
		, GeometryPassResult const & gp
		, ashes::TextureView const & lightDiffuse
		, ashes::TextureView const & lightSpecular
		, ashes::TextureView const & result
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, ashes::TextureView const * ssao )
		: OwnedBy< Engine >{ engine }
		, m_device{ getCurrentDevice( engine ) }
		, m_scene{ scene }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_ssaoResult{ ssao }
		, m_size{ result.getTexture().getDimensions().width, result.getTexture().getDimensions().height }
		, m_sampler{ engine.getDefaultSampler() }
		, m_geometryPassResult{ gp }
		, m_lightDiffuse{ lightDiffuse }
		, m_lightSpecular{ lightSpecular }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_uboDescriptorLayout{ doCreateUboDescriptorLayout( engine ) }
		, m_uboDescriptorPool{ m_uboDescriptorLayout->createPool( 1u ) }
		, m_uboDescriptorSet{ doCreateUboDescriptorSet( engine, *m_uboDescriptorPool, sceneUbo, gpInfoUbo ) }
		, m_texDescriptorLayout{ doCreateTexDescriptorLayout( engine, m_ssaoResult != nullptr, engine.getMaterialsType() ) }
		, m_texDescriptorPool{ m_texDescriptorLayout->createPool( 1u ) }
		, m_texDescriptorSet{ doCreateTexDescriptorSet( *m_texDescriptorPool, m_sampler ) }
		, m_renderPass{ doCreateRenderPass( engine, result.getFormat() ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_size, result ) }
		, m_finished{ m_device.createSemaphore() }
		, m_fence{ m_device.createFence( ashes::FenceCreateFlag::eSignaled ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Opaque" ), cuT( "Reflection pass" ) ) }
		, m_programs
		{
			{
				ProgramPipeline
				{
					engine,
					*m_uboDescriptorLayout,
					*m_texDescriptorLayout,
					*m_renderPass,
					m_ssaoResult,
					m_size,
					FogType::eDisabled,
					engine.getMaterialsType(),
				},
				ProgramPipeline
				{
					engine,
					*m_uboDescriptorLayout,
					*m_texDescriptorLayout,
					*m_renderPass,
					m_ssaoResult,
					m_size,
					FogType::eLinear,
					engine.getMaterialsType(),
				},
				ProgramPipeline
				{
					engine,
					*m_uboDescriptorLayout,
					*m_texDescriptorLayout,
					*m_renderPass,
					m_ssaoResult,
					m_size,
					FogType::eExponential,
					engine.getMaterialsType(),
				},
				ProgramPipeline
				{
					engine,
					*m_uboDescriptorLayout,
					*m_texDescriptorLayout,
					*m_renderPass,
					m_ssaoResult,
					m_size,
					FogType::eSquaredExponential,
					engine.getMaterialsType(),
				},
			}
		}
		, m_ssaoEnabled{ ssao != nullptr }
		, m_viewport{ engine }
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.resize( { m_size.width, m_size.height } );
		m_viewport.update();
	}

	void ReflectionPass::update( Camera const & camera )
	{
		auto index = size_t( m_scene.getFog().getType() );
		auto & program = m_programs[index];
		auto texDescriptorWrites = doCreateTexDescriptorWrites( *m_texDescriptorLayout
			, m_geometryPassResult
			, m_lightDiffuse
			, m_lightSpecular
			, m_ssaoResult
			, m_sampler
			, m_scene
			, getEngine()->getMaterialsType()
			, m_scene.getEnvironmentMaps() );
		m_texDescriptorSet->setBindings( texDescriptorWrites );
		m_texDescriptorSet->update();
		program.updateCommandBuffer( *m_vertexBuffer
			, *m_uboDescriptorSet
			, *m_texDescriptorSet
			, *m_frameBuffer
			, *m_timer );
	}

	ashes::Semaphore const & ReflectionPass::render( ashes::Semaphore const & toWait )const
	{
		auto * result = &toWait;
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto index = size_t( m_scene.getFog().getType() );
		auto & program = m_programs[index];

		m_device.getGraphicsQueue().submit( *program.m_commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	void ReflectionPass::accept( RenderTechniqueVisitor & visitor )
	{
		auto index = size_t( m_scene.getFog().getType() );
		auto & program = m_programs[index];
		program.accept( visitor );
	}
}

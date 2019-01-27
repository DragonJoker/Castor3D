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
#include "Shader/Shaders/GlslUtils.hpp"
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
#include <Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>

#include <Graphics/Image.hpp>

using namespace castor;
using namespace castor3d;

#define C3D_DebugSSAO 0
#define C3D_DebugDiffuseLighting 0
#define C3D_DebugSpecularLighting 0
#define C3D_DebugIBL 0
#define C3D_DebugNormals 0

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr c_environmentCount = 2u;

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
			if ( auto * buffer = vertexBuffer->getBuffer().lock( 0u
				, ~( 0ull )
				, ashes::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, data, sizeof( data ) );
				vertexBuffer->getBuffer().flush( 0u, ~( 0ull ) );
				vertexBuffer->getBuffer().unlock();
			}

			return vertexBuffer;
		}

		ShaderPtr doCreateVertexProgram( RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0 );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1 );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
			{
				vtx_texture = uv;
				out.gl_out.gl_Position = vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr doCreateLegacyPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			UBO_HDR_CONFIG( writer, HdrConfigUbo::BindingPoint, 0u );
			uint32_t index = MinBufferIndex;
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::LegacyMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
			
			shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth() };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();
			shader::PhongReflectionModel reflections{ writer };
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
			{
				auto lightResultTexcoord = writer.declLocale( cuT( "lightResultTexcoord" )
					, utils.bottomUpToTopDown( vtx_texture ) );
				auto uv = writer.declLocale( cuT( "uv" )
					, lightResultTexcoord );
				auto data5 = writer.declLocale( cuT( "data5" )
					, textureLod( c3d_mapData5, uv, 0.0_f ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );

				IF( writer, materialId == 0_i )
				{
					writer.discard();
				}
				FI;

				auto data1 = writer.declLocale( cuT( "data1" )
					, textureLod( c3d_mapData1, uv, 0.0_f ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, textureLod( c3d_mapData2, uv, 0.0_f ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, textureLod( c3d_mapData3, uv, 0.0_f ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, textureLod( c3d_mapData4, uv, 0.0_f ) );
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

				utils.decodeMaterial( flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );
				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, textureLod( c3d_mapLightDiffuse, lightResultTexcoord, 0.0_f ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, textureLod( c3d_mapLightSpecular, lightResultTexcoord, 0.0_f ).xyz() );
				auto depth = writer.declLocale( cuT( "depth" )
					, textureLod( c3d_mapDepth, uv, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( uv, depth, c3d_mtxInvViewProj ) );
				auto normal = writer.declLocale( cuT( "normal" )
					, normalize( data1.xyz() ) );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, data2.xyz() );
				auto occlusion = writer.declLocale( cuT( "occlusion" )
					, data3.a() );
				auto emissive = writer.declLocale( cuT( "emissive" )
					, data4.xyz() );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, clamp( c3d_ambientLight.xyz() + material.m_ambient * material.m_diffuse()
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );

				if ( hasSsao )
				{
					occlusion *= textureLod( c3d_mapSsao, uv, 0.0_f ).r();
				}

				IF( writer, envMapIndex > 0_i && writer.paren( reflection != 0_i || refraction != 0_i ) )
				{
					auto incident = writer.declLocale( cuT( "incident" )
						, reflections.computeIncident( position, c3d_cameraPosition.xyz() ) );
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i && refraction != 0_i )
					{
						for ( auto i = 0; i < c_environmentCount; ++i )
						{
							IF( writer, envMapIndex == Int( i ) )
							{
								ambient = reflections.computeReflRefr( incident
									, normal
									, occlusion
									, c3d_mapEnvironment[Int( i )]
									, material.m_refractionRatio
									, diffuse );
								diffuse = vec3( 0.0_f );
							}
							FI;
						}
					}
					ELSEIF( reflection != 0_i )
					{
						for ( auto i = 0; i < c_environmentCount; ++i )
						{
							IF( writer, envMapIndex == Int( i ) )
							{
								diffuse *= reflections.computeRefl( incident
									, normal
									, occlusion
									, c3d_mapEnvironment[Int( i )] );
								ambient = vec3( 0.0_f );
							}
							FI;
						}
					}
					ELSE
					{
						for ( auto i = 0; i < c_environmentCount; ++i )
						{
							IF( writer, envMapIndex == Int( i ) )
							{
								ambient = reflections.computeRefr( incident
									, normal
									, occlusion
									, c3d_mapEnvironment[Int( i )]
									, material.m_refractionRatio
									, diffuse );
								diffuse = vec3( 0.0_f );
							}
							FI;
						}
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
					position = utils.calcVSPosition( uv
							, textureLod( c3d_mapDepth, uv, 0.0_f ).r()
							, c3d_mtxInvProj );
					fog.applyFog( vec4( utils.removeGamma( c3d_gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, length( position )
						, position.z()
						, pxl_fragColor );
				}
				
#if C3D_DebugDiffuseLighting
				pxl_fragColor = vec4( lightDiffuse, 1.0_f );
#elif C3D_DebugSpecularLighting
				pxl_fragColor = vec4( lightSpecular, 1.0_f );
#elif C3D_DebugSSAO
				pxl_fragColor = vec4( vec3( occlusion ), 1.0_f );
#elif C3D_DebugSSSTransmittance
				pxl_fragColor = vec4( lightDiffuse, 1.0_f );
#elif C3D_DebugNormals
				pxl_fragColor = vec4( normal, 1.0_f );
#endif
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr doCreatePbrMRPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			UBO_HDR_CONFIG( writer, HdrConfigUbo::BindingPoint, 0u );
			auto index = MinBufferIndex;
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapBrdf" ), index++, 1u );
			auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapIrradiance" ), index++, 1u );
			auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapPrefiltered" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::PbrMRMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );

			shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth() };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeIBL();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();
			shader::MetallicPbrReflectionModel reflections{ writer };
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
			{
				auto lightResultTexcoord = writer.declLocale( cuT( "lightResultTexcoord" )
					, utils.bottomUpToTopDown( vtx_texture ) );
				auto uv = writer.declLocale( cuT( "uv" )
					, lightResultTexcoord );
				auto data5 = writer.declLocale( cuT( "data5" )
					, textureLod( c3d_mapData5, uv, 0.0_f ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );

				IF( writer, materialId == 0_i )
				{
					writer.discard();
				}
				FI;

				auto data1 = writer.declLocale( cuT( "data1" )
					, textureLod( c3d_mapData1, uv, 0.0_f ) );
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
				utils.decodeMaterial( flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );

				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, textureLod( c3d_mapData2, uv, 0.0_f ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, textureLod( c3d_mapData3, uv, 0.0_f ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, textureLod( c3d_mapData4, uv, 0.0_f ) );
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
					, textureLod( c3d_mapDepth, uv, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( uv, depth, c3d_mtxInvViewProj ) );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, c3d_ambientLight.xyz() );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, textureLod( c3d_mapLightDiffuse, lightResultTexcoord, 0.0_f ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, textureLod( c3d_mapLightSpecular, lightResultTexcoord, 0.0_f ).xyz() );

				if ( hasSsao )
				{
					occlusion *= textureLod( c3d_mapSsao, uv, 0.0_f ).r();
				}

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;
					auto incident = writer.declLocale( cuT( "incident" )
						, normalize( position - c3d_cameraPosition.xyz() ) );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio );

					IF( writer, reflection != 0_i )
					{
						for ( auto i = 0; i < c_environmentCount; ++i )
						{
							IF( writer, envMapIndex == Int( i ) )
							{
								// Reflection from environment map.
								ambient = reflections.computeRefl( incident
									, normal
									, occlusion
									, c3d_mapEnvironment[Int( i )]
									, c3d_ambientLight.xyz()
									, albedo );
							}
							FI;
						}
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
						for ( auto i = 0; i < c_environmentCount; ++i )
						{
							IF( writer, envMapIndex == Int( i ) )
							{
								// Refraction from environment map.
								ambient = reflections.computeRefr( incident
									, normal
									, occlusion
									, c3d_mapEnvironment[Int( i )]
									, material.m_refractionRatio
									, ambient
									, albedo
									, roughness );
							}
							FI;
						}
					}
					ELSEIF( ratio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
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
						, material.m_refractionRatio );

					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						auto incident = writer.declLocale( cuT( "incident" )
							, normalize( position - c3d_cameraPosition.xyz() ) );
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
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
					position = utils.calcVSPosition( uv
							, textureLod( c3d_mapDepth, uv, 0.0_f ).r()
							, c3d_mtxInvProj );
					fog.applyFog( vec4( utils.removeGamma( c3d_gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, length( position )
						, position.z()
						, pxl_fragColor );
				}

#if C3D_DebugDiffuseLighting
				pxl_fragColor = vec4( lightDiffuse, 1.0_f );
#elif C3D_DebugSpecularLighting
				pxl_fragColor = vec4( lightSpecular, 1.0_f );
#elif C3D_DebugSSAO
				pxl_fragColor = vec4( vec3( occlusion ), 1.0_f );
#elif C3D_DebugSSSTransmittance
				pxl_fragColor = vec4( lightDiffuse, 1.0_f );
#elif C3D_DebugIBL
				pxl_fragColor = vec4( ambient, 1.0_f );
#elif C3D_DebugNormals
				pxl_fragColor = vec4( normal, 1.0_f );
#endif
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr doCreatePbrSGPixelProgram( RenderSystem & renderSystem
			, FogType fogType
			, bool hasSsao )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
			UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
			UBO_HDR_CONFIG( writer, HdrConfigUbo::BindingPoint, 0u );
			auto index = MinBufferIndex;
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( cuT( "c3d_mapSsao" ), hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapLightDiffuse" ), index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapLightSpecular" ), index++, 1u );
			auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapBrdf" ), index++, 1u );
			auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapIrradiance" ), index++, 1u );
			auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( cuT( "c3d_mapPrefiltered" ), index++, 1u );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( cuT( "c3d_mapEnvironment" ), index++, 1u, c_environmentCount );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			shader::PbrSGMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );

			shader::Utils utils{ writer, renderSystem.isTopDown(), renderSystem.isZeroToOneDepth() };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeIBL();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();
			shader::SpecularPbrReflectionModel reflections{ writer };
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
			{
				auto lightResultTexcoord = writer.declLocale( cuT( "lightResultTexcoord" )
					, utils.bottomUpToTopDown( vtx_texture ) );
				auto uv = writer.declLocale( cuT( "uv" )
					, lightResultTexcoord );
				auto data5 = writer.declLocale( cuT( "data5" )
					, textureLod( c3d_mapData5, uv, 0.0_f ) );
				auto materialId = writer.declLocale( cuT( "materialId" )
					, writer.cast< Int >( data5.z() ) );

				IF( writer, materialId == 0_i )
				{
					writer.discard();
				}
				FI;

				auto data1 = writer.declLocale( cuT( "data1" )
					, textureLod( c3d_mapData1, uv, 0.0_f ) );
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
				utils.decodeMaterial(flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );

				auto material = writer.declLocale( cuT( "material" )
					, materials.getMaterial( materialId ) );
				auto data2 = writer.declLocale( cuT( "data2" )
					, textureLod( c3d_mapData2, uv, 0.0_f ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, textureLod( c3d_mapData3, uv, 0.0_f ) );
				auto data4 = writer.declLocale( cuT( "data4" )
					, textureLod( c3d_mapData4, uv, 0.0_f ) );
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
					, textureLod( c3d_mapDepth, uv, 0.0_f ).x() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( uv, depth, c3d_mtxInvViewProj ) );
				auto ambient = writer.declLocale( cuT( "ambient" )
					, c3d_ambientLight.xyz() );
				auto lightDiffuse = writer.declLocale( cuT( "lightDiffuse" )
					, textureLod( c3d_mapLightDiffuse, lightResultTexcoord, 0.0_f ).xyz() );
				auto lightSpecular = writer.declLocale( cuT( "lightSpecular" )
					, textureLod( c3d_mapLightSpecular, lightResultTexcoord, 0.0_f ).xyz() );

				if ( hasSsao )
				{
					occlusion *= textureLod( c3d_mapSsao, uv, 0.0_f ).r();
				}

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;
					auto incident = writer.declLocale( cuT( "incident" )
						, normalize( position - c3d_cameraPosition.xyz() ) );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, material.m_refractionRatio );

					IF( writer, reflection != 0_i )
					{
						for ( auto i = 0; i < c_environmentCount; ++i )
						{
							IF( writer, envMapIndex == Int( i ) )
							{
								// Reflection from environment map.
								ambient = reflections.computeRefl( incident
									, normal
									, occlusion
									, c3d_mapEnvironment[Int( i )]
									, c3d_ambientLight.xyz()
									, diffuse );
							}
							FI;
						}
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
						for ( auto i = 0; i < c_environmentCount; ++i )
						{
							IF( writer, envMapIndex == Int( i ) )
							{
								// Refraction from environment map.
								ambient = reflections.computeRefr( incident
									, normal
									, occlusion
									, c3d_mapEnvironment[Int( i )]
									, material.m_refractionRatio
									, ambient
									, diffuse
									, glossiness );
							}
							FI;
						}
					}
					ELSEIF( ratio != 0.0_f )
					{
						// Refraction from background skybox.
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
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
						, material.m_refractionRatio );

					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						auto incident = writer.declLocale( cuT( "incident" )
							, normalize( position - c3d_cameraPosition.xyz() ) );
						ambient = reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
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
					position = utils.calcVSPosition( uv
							, textureLod( c3d_mapDepth, uv, 0.0_f ).r()
							, c3d_mtxInvProj );
					fog.applyFog( vec4( utils.removeGamma( c3d_gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, length( position )
						, position.z()
						, pxl_fragColor );
				}

#if C3D_DebugDiffuseLighting
				pxl_fragColor = vec4( lightDiffuse, 1.0_f );
#elif C3D_DebugSpecularLighting
				pxl_fragColor = vec4( lightSpecular, 1.0_f );
#elif C3D_DebugSSAO
				pxl_fragColor = vec4( vec3( occlusion ), 1.0_f );
#elif C3D_DebugSSSTransmittance
				pxl_fragColor = vec4( lightDiffuse, 1.0_f );
#elif C3D_DebugIBL
				pxl_fragColor = vec4( ambient, 1.0_f );
#elif C3D_DebugNormals
				pxl_fragColor = vec4( normal, 1.0_f );
#endif
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::ShaderStageStateArray doCreateProgram( Engine & engine
			, FogType fogType
			, bool hasSsao
			, MaterialType matType
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & renderSystem = *engine.getRenderSystem();
			vertexShader.shader = doCreateVertexProgram( renderSystem );
			pixelShader.shader = matType == MaterialType::eLegacy
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
			result[0].module->loadShader( renderSystem.compileShader( vertexShader ) );
			result[1].module->loadShader( renderSystem.compileShader( pixelShader ) );
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
				{ HdrConfigUbo::BindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
			};
			return getCurrentDevice( engine ).createDescriptorSetLayout( std::move( bindings ) );
		}

		inline ashes::DescriptorSetPtr doCreateUboDescriptorSet( Engine & engine
			, ashes::DescriptorSetPool & pool
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, HdrConfigUbo const & hdrConfigUbo )
		{
			auto & passBuffer = engine.getMaterialCache().getPassBuffer();
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 0u );
			passBuffer.createBinding( *result, layout.getBinding( PassBufferIndex ) );
			result->createBinding( layout.getBinding( SceneUbo::BindingPoint )
				, sceneUbo.getUbo() );
			result->createBinding( layout.getBinding( GpInfoUbo::BindingPoint )
				, gpInfoUbo.getUbo() );
			result->createBinding( layout.getBinding( HdrConfigUbo::BindingPoint )
				, hdrConfigUbo.getUbo() );
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
		, GeometryPassResult const & gp
		, ashes::DescriptorSetLayout const & uboLayout
		, ashes::DescriptorSetLayout const & texLayout
		, ashes::RenderPass const & renderPass
		, ashes::TextureView const * ssao
		, ashes::Extent2D const & size
		, FogType fogType
		, MaterialType matType )
		: m_geometryPassResult{ gp }
		, m_program{ doCreateProgram( engine, fogType, ssao != nullptr, matType, m_vertexShader, m_pixelShader ) }
		, m_pipelineLayout{ getCurrentDevice( engine ).createPipelineLayout( { uboLayout, texLayout } ) }
		, m_pipeline{ doCreateRenderPipeline( *m_pipelineLayout, m_program, renderPass, size ) }
		, m_commandBuffer{ getCurrentDevice( engine ).getGraphicsCommandPool().createCommandBuffer( true ) }
		, m_renderPass{ &renderPass }
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "Reflection" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "Reflection" }
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
		m_commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_geometryPassResult.getDepthStencilView().makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
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
			, *m_vertexShader.shader );
		visitor.visit( cuT( "Reflection" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
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
		, HdrConfigUbo & hdrConfigUbo
		, ashes::TextureView const * ssao )
		: OwnedBy< Engine >{ engine }
		, m_device{ getCurrentDevice( engine ) }
		, m_scene{ scene }
		, m_ssaoResult{ ssao }
		, m_size{ result.getTexture().getDimensions().width, result.getTexture().getDimensions().height }
		, m_sampler{ engine.getDefaultSampler() }
		, m_geometryPassResult{ gp }
		, m_lightDiffuse{ lightDiffuse }
		, m_lightSpecular{ lightSpecular }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_uboDescriptorLayout{ doCreateUboDescriptorLayout( engine ) }
		, m_uboDescriptorPool{ m_uboDescriptorLayout->createPool( 1u ) }
		, m_uboDescriptorSet{ doCreateUboDescriptorSet( engine, *m_uboDescriptorPool, sceneUbo, gpInfoUbo, hdrConfigUbo ) }
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
					gp,
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
					gp,
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
					gp,
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
					gp,
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

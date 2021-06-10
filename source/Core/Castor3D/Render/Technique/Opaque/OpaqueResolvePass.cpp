#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/SsaoPass.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <CastorUtils/Graphics/Image.hpp>

CU_ImplementCUSmartPtr( castor3d, OpaqueResolvePass )

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr c_noIblEnvironmentCount = 5u;
		static uint32_t constexpr c_iblTexturesCount = 3u;
		static uint32_t constexpr c_iblEnvironmentCount = c_noIblEnvironmentCount - c_iblTexturesCount;

		ashes::PipelineVertexInputStateCreateInfoPtr doCreateVertexLayout()
		{
			return std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray
				{
					{ 0u, 16u, VK_VERTEX_INPUT_RATE_VERTEX },
				}
				, ashes::VkVertexInputAttributeDescriptionArray
				{
					{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, 0u },
					{ 1u, 0u, VK_FORMAT_R32G32_SFLOAT, 8u },
				} );
		}

		ashes::VertexBufferBasePtr doCreateVbo( RenderDevice const & device )
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

			auto vertexBuffer = makeVertexBufferBase( device
				, uint32_t( sizeof( data ) )
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "DeferredResolve" );

			if ( auto * buffer = vertexBuffer->getBuffer().lock( 0u
				, ~( 0ull )
				, 0u ) )
			{
				std::memcpy( buffer, data, sizeof( data ) );
				vertexBuffer->getBuffer().flush( 0u, ~( 0ull ) );
				vertexBuffer->getBuffer().unlock();
			}

			return vertexBuffer;
		}

		ShaderPtr doCreateVertexProgram( RenderDevice const & device )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0 );
			auto uv = writer.declInput< Vec2 >( "uv", 1 );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
			{
				vtx_texture = uv;
				out.vtx.position = vec4( position.x(), position.y(), 0.0_f, 1.0_f );
			} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		enum class ResolveUbo
		{
			eMaterials,
			eScene,
			eGpInfo,
			eHdrConfig,
		};

		ShaderPtr doCreatePhongPixelProgram( RenderDevice const & device
			, FogType fogType
			, bool hasSsao
			, bool hasDiffuseGi
			, bool hasSpecularGi )
		{
			auto & renderSystem = device.renderSystem;
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::PhongMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( ResolveUbo::eMaterials )
				, 0u );
			UBO_SCENE( writer, uint32_t( ResolveUbo::eScene ), 0u );
			UBO_GPINFO( writer, uint32_t( ResolveUbo::eGpInfo ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( ResolveUbo::eHdrConfig ), 0u );

			auto index = 0u;
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( "c3d_mapSsao", hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightSpecular", index++, 1u );
			auto c3d_mapLightIndirectDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectDiffuse", hasDiffuseGi ? index++ : 0u, 1u, hasDiffuseGi );
			auto c3d_mapLightIndirectSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectSpecular", hasSpecularGi ? index++ : 0u, 1u, hasSpecularGi );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( "c3d_mapEnvironment", index++, 1u, c_noIblEnvironmentCount );
			CU_Require( index < device.properties.limits.maxDescriptorSetSampledImages );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();

			if ( hasSpecularGi )
			{
				utils.declareFresnelSchlick();
			}

			shader::PhongReflectionModel reflections{ writer, utils };
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto data5 = writer.declLocale( "data5"
						, c3d_mapData5.lod( vtx_texture, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() ) );

					IF( writer, materialId == 0_u )
					{
						writer.discard();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, c3d_mapData1.lod( vtx_texture, 0.0_f ) );
					auto data2 = writer.declLocale( "data2"
						, c3d_mapData2.lod( vtx_texture, 0.0_f ) );
					auto flags = writer.declLocale( "flags"
						, data1.w() );
					auto envMapIndex = writer.declLocale( "envMapIndex"
						, 0_i );
					auto receiver = writer.declLocale( "receiver"
						, 0_i );
					auto reflection = writer.declLocale( "reflection"
						, 0_i );
					auto refraction = writer.declLocale( "refraction"
						, 0_i );
					auto lighting = writer.declLocale( "lighting"
						, 0_i );

					utils.decodeMaterial( flags
						, receiver
						, reflection
						, refraction
						, lighting
						, envMapIndex );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( vtx_texture, 0.0_f ).x() );
					auto diffuse = writer.declLocale( "diffuse"
						, data2.xyz() );
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( c3d_gpInfoData.projToWorld( utils, vtx_texture, depth )
						, data1.rgb() );

					IF( writer, lighting )
					{
						auto data3 = writer.declLocale( "data3"
							, c3d_mapData3.lod( vtx_texture, 0.0_f ) );
						auto data4 = writer.declLocale( "data4"
							, c3d_mapData4.lod( vtx_texture, 0.0_f ) );
						auto material = writer.declLocale( "material"
							, materials.getMaterial( materialId ) );
						auto shininess = writer.declLocale( "shininess"
							, data2.w() );
						auto specular = writer.declLocale( "specular"
							, data3.xyz() );
						auto occlusion = writer.declLocale( "occlusion"
							, data3.a() );
						auto emissive = writer.declLocale( "emissive"
							, data4.xyz() );
						auto ambient = writer.declLocale( "ambient"
							, clamp( c3d_sceneData.getAmbientLight() * material.m_ambient * diffuse
								, vec3( 0.0_f )
								, vec3( 1.0_f ) ) );
						auto lightDiffuse = writer.declLocale( "lightDiffuse"
							, c3d_mapLightDiffuse.lod( vtx_texture, 0.0_f ).xyz() );
						auto lightSpecular = writer.declLocale( "lightSpecular"
							, c3d_mapLightSpecular.lod( vtx_texture, 0.0_f ).xyz() );
						auto lightIndirectDiffuse = writer.declLocale( "lightIndirectDiffuse"
							, c3d_mapLightIndirectDiffuse.lod( vtx_texture, 0.0_f ).rgb()
							, hasDiffuseGi );
						auto lightIndirectSpecular = writer.declLocale( "lightIndirectSpecular"
							, c3d_mapLightIndirectSpecular.lod( vtx_texture, 0.0_f ).rgb()
							, hasSpecularGi );
						lightSpecular *= specular;

						if ( hasSpecularGi )
						{
							auto V = writer.declLocale( "V"
								, normalize( c3d_sceneData.getPosToCamera( surface.worldPosition ) ) );
							auto NdotV = writer.declLocale( "NdotV"
								, max( 0.0_f, dot( surface.worldNormal, V ) ) );
							lightIndirectSpecular *= utils.fresnelSchlick( NdotV, specular, ( 256.0_f - shininess ) / 256.0_f );
						}

						auto reflected = writer.declLocale( "reflected"
							, vec3( 0.0_f ) );
						auto refracted = writer.declLocale( "refracted"
							, vec3( 0.0_f ) );

						if ( hasSsao )
						{
							occlusion *= c3d_mapSsao.lod( vtx_texture, 0.0_f ).r();
						}

						IF( writer, envMapIndex > 0_i && ( reflection != 0_i || refraction != 0_i ) )
						{
							auto incident = writer.declLocale( "incident"
								, reflections.computeIncident( surface.worldPosition, c3d_sceneData.getCameraPosition() ) );
							envMapIndex = envMapIndex - 1_i;
							ambient = vec3( 0.0_f );

							IF( writer, envMapIndex >= Int( c_noIblEnvironmentCount ) )
							{
								envMapIndex = 0_i;
							}
							FI;

							IF( writer, reflection != 0_i && refraction != 0_i )
							{
								for ( auto i = 0; i < c_noIblEnvironmentCount; ++i )
								{
									IF( writer, envMapIndex == Int( i ) )
									{
										reflections.computeReflRefr( incident
											, surface.worldNormal
											, c3d_mapEnvironment[Int( i )]
											, material.m_refractionRatio
											, specular
											, material.m_transmission * diffuse
											, shininess
											, reflected
											, refracted );
									}
									FI;
								}
							}
							ELSEIF( reflection != 0_i )
							{
								for ( auto i = 0; i < c_noIblEnvironmentCount; ++i )
								{
									IF( writer, envMapIndex == Int( i ) )
									{
										reflected = reflections.computeRefl( incident
											, surface.worldNormal
											, c3d_mapEnvironment[Int( i )]
											, shininess
											, specular );
									}
									FI;
								}
							}
							ELSE
							{
								for ( auto i = 0; i < c_noIblEnvironmentCount; ++i )
								{
									IF( writer, envMapIndex == Int( i ) )
									{
										reflections.computeRefr( incident
											, surface.worldNormal
											, c3d_mapEnvironment[Int( i )]
											, material.m_refractionRatio
											, material.m_transmission * diffuse
											, shininess
											, reflected
											, refracted );
									}
									FI;
								}
							}
							FI;

							diffuse = vec3( 0.0_f );
						}
						ELSE
						{
							diffuse *= lightDiffuse;
						}
						FI;

						ambient *= occlusion;
						ambient *= lightIndirectDiffuse;
						lightDiffuse += lightIndirectDiffuse * occlusion;
						lightSpecular += lightIndirectSpecular * occlusion;
						pxl_fragColor = vec4( diffuse
								+ reflected
								+ refracted
								+ lightSpecular
								+ emissive
								+ ambient
							, 1.0_f );
					}
					ELSE
					{
						pxl_fragColor = vec4( diffuse, 1.0_f );
					}
					FI;

					if ( fogType != FogType::eDisabled )
					{
						surface.viewPosition = c3d_gpInfoData.projToView( utils
							, vtx_texture
							, c3d_mapDepth.lod( vtx_texture, 0.0_f ).x() );
						pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData )
							, pxl_fragColor
							, length( surface.viewPosition )
							, surface.viewPosition.z()
							, c3d_sceneData );
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr doCreatePbrMRPixelProgram( RenderDevice const & device
			, FogType fogType
			, bool hasSsao
			, bool hasDiffuseGi
			, bool hasSpecularGi )
		{
			auto & renderSystem = device.renderSystem;
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::PbrMRMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( ResolveUbo::eMaterials )
				, 0u );
			UBO_SCENE( writer, uint32_t( ResolveUbo::eScene ), 0u );
			UBO_GPINFO( writer, uint32_t( ResolveUbo::eGpInfo ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( ResolveUbo::eHdrConfig ), 0u );
			auto index = 0u;
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( "c3d_mapSsao", hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightSpecular", index++, 1u );
			auto c3d_mapLightIndirectDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectDiffuse", hasDiffuseGi ? index++ : 0u, 1u, hasDiffuseGi );
			auto c3d_mapLightIndirectSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectSpecular", hasSpecularGi ? index++ : 0u, 1u, hasSpecularGi );
			auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf", index++, 1u );
			auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance", index++, 1u );
			auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered", index++, 1u );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( "c3d_mapEnvironment", index++, 1u, c_iblEnvironmentCount );
			CU_Require( index < device.properties.limits.maxDescriptorSetSampledImages );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeIBL();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();

			if ( hasSpecularGi )
			{
				utils.declareFresnelSchlick();
			}

			shader::MetallicPbrReflectionModel reflections{ writer, utils };
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto fixedTexCoord = writer.declLocale( "fixedTexCoord"
						, vtx_texture );
					auto data5 = writer.declLocale( "data5"
						, c3d_mapData5.lod( fixedTexCoord, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() ) );

					IF( writer, materialId == 0_u )
					{
						writer.discard();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, c3d_mapData1.lod( fixedTexCoord, 0.0_f ) );
					auto data2 = writer.declLocale( "data2"
						, c3d_mapData2.lod( fixedTexCoord, 0.0_f ) );
					auto flags = writer.declLocale( "flags"
						, data1.w() );
					auto envMapIndex = writer.declLocale( "envMapIndex"
						, 0_i );
					auto receiver = writer.declLocale( "receiver"
						, 0_i );
					auto reflection = writer.declLocale( "reflection"
						, 0_i );
					auto refraction = writer.declLocale( "refraction"
						, 0_i );
					auto lighting = writer.declLocale( "lighting"
						, 0_i );
					utils.decodeMaterial( flags
						, receiver
						, reflection
						, refraction
						, lighting
						, envMapIndex );

					auto material = writer.declLocale( "material"
						, materials.getMaterial( materialId ) );
					auto albedo = writer.declLocale( "albedo"
						, data2.rgb() );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( vtx_texture, 0.0_f ).x() );
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( c3d_gpInfoData.projToWorld( utils, vtx_texture, depth )
						, data1.rgb() );

					IF( writer, lighting )
					{
						auto data3 = writer.declLocale( "data3"
							, c3d_mapData3.lod( fixedTexCoord, 0.0_f ) );
						auto data4 = writer.declLocale( "data4"
							, c3d_mapData4.lod( fixedTexCoord, 0.0_f ) );
						auto roughness = writer.declLocale( "roughness"
							, data2.a() );
						auto metalness = writer.declLocale( "metalness"
							, data3.r() );
						auto occlusion = writer.declLocale( "occlusion"
							, data3.a() );
						auto emissive = writer.declLocale( "emissive"
							, data4.rgb() );
						auto ambient = writer.declLocale( "ambient"
							, c3d_sceneData.getAmbientLight() );
						auto lightDiffuse = writer.declLocale( "lightDiffuse"
							, c3d_mapLightDiffuse.lod( fixedTexCoord, 0.0_f ).rgb() );
						auto lightSpecular = writer.declLocale( "lightSpecular"
							, c3d_mapLightSpecular.lod( fixedTexCoord, 0.0_f ).rgb() );
						auto lightIndirectDiffuse = writer.declLocale( "lightIndirectDiffuse"
							, c3d_mapLightIndirectDiffuse.lod( vtx_texture, 0.0_f ).rgb()
							, hasDiffuseGi );
						auto lightIndirectSpecular = writer.declLocale( "lightIndirectSpecular"
							, c3d_mapLightIndirectSpecular.lod( vtx_texture, 0.0_f ).rgb()
							, hasSpecularGi );

						if ( hasSpecularGi )
						{
							auto V = writer.declLocale( "V"
								, normalize( c3d_sceneData.getPosToCamera( surface.worldPosition ) ) );
							auto NdotV = writer.declLocale( "NdotV"
								, max( 0.0_f, dot( surface.worldNormal, V ) ) );
							lightIndirectSpecular *= utils.fresnelSchlick( NdotV, mix( vec3( 0.04_f ), albedo, vec3( metalness ) ), roughness );
						}

						auto reflected = writer.declLocale( "reflected"
							, vec3( 0.0_f ) );
						auto refracted = writer.declLocale( "refracted"
							, vec3( 0.0_f ) );

						if ( hasSsao )
						{
							occlusion *= c3d_mapSsao.lod( fixedTexCoord, 0.0_f ).r();
						}

						IF( writer, envMapIndex > 0_i )
						{
							envMapIndex = envMapIndex - 1_i;

							IF( writer, envMapIndex >= Int( c_iblEnvironmentCount ) )
							{
								envMapIndex = 0_i;
							}
							FI;

							auto incident = writer.declLocale( "incident"
								, normalize( c3d_sceneData.getCameraToPos( surface.worldPosition ) ) );
							auto ratio = writer.declLocale( "ratio"
								, material.m_refractionRatio );

							IF( writer, reflection != 0_i )
							{
								ambient = vec3( 0.0_f );

								for ( auto i = 0; i < c_iblEnvironmentCount; ++i )
								{
									IF( writer, envMapIndex == Int( i ) )
									{
										// Reflection from environment map.
										reflected = reflections.computeRefl( incident
											, surface.worldNormal
											, c3d_mapEnvironment[Int( i )]
											, albedo
											, metalness
											, roughness );
									}
									FI;
								}

								IF( writer, refraction != 0_i )
								{
									for ( auto i = 0; i < c_iblEnvironmentCount; ++i )
									{
										IF( writer, envMapIndex == Int( i ) )
										{
											// Refraction from environment map.
											reflections.computeRefrEnvMap( incident
												, surface.worldNormal
												, c3d_mapEnvironment[Int( i )]
												, material.m_refractionRatio
												, material.m_transmission * albedo
												, roughness
												, reflected
												, refracted );
										}
										FI;
									}
								}
								ELSEIF( ratio != 0.0_f )
								{
									// Refraction from background skybox.
									reflections.computeRefrSkybox( incident
										, surface.worldNormal
										, c3d_mapPrefiltered
										, material.m_refractionRatio
										, material.m_transmission * albedo
										, roughness
										, reflected
										, refracted );
								}
								FI;
							}
							ELSE
							{
								// Reflection from background skybox.
								ambient *= utils.computeMetallicIBL( surface
									, albedo
									, metalness
									, roughness
									, c3d_sceneData.getCameraPosition()
									, c3d_mapIrradiance
									, c3d_mapPrefiltered
									, c3d_mapBrdf );

								IF( writer, refraction != 0_i )
								{
									for ( auto i = 0; i < c_iblEnvironmentCount; ++i )
									{
										IF( writer, envMapIndex == Int( i ) )
										{
											// Refraction from environment map.
											reflections.computeRefrEnvMap( incident
												, surface.worldNormal
												, c3d_mapEnvironment[Int( i )]
												, material.m_refractionRatio
												, material.m_transmission * albedo
												, roughness
												, ambient
												, refracted );
										}
										FI;
									}
								}
								ELSEIF( ratio != 0.0_f )
								{
									// Refraction from background skybox.
									reflections.computeRefrSkybox( incident
										, surface.worldNormal
										, c3d_mapPrefiltered
										, material.m_refractionRatio
										, material.m_transmission * albedo
										, roughness
										, ambient
										, refracted );
								}
								FI;
							}
							FI;
						}
						ELSE
						{
							// Reflection from background skybox.
							ambient *= utils.computeMetallicIBL( surface
								, albedo
								, metalness
								, roughness
								, c3d_sceneData.getCameraPosition()
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
							auto ratio = writer.declLocale( "ratio"
								, material.m_refractionRatio );

							IF( writer, ratio != 0.0_f )
							{
								// Refraction from background skybox.
								auto incident = writer.declLocale( "incident"
									, normalize( c3d_sceneData.getCameraToPos( surface.worldPosition ) ) );
								reflections.computeRefrSkybox( incident
									, surface.worldNormal
									, c3d_mapPrefiltered
									, material.m_refractionRatio
									, material.m_transmission * albedo
									, roughness
									, ambient
									, refracted );
							}
							FI;
						}
						FI;

						ambient *= occlusion;
						ambient *= lightIndirectDiffuse;
						lightDiffuse += lightIndirectDiffuse * occlusion;
						lightSpecular += lightIndirectSpecular * occlusion;
						pxl_fragColor = vec4( lightDiffuse * albedo
								+ lightSpecular
								+ emissive
								+ ambient
								+ reflected + refracted
							, 1.0_f );
					}
					ELSE
					{
						pxl_fragColor = vec4( albedo, 1.0_f );
					}
					FI;

					if ( fogType != FogType::eDisabled )
					{
						surface.viewPosition = c3d_gpInfoData.projToView( utils
							, vtx_texture
							, c3d_mapDepth.lod( vtx_texture, 0.0_f ).x() );
						pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData )
							, pxl_fragColor
							, length( surface.viewPosition )
							, surface.viewPosition.z()
							, c3d_sceneData );
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr doCreatePbrSGPixelProgram( RenderDevice const & device
			, FogType fogType
			, bool hasSsao
			, bool hasDiffuseGi
			, bool hasSpecularGi )
		{
			auto & renderSystem = device.renderSystem;
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::PbrSGMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( ResolveUbo::eMaterials )
				, 0u );
			UBO_SCENE( writer, uint32_t( ResolveUbo::eScene ), 0u );
			UBO_GPINFO( writer, uint32_t( ResolveUbo::eGpInfo ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( ResolveUbo::eHdrConfig ), 0u );
			auto index = 0u;
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), index++, 1u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), index++, 1u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), index++, 1u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), index++, 1u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), index++, 1u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), index++, 1u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( "c3d_mapSsao", hasSsao ? index++ : 0u, 1u, hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", index++, 1u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightSpecular", index++, 1u );
			auto c3d_mapLightIndirectDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectDiffuse", hasDiffuseGi ? index++ : 0u, 1u, hasDiffuseGi );
			auto c3d_mapLightIndirectSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectSpecular", hasSpecularGi ? index++ : 0u, 1u, hasSpecularGi );
			auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf", index++, 1u );
			auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance", index++, 1u );
			auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered", index++, 1u );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( "c3d_mapEnvironment", index++, 1u, c_iblEnvironmentCount );
			CU_Require( index < device.properties.limits.maxDescriptorSetSampledImages );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeIBL();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();

			if ( hasSpecularGi )
			{
				utils.declareFresnelSchlick();
			}

			shader::SpecularPbrReflectionModel reflections{ writer, utils };
			shader::Fog fog{ fogType, writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto fixedTexCoord = writer.declLocale( "fixedTexCoord"
						, vtx_texture );
					auto data5 = writer.declLocale( "data5"
						, c3d_mapData5.lod( fixedTexCoord, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data5.z() ) );

					IF( writer, materialId == 0_u )
					{
						writer.discard();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, c3d_mapData1.lod( fixedTexCoord, 0.0_f ) );
					auto data2 = writer.declLocale( "data2"
						, c3d_mapData2.lod( fixedTexCoord, 0.0_f ) );
					auto flags = writer.declLocale( "flags"
						, data1.w() );
					auto envMapIndex = writer.declLocale( "envMapIndex"
						, 0_i );
					auto receiver = writer.declLocale( "receiver"
						, 0_i );
					auto reflection = writer.declLocale( "reflection"
						, 0_i );
					auto refraction = writer.declLocale( "refraction"
						, 0_i );
					auto lighting = writer.declLocale( "lighting"
						, 0_i );
					utils.decodeMaterial( flags
						, receiver
						, reflection
						, refraction
						, lighting
						, envMapIndex );

					auto material = writer.declLocale( "material"
						, materials.getMaterial( materialId ) );
					auto diffuse = writer.declLocale( "diffuse"
						, data2.xyz() );
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( vtx_texture, 0.0_f ).x() );
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( c3d_gpInfoData.projToWorld( utils, vtx_texture, depth )
						, data1.rgb() );

					IF( writer, lighting )
					{
						auto data3 = writer.declLocale( "data3"
							, c3d_mapData3.lod( fixedTexCoord, 0.0_f ) );
						auto data4 = writer.declLocale( "data4"
							, c3d_mapData4.lod( fixedTexCoord, 0.0_f ) );
						auto glossiness = writer.declLocale( "glossiness"
							, data2.w() );
						auto specular = writer.declLocale( "specular"
							, data3.xyz() );
						auto occlusion = writer.declLocale( "occlusion"
							, data3.a() );
						auto emissive = writer.declLocale( "emissive"
							, data4.xyz() );
						auto ambient = writer.declLocale( "ambient"
							, c3d_sceneData.getAmbientLight() );
						auto lightDiffuse = writer.declLocale( "lightDiffuse"
							, c3d_mapLightDiffuse.lod( fixedTexCoord, 0.0_f ).xyz() );
						auto lightSpecular = writer.declLocale( "lightSpecular"
							, c3d_mapLightSpecular.lod( fixedTexCoord, 0.0_f ).xyz() );
						auto lightIndirectDiffuse = writer.declLocale( "lightIndirectDiffuse"
							, c3d_mapLightIndirectDiffuse.lod( vtx_texture, 0.0_f ).rgb()
							, hasDiffuseGi );
						auto lightIndirectSpecular = writer.declLocale( "lightIndirectSpecular"
							, c3d_mapLightIndirectSpecular.lod( vtx_texture, 0.0_f ).rgb()
							, hasSpecularGi );

						if ( hasSpecularGi )
						{
							auto V = writer.declLocale( "V"
								, normalize( c3d_sceneData.getPosToCamera( surface.worldPosition ) ) );
							auto NdotV = writer.declLocale( "NdotV"
								, max( 0.0_f, dot( surface.worldNormal, V ) ) );
							lightIndirectSpecular *= utils.fresnelSchlick( NdotV, specular, 1.0_f - glossiness );
						}

						auto reflected = writer.declLocale( "reflected"
							, vec3( 0.0_f ) );
						auto refracted = writer.declLocale( "refracted"
							, vec3( 0.0_f ) );

						if ( hasSsao )
						{
							occlusion *= c3d_mapSsao.lod( fixedTexCoord, 0.0_f ).r();
						}

						IF( writer, envMapIndex > 0_i )
						{
							envMapIndex = envMapIndex - 1_i;

							IF( writer, envMapIndex >= Int( c_iblEnvironmentCount ) )
							{
								envMapIndex = 0_i;
							}
							FI;

							auto incident = writer.declLocale( "incident"
								, normalize( c3d_sceneData.getCameraToPos( surface.worldPosition ) ) );
							auto ratio = writer.declLocale( "ratio"
								, material.m_refractionRatio );

							IF( writer, reflection != 0_i )
							{
								ambient = vec3( 0.0_f );

								for ( auto i = 0; i < c_iblEnvironmentCount; ++i )
								{
									IF( writer, envMapIndex == Int( i ) )
									{
										// Reflection from environment map.
										reflected = reflections.computeRefl( incident
											, surface.worldNormal
											, c3d_mapEnvironment[Int( i )]
											, specular
											, glossiness );
									}
									FI;
								}

								IF( writer, refraction != 0_i )
								{
									for ( auto i = 0; i < c_iblEnvironmentCount; ++i )
									{
										IF( writer, envMapIndex == Int( i ) )
										{
											// Refraction from environment map.
											reflections.computeRefrEnvMap( incident
												, surface.worldNormal
												, c3d_mapEnvironment[Int( i )]
												, material.m_refractionRatio
												, material.m_transmission * diffuse
												, glossiness
												, reflected
												, refracted );
										}
										FI;
									}
								}
								ELSEIF( ratio != 0.0_f )
								{
									// Refraction from background skybox.
									reflections.computeRefrSkybox( incident
										, surface.worldNormal
										, c3d_mapPrefiltered
										, material.m_refractionRatio
										, material.m_transmission * diffuse
										, glossiness
										, reflected
										, refracted );
								}
								FI;
							}
							ELSE
							{
								// Reflection from background skybox.
								ambient *= utils.computeSpecularIBL( surface
									, diffuse
									, specular
									, glossiness
									, c3d_sceneData.getCameraPosition()
									, c3d_mapIrradiance
									, c3d_mapPrefiltered
									, c3d_mapBrdf );

								IF( writer, refraction != 0_i )
								{
									for ( auto i = 0; i < c_iblEnvironmentCount; ++i )
									{
										IF( writer, envMapIndex == Int( i ) )
										{
											// Refraction from environment map.
											reflections.computeRefrEnvMap( incident
												, surface.worldNormal
												, c3d_mapEnvironment[Int( i )]
												, material.m_refractionRatio
												, material.m_transmission * diffuse
												, glossiness
												, ambient
												, refracted );
										}
										FI;
									}
								}
								ELSEIF( ratio != 0.0_f )
								{
									// Refraction from background skybox.
									reflections.computeRefrSkybox( incident
										, surface.worldNormal
										, c3d_mapPrefiltered
										, material.m_refractionRatio
										, material.m_transmission * diffuse
										, glossiness
										, ambient
										, refracted );
								}
								FI;
							}
							FI;
						}
						ELSE
						{
							// Reflection from background skybox.
							ambient *= utils.computeSpecularIBL( surface
								, diffuse
								, specular
								, glossiness
								, c3d_sceneData.getCameraPosition()
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
							auto ratio = writer.declLocale( "ratio"
								, material.m_refractionRatio );

							IF( writer, ratio != 0.0_f )
							{
								// Refraction from background skybox.
								auto incident = writer.declLocale( "incident"
									, normalize( c3d_sceneData.getCameraToPos( surface.worldPosition ) ) );
								reflections.computeRefrSkybox( incident
									, surface.worldNormal
									, c3d_mapPrefiltered
									, material.m_refractionRatio
									, material.m_transmission * diffuse
									, glossiness
									, ambient
									, refracted );
							}
							FI;
						}
						FI;

						ambient *= occlusion;
						ambient *= lightIndirectDiffuse;
						lightDiffuse += lightIndirectDiffuse * occlusion;
						lightSpecular += lightIndirectSpecular * occlusion;
						pxl_fragColor = vec4( lightDiffuse * diffuse
								+ lightSpecular
								+ emissive
								+ ambient
								+ reflected + refracted
							, 1.0_f );
					}
					ELSE
					{
						pxl_fragColor = vec4( diffuse, 1.0_f );
					}
					FI;

					if ( fogType != FogType::eDisabled )
					{
						surface.viewPosition = c3d_gpInfoData.projToView( utils
							, vtx_texture
							, c3d_mapDepth.lod( vtx_texture, 0.0_f ).x() );
						pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData )
							, pxl_fragColor
							, length( surface.viewPosition )
							, surface.viewPosition.z()
							, c3d_sceneData );
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
			, FogType fogType
			, bool hasSsao
			, bool hasDiffuseGi
			, bool hasSpecularGi
			, MaterialType matType
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			vertexShader.shader = doCreateVertexProgram( device );
			pixelShader.shader = ( matType == MaterialType::ePhong
				? doCreatePhongPixelProgram( device, fogType, hasSsao, hasDiffuseGi, hasSpecularGi )
				: ( matType == MaterialType::eMetallicRoughness
					? doCreatePbrMRPixelProgram( device, fogType, hasSsao, hasDiffuseGi, hasSpecularGi )
					: doCreatePbrSGPixelProgram( device, fogType, hasSsao, hasDiffuseGi, hasSpecularGi ) ) );

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vertexShader ),
				makeShaderState( device, pixelShader ),
			};
		}

		inline ashes::DescriptorSetLayoutPtr doCreateUboDescriptorLayout( Engine & engine
			, RenderDevice const & device )
		{
			auto & passBuffer = engine.getMaterialCache().getPassBuffer();
			ashes::VkDescriptorSetLayoutBindingArray bindings
			{
				passBuffer.createLayoutBinding( uint32_t( ResolveUbo::eMaterials ) ),
				makeDescriptorSetLayoutBinding( uint32_t( ResolveUbo::eScene )
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
				makeDescriptorSetLayoutBinding( uint32_t( ResolveUbo::eGpInfo )
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
				makeDescriptorSetLayoutBinding( uint32_t( ResolveUbo::eHdrConfig )
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
			};
			auto result = device->createDescriptorSetLayout( "DeferredResolveUbo"
				, std::move( bindings ) );
			return result;
		}

		inline ashes::DescriptorSetPtr doCreateUboDescriptorSet( Engine & engine
			, ashes::DescriptorSetPool & pool
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, HdrConfigUbo const & hdrConfigUbo )
		{
			auto & passBuffer = engine.getMaterialCache().getPassBuffer();
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( "DeferredResolveUbo", 0u );
			passBuffer.createBinding( *result, layout.getBinding( uint32_t( ResolveUbo::eMaterials ) ) );
			sceneUbo.createSizedBinding( *result
				, layout.getBinding( uint32_t( ResolveUbo::eScene ) ) );
			gpInfoUbo.createSizedBinding( *result
				, layout.getBinding( uint32_t( ResolveUbo::eGpInfo ) ) );
			hdrConfigUbo.createSizedBinding( *result
				, layout.getBinding( uint32_t( ResolveUbo::eHdrConfig ) ) );
			result->update();
			return result;
		}

		inline ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				}
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
					{},
					ashes::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( "DeferredResolve"
				, std::move( createInfo ) );
			return result;
		}

		inline ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, VkExtent2D const size
			, ashes::ImageView const & view )
		{
			ashes::ImageViewCRefArray attaches
			{
				view
			};
			auto result = renderPass.createFrameBuffer( "DeferredResolve"
				, size
				, std::move( attaches ) );
			return result;
		}

		inline ashes::DescriptorSetLayoutPtr doCreateTexDescriptorLayout( RenderDevice const & device
			, bool hasSsao
			, bool hasDiffuseGi
			, bool hasSpecularGi
			, MaterialType matType )
		{
			uint32_t index = 0u;
			ashes::VkDescriptorSetLayoutBindingArray bindings
			{
				makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),	// DsTexture::eDepth
				makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),	// DsTexture::eData1
				makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),	// DsTexture::eData2
				makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),	// DsTexture::eData3
				makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),	// DsTexture::eData4
				makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),	// DsTexture::eData5
			};

			if ( hasSsao )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapSsao
			}

			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapLightDiffuse
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapLightSpecular

			if ( hasDiffuseGi )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapLightIndirectDiffuse
			}

			if ( hasSpecularGi )
			{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapLightIndirectSpecular
			}

			auto envMapCount = c_noIblEnvironmentCount;

			if ( matType != MaterialType::ePhong )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapIrradiance
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapPrefiltered
				envMapCount -= c_iblTexturesCount;
			}

			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, envMapCount ) );	// c3d_mapEnvironment

			auto result = device->createDescriptorSetLayout( "DeferredResolveTex"
				, std::move( bindings ) );
			return result;
		}

		inline ashes::DescriptorSetPtr doCreateTexDescriptorSet( RenderDevice const & device
			, ashes::DescriptorSetPool & pool
			, ashes::DescriptorSetLayout const & layout
			, OpaquePassResult const & gp
			, crg::ImageViewId const * ssao
			, crg::ImageViewId const * sssss
			, crg::ImageViewId const & lightDiffuse
			, crg::ImageViewId const & lightSpecular
			, crg::ImageViewId const * lightIndirectDiffuse
			, crg::ImageViewId const * lightIndirectSpecular
			, SamplerSPtr sampler
			, Scene const & scene
			, MaterialType matType
			, OpaqueResolvePass::EnvMapArray const & envMaps )
		{
			sampler->initialise( device );
			auto descriptorSet = pool.createDescriptorSet( "DeferredResolveTex", 1u );
			uint32_t index = 0u;
			auto imgLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			ashes::WriteDescriptorSetArray writes;

			// TODO CRG
			//for ( auto unit : gp )
			//{
			//	writes.push_back( { index++
			//		, 0u
			//		, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			//		, { { unit.sampler
			//			, unit.wholeView
			//			, imgLayout } } } );
			//}

			//if ( ssao )
			//{
			//	writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { sampler->getSampler(), *ssao, imgLayout } } } );
			//}

			//if ( sssss )
			//{
			//	writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { sampler->getSampler(), *sssss, imgLayout } } } );
			//}
			//else
			//{
			//	writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { sampler->getSampler(), lightDiffuse, imgLayout } } } );
			//}

			//writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { sampler->getSampler(), lightSpecular, imgLayout } } } );

			//if ( lightIndirectDiffuse )
			//{
			//	writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { sampler->getSampler(), *lightIndirectDiffuse, imgLayout } } } );
			//}

			//if ( lightIndirectSpecular )
			//{
			//	writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { sampler->getSampler(), *lightIndirectSpecular, imgLayout } } } );
			//}

			//auto & background = *scene.getBackground();
			//auto envMapCount = c_noIblEnvironmentCount;

			//if ( matType != MaterialType::ePhong
			//	&& background.hasIbl() )
			//{
			//	auto & ibl = background.getIbl();
			//	writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { ibl.getPrefilteredBrdfSampler(), ibl.getPrefilteredBrdfTexture(), imgLayout } } } );
			//	writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { ibl.getIrradianceSampler(), ibl.getIrradianceTexture(), imgLayout } } } );
			//	writes.push_back( { index++, 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, { { ibl.getPrefilteredEnvironmentSampler(), ibl.getPrefilteredEnvironmentTexture(), imgLayout } } } );
			//	envMapCount -= c_iblTexturesCount;
			//}

			//writes.push_back( { index++, 0u, envMapCount, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER } );
			//CU_Require( index < device.properties.limits.maxDescriptorSetSampledImages );

			//writes.back().imageInfo.reserve( envMapCount );

			//auto & envWrites = writes.back();
			//auto it = envMaps.begin();
			//uint32_t i = 0u;

			//while ( it != envMaps.end() && i < envMapCount )
			//{
			//	envWrites.imageInfo.push_back( { it->get().getTexture().getSampler()->getSampler()
			//		, it->get().getTexture().getTexture()->getDefaultView().getSampledView()
			//		, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
			//	++i;
			//	++it;
			//}

			//while ( i < envMapCount )
			//{
			//	envWrites.imageInfo.push_back( { sampler->getSampler()
			//		, background.getView()
			//		, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
			//	++i;
			//}

			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		ashes::GraphicsPipelinePtr doCreateRenderPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::PipelineShaderStageCreateInfoArray program
			, ashes::RenderPass const & renderPass
			, VkExtent2D const & size )
		{

			auto result = device->createPipeline( "DeferredResolve"
				, ashes::GraphicsPipelineCreateInfo
				{
					0u,
					std::move( program ),
					*doCreateVertexLayout(),
					ashes::PipelineInputAssemblyStateCreateInfo{},
					ashes::nullopt,
					ashes::PipelineViewportStateCreateInfo
					{
						0u,
						{ 1u, VkViewport{ 0.0f, 0.0f, float( size.width ), float( size.height ), 0.0f, 1.0f } },
						{ 1u, VkRect2D{ 0, 0, size.width, size.height } }
					},
					ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
					ashes::PipelineMultisampleStateCreateInfo{},
					ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE },
					ashes::PipelineColorBlendStateCreateInfo{},
					ashes::nullopt,
					pipelineLayout,
					renderPass,
				} );
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

	//*********************************************************************************************

	OpaqueResolvePass::ProgramPipeline::ProgramPipeline( Engine & engine
		, RenderDevice const & device
		, OpaquePassResult const & gp
		, ashes::DescriptorSetLayout const & uboLayout
		, ashes::RenderPass const & renderPass
		, crg::ImageViewId const * ssao
		, crg::ImageViewId const * subsurfaceScattering
		, crg::ImageViewId const & lightDiffuse
		, crg::ImageViewId const & lightSpecular
		, crg::ImageViewId const * lightIndirectDiffuse
		, crg::ImageViewId const * lightIndirectSpecular
		, SamplerSPtr const & sampler
		, VkExtent2D const & size
		, FogType fogType
		, MaterialType matType
		, Scene const & scene )
		: engine{ engine }
		, opaquePassResult{ gp }
		, renderPass{ &renderPass }
		, vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "DeferredResolve" }
		, pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "DeferredResolve" }
		, program{ doCreateProgram( device
			, fogType, ssao != nullptr
			, lightIndirectDiffuse != nullptr
			, lightIndirectSpecular != nullptr
			, matType
			, vertexShader
			, pixelShader ) }
		, texDescriptorLayout{ doCreateTexDescriptorLayout( device
			, ssao != nullptr
			, lightIndirectDiffuse != nullptr
			, lightIndirectSpecular != nullptr
			, engine.getMaterialsType() ) }
		, texDescriptorPool{ texDescriptorLayout->createPool( "OpaqueResolvePassTex", 1u ) }
		, texDescriptorSet{ doCreateTexDescriptorSet( device
			, *texDescriptorPool
			, *texDescriptorLayout
			, gp
			, ssao
			, subsurfaceScattering
			, lightDiffuse
			, lightSpecular
			, lightIndirectDiffuse
			, lightIndirectSpecular
			, sampler
			, scene
			, matType
			, scene.getEnvironmentMaps() ) }
		, pipelineLayout{ device->createPipelineLayout( "DeferredResolve", { uboLayout, *texDescriptorLayout } ) }
		, pipeline{ doCreateRenderPipeline( device, *pipelineLayout, program, renderPass, size ) }
		, commandBuffer{ device.graphicsCommandPool->createCommandBuffer( "DeferredResolve", VK_COMMAND_BUFFER_LEVEL_PRIMARY ) }
	{
	}

	void OpaqueResolvePass::ProgramPipeline::updateCommandBuffer( ashes::VertexBufferBase & vbo
		, ashes::DescriptorSet const & uboSet
		, ashes::FrameBuffer const & frameBuffer
		, RenderPassTimer & timer )
	{
		// TODO CRG
		//commandBuffer->begin();
		//commandBuffer->beginDebugBlock(
		//	{
		//		"Deferred - Resolve",
		//		makeFloatArray( engine.getNextRainbowColour() ),
		//	} );
		//timer.beginPass( *commandBuffer );
		//commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
		//	, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//	, opaquePassResult[DsTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) );
		//commandBuffer->beginRenderPass( *renderPass
		//	, frameBuffer
		//	, { transparentBlackClearColor }
		//	, VK_SUBPASS_CONTENTS_INLINE );
		//commandBuffer->bindPipeline( *pipeline );
		//commandBuffer->bindDescriptorSets( { uboSet, *texDescriptorSet }, *pipelineLayout );
		//commandBuffer->bindVertexBuffer( 0u, vbo.getBuffer(), 0u );
		//commandBuffer->draw( 6u );
		//commandBuffer->endRenderPass();
		//commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//	, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
		//	, opaquePassResult[DsTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeDepthStencilReadOnly( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		//timer.endPass( *commandBuffer );
		//commandBuffer->endDebugBlock();
		//commandBuffer->end();
	}

	void OpaqueResolvePass::ProgramPipeline::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( vertexShader );
		visitor.visit( pixelShader );
	}

	//*********************************************************************************************

	OpaqueResolvePass::OpaqueResolvePass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, Scene & scene
		, OpaquePassResult const & gp
		, SsaoPass const & ssao
		, Texture const & subsurfaceScattering
		, Texture const & lightDiffuse
		, Texture const & lightSpecular
		, Texture const & lightIndirectDiffuse
		, Texture const & lightIndirectSpecular
		, Texture const & result
		, SceneUbo const & sceneUbo
		, GpInfoUbo const & gpInfoUbo
		, HdrConfigUbo const & hdrConfigUbo )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_scene{ scene }
		, m_result{ result }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_hdrConfigUbo{ hdrConfigUbo }
		, m_ssao{ ssao }
		, m_sampler{ getEngine()->getDefaultSampler() }
		, m_opaquePassResult{ gp }
		, m_subsurfaceScattering{ subsurfaceScattering }
		, m_lightDiffuse{ lightDiffuse }
		, m_lightSpecular{ lightSpecular }
		, m_lightIndirectDiffuse{ lightIndirectDiffuse }
		, m_lightIndirectSpecular{ lightIndirectSpecular }
		, m_viewport{ *getEngine() }
		, m_programs{ nullptr }
	{
	}

	void OpaqueResolvePass::initialise()
	{
		auto & engine = *getEngine();
		auto & result = m_result;
		VkExtent2D size{ getExtent( result.imageId ).width
			, getExtent( result.imageId ).height };
		m_vertexBuffer = doCreateVbo( m_device );
		m_uboDescriptorLayout = doCreateUboDescriptorLayout( engine, m_device );
		m_uboDescriptorPool = m_uboDescriptorLayout->createPool( "OpaqueResolvePassUbo", 1u );
		m_uboDescriptorSet = doCreateUboDescriptorSet( engine, *m_uboDescriptorPool, m_sceneUbo, m_gpInfoUbo, m_hdrConfigUbo );
		// TODO CRG
		//m_renderPass = doCreateRenderPass( m_device, getFormat( result ) );
		//m_frameBuffer = doCreateFrameBuffer( *m_renderPass, size, result );
		m_finished = m_device->createSemaphore( "OpaqueResolvePass" );
		m_timer = std::make_shared< RenderPassTimer >( m_device, cuT( "Opaque" ), cuT( "Resolve pass" ) );
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.resize( { m_size.width, m_size.height } );
		m_viewport.update();
	}

	void OpaqueResolvePass::cleanup()
	{
		m_programs = {};
		m_timer.reset();
		m_finished.reset();
		m_uboDescriptorSet.reset();
		m_uboDescriptorPool.reset();
		m_uboDescriptorLayout.reset();
		m_vertexBuffer.reset();
	}

	void OpaqueResolvePass::update( GpuUpdater & updater )
	{
		m_voxelConeTracing = updater.voxelConeTracing;
		auto & program = getProgram();
		// TODO CRG
		//program.updateCommandBuffer( *m_vertexBuffer
		//	, *m_uboDescriptorSet
		//	, *m_frameBuffer
		//	, *m_timer );
		m_currentProgram = &program;
	}

	ashes::Semaphore const & OpaqueResolvePass::render( ashes::Semaphore const & toWait )const
	{
		auto * result = &toWait;
		RenderPassTimerBlock timerBlock{ m_timer->start() };
		timerBlock->notifyPassRender();

		m_device.graphicsQueue->submit( *m_currentProgram->commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	void OpaqueResolvePass::accept( PipelineVisitorBase & visitor )
	{
		auto & program = getProgram();
		program.accept( visitor );
	}

	OpaqueResolvePass::ProgramPipeline & OpaqueResolvePass::getProgram()
	{
		auto fog = m_scene.getFog().getType();
		bool hasSsao = m_ssao.getConfig().enabled;
		bool hasSssss = m_scene.needsSubsurfaceScattering();
		bool hasDiffuseGi = m_scene.needsGlobalIllumination() || m_voxelConeTracing;
		bool hasSpecularGi = m_voxelConeTracing;
		auto index = ( size_t( fog ) * SsaoCount * SsssCount * DiffuseGiCount * SpecularGiCount )
			+ ( ( hasSsao ? 1u : 0 ) * SsssCount * DiffuseGiCount * SpecularGiCount )
			+ ( ( hasSssss ? 1u : 0u ) * DiffuseGiCount * SpecularGiCount )
			+ ( ( hasDiffuseGi ? 1u : 0u ) * SpecularGiCount )
			+ ( hasSpecularGi ? 1u : 0u );
		auto & result = m_programs[index];

		// TODO CRG
		//if ( !result )
		//{
		//	auto & engine = *getEngine();
		//	auto & tex = *m_result.getTexture();
		//	VkExtent2D size{ tex.getWidth(), tex.getHeight() };
		//	result = std::make_unique< ProgramPipeline >( engine
		//		, m_device
		//		, m_opaquePassResult
		//		, *m_uboDescriptorLayout
		//		, *m_renderPass
		//		, ( hasSsao
		//			? &m_ssao.getResult().getTexture()->getDefaultView().getSampledView()
		//			: nullptr )
		//		, ( hasSssss
		//			? &m_subsurfaceScattering.getTexture()->getDefaultView().getSampledView()
		//			: nullptr )
		//		, m_lightDiffuse.getTexture()->getDefaultView().getSampledView()
		//		, m_lightSpecular.getTexture()->getDefaultView().getSampledView()
		//		, ( hasDiffuseGi
		//			? &m_lightIndirectDiffuse.getTexture()->getDefaultView().getSampledView()
		//			: nullptr )
		//		, ( hasSpecularGi
		//			? &m_lightIndirectSpecular.getTexture()->getDefaultView().getSampledView()
		//			: nullptr )
		//		, m_sampler
		//		, size
		//		, fog
		//		, engine.getMaterialsType()
		//		, m_scene );
		//}

		return *result;
	}
}

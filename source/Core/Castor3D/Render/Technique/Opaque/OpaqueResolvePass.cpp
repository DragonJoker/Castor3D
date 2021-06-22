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
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
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

		struct ResolveProgramConfig
		{
			ResolveProgramConfig( Scene const & scene
				, SsaoConfig const & ssao )
				: hasSsao{ ssao.enabled }
				, hasSssss{ scene.needsSubsurfaceScattering() }
				, hasDiffuseGi{ checkFlag( scene.getFlags(), SceneFlag::eLayeredLpvGI )
					|| checkFlag( scene.getFlags(), SceneFlag::eLpvGI )
					|| checkFlag( scene.getFlags(), SceneFlag::eVoxelConeTracing ) }
				, hasSpecularGi{ checkFlag( scene.getFlags(), SceneFlag::eVoxelConeTracing ) }
				, index{ ( ( hasSsao ? 1u : 0 ) * SsssCount * DiffuseGiCount * SpecularGiCount )
					+ ( ( hasSssss ? 1u : 0u ) * DiffuseGiCount * SpecularGiCount )
					+ ( ( hasDiffuseGi ? 1u : 0u ) * SpecularGiCount )
					+ ( hasSpecularGi ? 1u : 0u ) }
			{
			}

			ResolveProgramConfig( uint32_t value )
				: hasSsao{ ( ( value >> 3 ) % 2 ) != 0 }
				, hasSssss{ ( ( value >> 2 ) % 2 ) != 0 }
				, hasDiffuseGi{ ( ( value >> 1 ) % 2 ) != 0 }
				, hasSpecularGi{ ( ( value >> 0 ) % 2 ) != 0 }
				, index{ value }
			{
			}

			bool hasSsao;
			bool hasSssss;
			bool hasDiffuseGi;
			bool hasSpecularGi;
			uint32_t index;

			static size_t constexpr SsaoCount = 2u;
			static size_t constexpr SsssCount = 2u;
			static size_t constexpr DiffuseGiCount = 2u;
			static size_t constexpr SpecularGiCount = 2u;
			static size_t constexpr MaxProgramsCount = SsaoCount * SsssCount * DiffuseGiCount * SpecularGiCount;
		};

		ShaderPtr createVertexProgram()
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

		enum class ResolveBind
		{
			eMaterials,
			eScene,
			eGpInfo,
			eHdrConfig,
			eDepth,
			eData1,
			eData2,
			eData3,
			eData4,
			eData5,
			eSsao,
			eDirectDiffuse,
			eDirectSpecular,
			eIndirectDiffuse,
			eIndirectSpecular,
			eBrdf,
			eIrradiance,
			ePrefiltered,
			eEnvironment,
		};

		ShaderPtr createPhongPixelProgram( RenderSystem const & renderSystem
			, ResolveProgramConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::PhongMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( ResolveBind::eMaterials )
				, 0u );
			UBO_SCENE( writer, uint32_t( ResolveBind::eScene ), 0u );
			UBO_GPINFO( writer, uint32_t( ResolveBind::eGpInfo ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( ResolveBind::eHdrConfig ), 0u );

			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), uint32_t( ResolveBind::eDepth ), 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( ResolveBind::eData1 ), 0u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( ResolveBind::eData2 ), 0u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( ResolveBind::eData3 ), 0u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), uint32_t( ResolveBind::eData4 ), 0u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), uint32_t( ResolveBind::eData5 ), 0u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( "c3d_mapSsao", uint32_t( ResolveBind::eSsao ), 0u, config.hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", uint32_t( ResolveBind::eDirectDiffuse ), 0u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightSpecular", uint32_t( ResolveBind::eDirectSpecular ), 0u );
			auto c3d_mapLightIndirectDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectDiffuse", uint32_t( ResolveBind::eIndirectDiffuse ), 0u, config.hasDiffuseGi );
			auto c3d_mapLightIndirectSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectSpecular", uint32_t( ResolveBind::eIndirectSpecular ), 0u, config.hasSpecularGi );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( "c3d_mapEnvironment", uint32_t( ResolveBind::eEnvironment ), 0u, c_noIblEnvironmentCount );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();

			if ( config.hasSpecularGi )
			{
				utils.declareFresnelSchlick();
			}

			shader::PhongReflectionModel reflections{ writer, utils };
			shader::CommonFog fog{ writer };

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
							, config.hasDiffuseGi );
						auto lightIndirectSpecular = writer.declLocale( "lightIndirectSpecular"
							, c3d_mapLightIndirectSpecular.lod( vtx_texture, 0.0_f ).rgb()
							, config.hasSpecularGi );
						lightSpecular *= specular;

						if ( config.hasSpecularGi )
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

						if ( config.hasSsao )
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
						FI;

						pxl_fragColor = vec4( shader::PhongLightingModel::combine( lightDiffuse
								, config.hasDiffuseGi ? lightIndirectDiffuse : vec3( 0.0_f )
								, lightSpecular
								, config.hasSpecularGi ? lightIndirectSpecular : vec3( 0.0_f )
								, ambient
								, config.hasDiffuseGi ? lightIndirectDiffuse : vec3( 1.0_f )
								, occlusion
								, emissive
								, reflected + refracted
								, diffuse )
							, 1.0_f );
					}
					ELSE
					{
						pxl_fragColor = vec4( diffuse, 1.0_f );
					}
					FI;

					IF( writer, c3d_sceneData.fogType != UInt( uint32_t( FogType::eDisabled ) ) )
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
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr createPbrMRPixelProgram( RenderSystem const & renderSystem
			, ResolveProgramConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::PbrMRMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( ResolveBind::eMaterials )
				, 0u );
			UBO_SCENE( writer, uint32_t( ResolveBind::eScene ), 0u );
			UBO_GPINFO( writer, uint32_t( ResolveBind::eGpInfo ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( ResolveBind::eHdrConfig ), 0u );

			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), uint32_t( ResolveBind::eDepth ), 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( ResolveBind::eData1 ), 0u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( ResolveBind::eData2 ), 0u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( ResolveBind::eData3 ), 0u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), uint32_t( ResolveBind::eData4 ), 0u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), uint32_t( ResolveBind::eData5 ), 0u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( "c3d_mapSsao", uint32_t( ResolveBind::eSsao ), 0u, config.hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", uint32_t( ResolveBind::eDirectDiffuse ), 0u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightSpecular", uint32_t( ResolveBind::eDirectSpecular ), 0u );
			auto c3d_mapLightIndirectDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectDiffuse", uint32_t( ResolveBind::eIndirectDiffuse ), 0u, config.hasDiffuseGi );
			auto c3d_mapLightIndirectSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectSpecular", uint32_t( ResolveBind::eIndirectSpecular ), 0u, config.hasSpecularGi );
			auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf", uint32_t( ResolveBind::eBrdf ), 0u );
			auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance", uint32_t( ResolveBind::eIrradiance ), 0u );
			auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered", uint32_t( ResolveBind::ePrefiltered ), 0u );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( "c3d_mapEnvironment", uint32_t( ResolveBind::eEnvironment ), 0u, c_iblEnvironmentCount );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeIBL();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();

			if ( config.hasSpecularGi )
			{
				utils.declareFresnelSchlick();
			}

			shader::MetallicPbrReflectionModel reflections{ writer, utils };
			shader::CommonFog fog{ writer };

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
							, config.hasDiffuseGi );
						auto lightIndirectSpecular = writer.declLocale( "lightIndirectSpecular"
							, c3d_mapLightIndirectSpecular.lod( vtx_texture, 0.0_f ).rgb()
							, config.hasSpecularGi );

						if ( config.hasSpecularGi )
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

						if ( config.hasSsao )
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

						pxl_fragColor = vec4( shader::MetallicBrdfLightingModel::combine( lightDiffuse
								, config.hasDiffuseGi ? lightIndirectDiffuse : vec3( 0.0_f )
								, lightSpecular
								, config.hasSpecularGi ? lightIndirectSpecular : vec3( 0.0_f )
								, ambient
								, config.hasDiffuseGi ? lightIndirectDiffuse : vec3( 1.0_f )
								, occlusion
								, emissive
								, reflected + refracted
								, albedo )
							, 1.0_f );
					}
					ELSE
					{
						pxl_fragColor = vec4( albedo, 1.0_f );
					}
					FI;

					IF( writer, c3d_sceneData.fogType != UInt( uint32_t( FogType::eDisabled ) ) )
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
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr createPbrSGPixelProgram( RenderSystem const & renderSystem
			, ResolveProgramConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::PbrSGMaterials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( ResolveBind::eMaterials )
				, 0u );
			UBO_SCENE( writer, uint32_t( ResolveBind::eScene ), 0u );
			UBO_GPINFO( writer, uint32_t( ResolveBind::eGpInfo ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( ResolveBind::eHdrConfig ), 0u );

			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), uint32_t( ResolveBind::eDepth ), 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( ResolveBind::eData1 ), 0u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( ResolveBind::eData2 ), 0u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( ResolveBind::eData3 ), 0u );
			auto c3d_mapData4 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), uint32_t( ResolveBind::eData4 ), 0u );
			auto c3d_mapData5 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), uint32_t( ResolveBind::eData5 ), 0u );
			auto c3d_mapSsao = writer.declSampledImage< FImg2DRg32 >( "c3d_mapSsao", uint32_t( ResolveBind::eSsao ), 0u, config.hasSsao );
			auto c3d_mapLightDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightDiffuse", uint32_t( ResolveBind::eDirectDiffuse ), 0u );
			auto c3d_mapLightSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightSpecular", uint32_t( ResolveBind::eDirectSpecular ), 0u );
			auto c3d_mapLightIndirectDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectDiffuse", uint32_t( ResolveBind::eIndirectDiffuse ), 0u, config.hasDiffuseGi );
			auto c3d_mapLightIndirectSpecular = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapLightIndirectSpecular", uint32_t( ResolveBind::eIndirectSpecular ), 0u, config.hasSpecularGi );
			auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf", uint32_t( ResolveBind::eBrdf ), 0u );
			auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance", uint32_t( ResolveBind::eIrradiance ), 0u );
			auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered", uint32_t( ResolveBind::ePrefiltered ), 0u );
			auto c3d_mapEnvironment = writer.declSampledImageArray< FImgCubeRgba32 >( "c3d_mapEnvironment", uint32_t( ResolveBind::eEnvironment ), 0u, c_iblEnvironmentCount );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer };
			utils.declareRemoveGamma();
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeIBL();
			utils.declareDecodeMaterial();
			utils.declareInvertVec2Y();

			if ( config.hasSpecularGi )
			{
				utils.declareFresnelSchlick();
			}

			shader::SpecularPbrReflectionModel reflections{ writer, utils };
			shader::CommonFog fog{ writer };

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
							, config.hasDiffuseGi );
						auto lightIndirectSpecular = writer.declLocale( "lightIndirectSpecular"
							, c3d_mapLightIndirectSpecular.lod( vtx_texture, 0.0_f ).rgb()
							, config.hasSpecularGi );

						if ( config.hasSpecularGi )
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

						if ( config.hasSsao )
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

						pxl_fragColor = vec4( shader::MetallicBrdfLightingModel::combine( lightDiffuse
								, config.hasDiffuseGi ? lightIndirectDiffuse : vec3( 0.0_f )
								, lightSpecular
								, config.hasSpecularGi ? lightIndirectSpecular : vec3( 0.0_f )
								, ambient
								, config.hasDiffuseGi ? lightIndirectDiffuse : vec3( 1.0_f )
								, occlusion
								, emissive
								, reflected + refracted
								, diffuse )
							, 1.0_f );
					}
					ELSE
					{
						pxl_fragColor = vec4( diffuse, 1.0_f );
					}
					FI;

					IF( writer, c3d_sceneData.fogType != UInt( uint32_t( FogType::eDisabled ) ) )
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
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr createPixelProgram( RenderDevice const & device
			, ResolveProgramConfig const & config
			, MaterialType matType )
		{
			return ( matType == MaterialType::ePhong
				? createPhongPixelProgram( device.renderSystem, config )
				: ( matType == MaterialType::eMetallicRoughness
					? createPbrMRPixelProgram( device.renderSystem, config )
					: createPbrSGPixelProgram( device.renderSystem, config ) ) );
		}

		std::vector< crg::VkPipelineShaderStageCreateInfoArray > createPrograms( std::vector< OpaqueResolvePass::Program > const & programs )
		{
			std::vector< crg::VkPipelineShaderStageCreateInfoArray > result;

			for ( auto & program : programs )
			{
				result.push_back( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( program.stages ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	OpaqueResolvePass::OpaqueResolvePass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, Scene & scene
		, OpaquePassResult const & gp
		, SsaoConfig const & ssao
		, Texture const & ssaoResult
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
		, m_opaquePassResult{ gp }
		, m_ssaoResult{ ssaoResult }
		, m_subsurfaceScattering{ subsurfaceScattering }
		, m_lightDiffuse{ lightDiffuse }
		, m_lightSpecular{ lightSpecular }
		, m_lightIndirectDiffuse{ lightIndirectDiffuse }
		, m_lightIndirectSpecular{ lightIndirectSpecular }
		, m_programs{ doCreatePrograms() }
	{
		previousPass = &doCreatePass( graph, *previousPass );
	}

	std::vector< OpaqueResolvePass::Program > OpaqueResolvePass::doCreatePrograms()
	{
		std::vector< Program > result;

		for ( uint32_t i = 0u; i < ResolveProgramConfig::MaxProgramsCount; ++i )
		{
			ResolveProgramConfig config{ i };
			Program program{ { VK_SHADER_STAGE_VERTEX_BIT, "OpaqueResolve" + std::to_string( i ), createVertexProgram() }
				, { VK_SHADER_STAGE_FRAGMENT_BIT, "OpaqueResolve" + std::to_string( i ), createPixelProgram( m_device, config, m_scene.getMaterialsType() ) }
				, {} };
			program.stages = { makeShaderState( m_device, program.vertexShader )
				, makeShaderState( m_device, program.pixelShader ) };
			result.emplace_back( std::move( program ) );
		}

		return result;
	}

	crg::FramePass const & OpaqueResolvePass::doCreatePass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass )
	{
		auto & engine = *getEngine();
		auto & passBuffer = engine.getMaterialCache().getPassBuffer();
		auto & pass = graph.createPass( "DeferredResolve"
			, [this, &engine]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.texcoordConfig( crg::rq::Texcoord{} )
					.renderSize( makeExtent2D( m_result.getExtent() ) )
					.passIndex( &m_programIndex )
					.programs( createPrograms( m_programs ) )
					.build( pass, context, graph, uint32_t( m_programs.size() ) );
				engine.registerTimer( "Opaque"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );

		passBuffer.createPassBinding( pass
			, uint32_t( ResolveBind::eMaterials ) );
		m_sceneUbo.createPassBinding( pass
			, uint32_t( ResolveBind::eScene ) );
		m_gpInfoUbo.createPassBinding( pass
			, uint32_t( ResolveBind::eGpInfo ) );
		m_hdrConfigUbo.createPassBinding( pass
			, uint32_t( ResolveBind::eHdrConfig ) );

		pass.addSampledView( m_opaquePassResult[DsTexture::eDepth].sampledViewId
			, uint32_t( ResolveBind::eDepth )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData1].sampledViewId
			, uint32_t( ResolveBind::eData1 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData2].sampledViewId
			, uint32_t( ResolveBind::eData2 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData3].sampledViewId
			, uint32_t( ResolveBind::eData3 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData4].sampledViewId
			, uint32_t( ResolveBind::eData4 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData5].sampledViewId
			, uint32_t( ResolveBind::eData5 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_ssaoResult.sampledViewId
			, uint32_t( ResolveBind::eSsao )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_subsurfaceScattering.sampledViewId
			, uint32_t( ResolveBind::eDirectDiffuse )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_lightSpecular.sampledViewId
			, uint32_t( ResolveBind::eDirectSpecular )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_lightIndirectDiffuse.sampledViewId
			, uint32_t( ResolveBind::eIndirectDiffuse )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_lightIndirectSpecular.sampledViewId
			, uint32_t( ResolveBind::eIndirectSpecular )
			, VK_IMAGE_LAYOUT_UNDEFINED );

		auto & background = *m_scene.getBackground();
		background.initialise( m_device );
		auto envMapCount = c_noIblEnvironmentCount;

		if ( m_scene.getMaterialsType() != MaterialType::ePhong )
		{
			if ( background.hasIbl() )
			{
				auto & ibl = background.getIbl();;
				pass.addSampledView( ibl.getPrefilteredBrdfTexture().sampledViewId
					, uint32_t( ResolveBind::eBrdf )
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
				pass.addSampledView( ibl.getIrradianceTexture().sampledViewId
					, uint32_t( ResolveBind::eIrradiance )
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
				pass.addSampledView( ibl.getPrefilteredEnvironmentTexture().sampledViewId
					, uint32_t( ResolveBind::ePrefiltered )
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
				envMapCount -= c_iblTexturesCount;
			}
		}

		auto & envMaps = m_scene.getEnvironmentMaps();
		auto it = envMaps.begin();
		uint32_t i = 0u;
		crg::ImageViewIdArray envViews;

		while ( it != envMaps.end() && i < envMapCount )
		{
			envViews.push_back( it->get().getColourId().sampledViewId );
			++i;
			++it;
		}

		while ( i < envMapCount )
		{
			envViews.push_back( background.getTextureId().sampledViewId );
			++i;
		}

		pass.addSampledViews( envViews
			, uint32_t( ResolveBind::eEnvironment )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );

		pass.addInOutColourView( m_result.targetViewId );
		return pass;
	}

	void OpaqueResolvePass::update( CpuUpdater & updater )
	{
		ResolveProgramConfig config{ m_scene, m_ssao };
		m_programIndex = config.index;
	}

	void OpaqueResolvePass::accept( PipelineVisitorBase & visitor )
	{
	}
}

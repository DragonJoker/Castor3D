#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
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

			explicit ResolveProgramConfig( uint32_t value )
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

			static uint32_t constexpr SsaoCount = 2u;
			static uint32_t constexpr SsssCount = 2u;
			static uint32_t constexpr DiffuseGiCount = 2u;
			static uint32_t constexpr SpecularGiCount = 2u;
			static uint32_t constexpr MaxProgramsCount = SsaoCount * SsssCount * DiffuseGiCount * SpecularGiCount;
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

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
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
			eData0,
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
			eEnvironment,
			eBrdf,
			eIrradiance,
			ePrefiltered,
		};

		ShaderPtr createPixelProgram( RenderSystem const & renderSystem
			, ResolveProgramConfig const & config
			, PassTypeID passType )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			shader::Materials materials{ writer };
			materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( ResolveBind::eMaterials )
				, 0u );
			UBO_SCENE( writer, uint32_t( ResolveBind::eScene ), 0u );
			UBO_GPINFO( writer, uint32_t( ResolveBind::eGpInfo ), 0u );
			UBO_HDR_CONFIG( writer, uint32_t( ResolveBind::eHdrConfig ), 0u );

			auto c3d_mapData0 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData0 ), uint32_t( ResolveBind::eData0 ), 0u );
			auto c3d_mapData1 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( ResolveBind::eData1 ), 0u );
			auto c3d_mapData2 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( ResolveBind::eData2 ), 0u );
			auto c3d_mapData3 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( ResolveBind::eData3 ), 0u );
			auto c3d_mapData4 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData4 ), uint32_t( ResolveBind::eData4 ), 0u );
			auto c3d_mapData5 = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eData5 ), uint32_t( ResolveBind::eData5 ), 0u );
			auto c3d_mapSsao = writer.declCombinedImg< FImg2DRg32 >( "c3d_mapSsao", uint32_t( ResolveBind::eSsao ), 0u, config.hasSsao );
			auto c3d_mapLightDiffuse = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightDiffuse", uint32_t( ResolveBind::eDirectDiffuse ), 0u );
			auto c3d_mapLightSpecular = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightSpecular", uint32_t( ResolveBind::eDirectSpecular ), 0u );
			auto c3d_mapLightIndirectDiffuse = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightIndirectDiffuse", uint32_t( ResolveBind::eIndirectDiffuse ), 0u, config.hasDiffuseGi );
			auto c3d_mapLightIndirectSpecular = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightIndirectSpecular", uint32_t( ResolveBind::eIndirectSpecular ), 0u, config.hasSpecularGi );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::Utils utils{ writer, *renderSystem.getEngine() };
			shader::CookTorranceBRDF cookTorrance{ writer, utils };

			auto lightingModel = utils.createLightingModel( shader::getLightingModelName( *renderSystem.getEngine(), passType )
				, {}
				, true
				, renderSystem.getGpuInformations().hasShaderStorageBuffers() );
			auto reflections = lightingModel->getReflectionModel( uint32_t( ResolveBind::eEnvironment )
				, 0u );
			shader::Fog fog{ writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto data0 = writer.declLocale( "data0"
						, c3d_mapData0.lod( vtx_texture, 0.0_f ) );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( data0.w() ) );

					IF( writer, materialId == 0_u )
					{
						writer.demote();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, c3d_mapData1.lod( vtx_texture, 0.0_f ) );
					auto data2 = writer.declLocale( "data2"
						, c3d_mapData2.lod( vtx_texture, 0.0_f ) );
					auto data5 = writer.declLocale( "data5"
						, c3d_mapData5.lod( vtx_texture, 0.0_f ) );
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
						, data0.x() );
					auto albedo = writer.declLocale( "albedo"
						, data2.rgb() );
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( c3d_gpInfoData.projToWorld( utils, vtx_texture, depth )
						, data1.rgb() );
					surface.texCoord.xy() = vtx_texture;

					IF( writer, lighting )
					{
						auto data3 = writer.declLocale( "data3"
							, c3d_mapData3.lod( vtx_texture, 0.0_f ) );
						auto data4 = writer.declLocale( "data4"
							, c3d_mapData4.lod( vtx_texture, 0.0_f ) );

						auto material = writer.declLocale( "material"
							, materials.getMaterial( materialId ) );
						auto occlusion = writer.declLocale( "occlusion"
							, data5.a() );
						auto emissive = writer.declLocale( "emissive"
							, data4.xyz() );
						auto lightMat = lightingModel->declMaterial( "lightMat" );
						lightMat->create( albedo
							, data3
							, data2
							, material );

						auto ambient = writer.declLocale( "ambient"
							, lightMat->getAmbient( c3d_sceneData.ambientLight ) );
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
						lightMat->adjustDirectSpecular( lightSpecular );

						if ( config.hasSsao )
						{
							occlusion *= c3d_mapSsao.lod( vtx_texture, 0.0_f ).r();
						}

						auto reflected = writer.declLocale( "reflected"
							, vec3( 0.0_f ) );
						auto refracted = writer.declLocale( "refracted"
							, vec3( 0.0_f ) );
						reflections->computeDeferred( *lightMat
							, surface
							, c3d_sceneData
							, envMapIndex
							, reflection
							, refraction
							, material.refractionRatio
							, material.transmission
							, ambient
							, reflected
							, refracted );
						auto indirectAmbient = writer.declLocale( "indirectAmbient"
							, lightMat->getIndirectAmbient( config.hasDiffuseGi ? lightIndirectDiffuse : vec3( 1.0_f ) ) );
						auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
							, ( config.hasDiffuseGi
								? cookTorrance.computeDiffuse( lightIndirectDiffuse
									, c3d_sceneData.cameraPosition
									, surface.worldNormal
									, lightMat->specular
									, lightMat->getMetalness()
									, surface )
								: vec3( 0.0_f ) ) );
						pxl_fragColor = vec4( lightingModel->combine( lightDiffuse
								, indirectDiffuse
								, lightSpecular
								, config.hasSpecularGi ? lightIndirectSpecular : vec3( 0.0_f )
								, ambient
								, indirectAmbient
								, occlusion
								, emissive
								, reflected
								, refracted
								, lightMat->albedo )
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
							, depth );
						pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData )
							, pxl_fragColor
							, surface.worldPosition
							, c3d_sceneData );
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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

	OpaqueResolvePass::OpaqueResolvePass( crg::FramePassGroup & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, ProgressBar * progress
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
		, m_lightSpecular{ lightSpecular }
		, m_lightIndirectDiffuse{ lightIndirectDiffuse }
		, m_lightIndirectSpecular{ lightIndirectSpecular }
		, m_programs{ doCreatePrograms() }
	{
		previousPass = &doCreatePass( graph, *previousPass, progress );
	}

	std::vector< OpaqueResolvePass::Program > OpaqueResolvePass::doCreatePrograms()
	{
		std::vector< Program > result;

		for ( uint32_t i = 0u; i < ResolveProgramConfig::MaxProgramsCount; ++i )
		{
			ResolveProgramConfig config{ i };
			Program program{ { VK_SHADER_STAGE_VERTEX_BIT, "OpaqueResolve" + std::to_string( i ), createVertexProgram() }
				, { VK_SHADER_STAGE_FRAGMENT_BIT, "OpaqueResolve" + std::to_string( i ), createPixelProgram( m_device.renderSystem, config, m_scene.getPassesType() ) }
				, {} };
			program.stages = { makeShaderState( m_device, program.vertexShader )
				, makeShaderState( m_device, program.pixelShader ) };
			result.emplace_back( std::move( program ) );
		}

		return result;
	}

	crg::FramePass const & OpaqueResolvePass::doCreatePass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating opaque resolve pass" );
		auto & engine = *getEngine();
		auto & passBuffer = engine.getMaterialCache().getPassBuffer();
		auto & pass = graph.createPass( "Resolve"
			, [this, progress, &engine]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising opaque resolve pass" );
				auto result = crg::RenderQuadBuilder{}
					.texcoordConfig( crg::Texcoord{} )
					.renderSize( makeExtent2D( m_result.getExtent() ) )
					.passIndex( &m_programIndex )
					.programs( createPrograms( m_programs ) )
					.build( framePass, context, runnableGraph, { uint32_t( m_programs.size() ) } );
				engine.registerTimer( framePass.getFullName()
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

		pass.addSampledView( m_opaquePassResult[DsTexture::eData0].sampledViewId
			, uint32_t( ResolveBind::eData0 ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData1].sampledViewId
			, uint32_t( ResolveBind::eData1 ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData2].sampledViewId
			, uint32_t( ResolveBind::eData2 ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData3].sampledViewId
			, uint32_t( ResolveBind::eData3 ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData4].sampledViewId
			, uint32_t( ResolveBind::eData4 ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eData5].sampledViewId
			, uint32_t( ResolveBind::eData5 ) );
		pass.addSampledView( m_ssaoResult.sampledViewId
			, uint32_t( ResolveBind::eSsao ) );
		pass.addSampledView( m_subsurfaceScattering.sampledViewId
			, uint32_t( ResolveBind::eDirectDiffuse ) );
		pass.addSampledView( m_lightSpecular.sampledViewId
			, uint32_t( ResolveBind::eDirectSpecular ) );
		pass.addSampledView( m_lightIndirectDiffuse.sampledViewId
			, uint32_t( ResolveBind::eIndirectDiffuse ) );
		pass.addSampledView( m_lightIndirectSpecular.sampledViewId
			, uint32_t( ResolveBind::eIndirectSpecular ) );

		auto & background = *m_scene.getBackground();
		background.initialise( m_device );

		if ( m_scene.getEngine()->getPassFactory().hasIBL( m_scene.getPassesType() ) )
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
			}
		}

		pass.addSampledView( m_scene.getEnvironmentMap().getColourId().sampledViewId
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
		auto & program = m_programs[m_programIndex];
		visitor.visit( program.vertexShader );
		visitor.visit( program.pixelShader );
	}
}

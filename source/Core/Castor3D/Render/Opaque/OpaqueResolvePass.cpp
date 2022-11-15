#include "Castor3D/Render/Opaque/OpaqueResolvePass.hpp"

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
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"
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
	namespace dropqrslv
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

		static ShaderPtr createVertexProgram()
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
			eModels,
			eScene,
			eGpInfo,
			eHdrConfig,
			eDepthObj,
			eNmlOcc,
			eColMtl,
			eSpcRgh,
			eEmsTrn,
			eClrCot,
			eCrTsIr,
			eSheen,
			eSsao,
			eBrdf,
			eDirectDiffuse,
			eDirectSpecular,
			eDirectScattering,
			eDirectCoatingSpecular,
			eDirectSheen,
			eIndirectDiffuse,
			eIndirectSpecular,
			eEnvironment,
			// Last fixed index
			eBackground,
		};

		static ShaderPtr createPixelProgram( RenderSystem const & renderSystem
			, Scene const & scene
			, ResolveProgramConfig const & config
			, PassTypeID passType
			, VkExtent3D const & size )
		{
			using namespace sdw;
			FragmentWriter writer;

			shader::Utils utils{ writer };
			shader::BRDFHelpers brdf{ writer };
			shader::PassShaders passShaders{ renderSystem.getEngine()->getPassComponentsRegister()
				, TextureCombine{}
				, ( ComponentModeFlag::eColour
					| ComponentModeFlag::eNormals
					| ComponentModeFlag::eGeometry
					| ComponentModeFlag::eDiffuseLighting
					| ComponentModeFlag::eSpecularLighting
					| ComponentModeFlag::eOpacity
					| ComponentModeFlag::eOcclusion
					| ComponentModeFlag::eSpecifics )
				, utils };

			// Shader inputs
			shader::Materials materials{ writer
				, passShaders
				, uint32_t( ResolveBind::eMaterials )
				, 0u };
			C3D_ModelsData( writer, ResolveBind::eModels, 0u );
			C3D_Scene( writer, ResolveBind::eScene, 0u );
			C3D_GpInfo( writer, ResolveBind::eGpInfo, 0u );
			C3D_HdrConfig( writer, ResolveBind::eHdrConfig, 0u );

			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", uint32_t( ResolveBind::eDepthObj ), 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eNmlOcc ), uint32_t( ResolveBind::eNmlOcc ), 0u );
			auto c3d_mapColMtl = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eColMtl ), uint32_t( ResolveBind::eColMtl ), 0u );
			auto c3d_mapSpcRgh = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eSpcRgh ), uint32_t( ResolveBind::eSpcRgh ), 0u );
			auto c3d_mapEmsTrn = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eEmsTrn ), uint32_t( ResolveBind::eEmsTrn ), 0u );
			auto c3d_mapClrCot = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eClrCot ), uint32_t( ResolveBind::eClrCot ), 0u );
			auto c3d_mapCrTsIr = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eCrTsIr ), uint32_t( ResolveBind::eCrTsIr ), 0u );
			auto c3d_mapSheen = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eSheen ), uint32_t( ResolveBind::eSheen ), 0u );
			auto c3d_mapSsao = writer.declCombinedImg< FImg2DRg32 >( "c3d_mapSsao", uint32_t( ResolveBind::eSsao ), 0u, config.hasSsao );
			auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBrdf", uint32_t( ResolveBind::eBrdf ), 0u );
			auto c3d_mapLightDiffuse = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightDiffuse", uint32_t( ResolveBind::eDirectDiffuse ), 0u );
			auto c3d_mapLightSpecular = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightSpecular", uint32_t( ResolveBind::eDirectSpecular ), 0u );
			auto c3d_mapLightScattering = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightScattering", uint32_t( ResolveBind::eDirectScattering ), 0u );
			auto c3d_mapLightCoatingSpecular = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightCoatingSpecular", uint32_t( ResolveBind::eDirectCoatingSpecular ), 0u );
			auto c3d_mapLightSheen = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightSheen", uint32_t( ResolveBind::eDirectSheen ), 0u );
			auto c3d_mapLightIndirectDiffuse = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightIndirectDiffuse", uint32_t( ResolveBind::eIndirectDiffuse ), 0u, config.hasDiffuseGi );
			auto c3d_mapLightIndirectSpecular = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapLightIndirectSpecular", uint32_t( ResolveBind::eIndirectSpecular ), 0u, config.hasSpecularGi );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			shader::CookTorranceBRDF cookTorrance{ writer, utils, brdf };

			auto lightingModel = utils.createLightingModel( *renderSystem.getEngine()
				, materials
				, brdf
				, shader::getLightingModelName( *renderSystem.getEngine(), passType )
				, {}
				, nullptr
				, true );
			auto index = uint32_t( ResolveBind::eEnvironment );
			auto reflections = lightingModel->getReflectionModel( index
				, 0u );
			auto backgroundModel = shader::BackgroundModel::createModel( scene
				, writer
				, utils
				, makeExtent2D( size )
				, true
				, index
				, 0u );
			shader::Fog fog{ writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( vtx_texture, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< UInt >( depthObj.z() ) );

					IF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					FI;

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
					auto materialId = writer.declLocale( "materialId"
						, writer.cast< UInt >( modelData.getMaterialId() ) );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( materialId ) );
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( vtx_texture, 0.0_f ) );
					auto colMtl = writer.declLocale( "colMtl"
						, c3d_mapColMtl.lod( vtx_texture, 0.0_f ) );
					auto clrCot = writer.declLocale( "clrCot"
						, c3d_mapClrCot.lod( vtx_texture, 0.0_f ) );
					auto crTsIr = writer.declLocale( "crTsIr"
						, c3d_mapCrTsIr.lod( vtx_texture, 0.0_f ) );
					auto sheen = writer.declLocale( "sheen"
						, c3d_mapSheen.lod( vtx_texture, 0.0_f ) );
					auto envMapIndex = writer.declLocale( "envMapIndex"
						, modelData.getEnvMapIndex() );
					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto albedo = writer.declLocale( "albedo"
						, colMtl.rgb() );
					auto surface = writer.declLocale( "surface"
						, shader::Surface{ vec3( in.fragCoord.xy(), depth )
							, vec3( 0.0_f )
							, c3d_gpInfoData.projToWorld( utils, vtx_texture, depth )
							, nmlOcc.rgb()
							, vec3( vtx_texture, 0.0_f ) } );

					IF( writer, material.lighting != 0_u )
					{
						auto spcRgh = writer.declLocale( "spcRgh"
							, c3d_mapSpcRgh.lod( vtx_texture, 0.0_f ) );
						auto emsTrn = writer.declLocale( "emsTrn"
							, c3d_mapEmsTrn.lod( vtx_texture, 0.0_f ) );

						auto occlusion = writer.declLocale( "occlusion"
							, ( config.hasSsao
								? nmlOcc.w() * c3d_mapSsao.lod( vtx_texture, 0.0_f ).r()
								: nmlOcc.w() ) );
						auto emissive = writer.declLocale( "emissive"
							, emsTrn.xyz() );
						materials.fill( albedo, spcRgh, colMtl, crTsIr, sheen, material );
						auto components = writer.declLocale( "components"
							, shader::BlendComponents{ materials
								, material
								, surface
								, clrCot } );

						auto directAmbient = writer.declLocale( "directAmbient"
							, c3d_sceneData.ambientLight );
						auto lightDiffuse = writer.declLocale( "lightDiffuse"
							, c3d_mapLightDiffuse.lod( vtx_texture, 0.0_f ).xyz() );
						auto lightSpecular = writer.declLocale( "lightSpecular"
							, c3d_mapLightSpecular.lod( vtx_texture, 0.0_f ).xyz() );
						auto lightScattering = writer.declLocale( "lightScattering"
							, c3d_mapLightScattering.lod( vtx_texture, 0.0_f ).xyz() );
						auto lightCoatingSpecular = writer.declLocale( "lightCoatingSpecular"
							, c3d_mapLightCoatingSpecular.lod( vtx_texture, 0.0_f ).xyz() );
						auto lightSheen = writer.declLocale( "lightSheen"
							, c3d_mapLightSheen.lod( vtx_texture, 0.0_f ).xy() );
						auto lightIndirectDiffuse = writer.declLocale( "lightIndirectDiffuse"
							, c3d_mapLightIndirectDiffuse.lod( vtx_texture, 0.0_f ).rgb()
							, config.hasDiffuseGi );
						auto lightIndirectSpecular = writer.declLocale( "lightIndirectSpecular"
							, c3d_mapLightIndirectSpecular.lod( vtx_texture, 0.0_f ).rgb()
							, config.hasSpecularGi );
						components.finish( passShaders
							, surface
							, utils
							, c3d_sceneData.cameraPosition );

						auto incident = writer.declLocale( "incident"
							, reflections->computeIncident( surface.worldPosition.xyz(), c3d_sceneData.cameraPosition ) );
						auto reflectedDiffuse = writer.declLocale( "reflectedDiffuse"
							, vec3( 0.0_f ) );
						auto reflectedSpecular = writer.declLocale( "reflectedSpecular"
							, vec3( 0.0_f ) );
						auto refracted = writer.declLocale( "refracted"
							, vec3( 0.0_f ) );
						auto coatReflected = writer.declLocale( "coatReflected"
							, vec3( 0.0_f ) );
						auto sheenReflected = writer.declLocale( "sheenReflected"
							, vec3( 0.0_f ) );
						reflections->computeCombined( components
							, incident
							, *backgroundModel
							, envMapIndex
							, components.hasReflection
							, components.refractionRatio
							, directAmbient
							, reflectedDiffuse
							, reflectedSpecular
							, refracted
							, coatReflected
							, sheenReflected );
						auto indirectAmbient = writer.declLocale( "indirectAmbient"
							, config.hasDiffuseGi ? lightIndirectDiffuse : vec3( 1.0_f ) );
						auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
							, ( config.hasDiffuseGi
								? cookTorrance.computeDiffuse( lightIndirectDiffuse
									, c3d_sceneData.cameraPosition
									, components.normal
									, components.specular
									, components.metalness
									, surface )
								: vec3( 0.0_f ) ) );

						IF( writer, material.hasTransmission != 0_u )
						{
							refracted = vec3( 0.0_f );
						}
						FI;

						pxl_fragColor = vec4( lightingModel->combine( components
								, incident
								, lightDiffuse
								, indirectDiffuse
								, lightSpecular
								, lightScattering
								, lightCoatingSpecular
								, lightSheen
								, config.hasSpecularGi ? lightIndirectSpecular : vec3( 0.0_f )
								, directAmbient
								, indirectAmbient
								, occlusion
								, emissive
								, reflectedDiffuse
								, reflectedSpecular
								, refracted
								, coatReflected
								, sheenReflected )
							, 1.0_f );
					}
					ELSE
					{
						pxl_fragColor = vec4( albedo, 1.0_f );
					}
					FI;

					IF( writer, c3d_sceneData.fogType != UInt( uint32_t( FogType::eDisabled ) ) )
					{
						surface.viewPosition = vec4( c3d_gpInfoData.projToView( utils
								, vtx_texture
								, depth )
							, 1.0_f );
						pxl_fragColor = fog.apply( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData )
							, pxl_fragColor
							, surface.worldPosition.xyz()
							, c3d_sceneData );
					}
					FI;

					auto linearDepth = writer.declLocale( "linearDepth"
						, min( depthObj.y(), c3d_sceneData.farPlane ) );
					backgroundModel->applyVolume( in.fragCoord.xy()
						, linearDepth
						, c3d_sceneData.renderSize
						, c3d_sceneData.cameraPlanes
						, pxl_fragColor );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	OpaqueResolvePass::OpaqueResolvePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, ProgressBar * progress
		, Scene & scene
		, Texture const & depthObj
		, OpaquePassResult const & gp
		, SsaoConfig const & ssao
		, Texture const & ssaoResult
		, Texture const & subsurfaceScattering
		, LightPassResult const & lighting
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
		, m_depthObj{ depthObj }
		, m_opaquePassResult{ gp }
		, m_ssaoResult{ ssaoResult }
		, m_subsurfaceScattering{ subsurfaceScattering }
		, m_lighting{ lighting }
	{
		m_programs.resize( dropqrslv::ResolveProgramConfig::MaxProgramsCount );
		m_lastPass = &doCreatePass( graph, previousPasses, progress );
	}

	OpaqueResolvePass::Program & OpaqueResolvePass::doCreateProgram( uint32_t programIndex )
	{
		CU_Require( programIndex < dropqrslv::ResolveProgramConfig::MaxProgramsCount );

		if ( !m_programs[programIndex] )
		{
			dropqrslv::ResolveProgramConfig config{ programIndex };
			m_programs[programIndex] = std::make_unique< Program >( ShaderModule{ VK_SHADER_STAGE_VERTEX_BIT
					, "OpaqueResolve" + std::to_string( programIndex )
					, dropqrslv::createVertexProgram() }
				, ShaderModule{ VK_SHADER_STAGE_FRAGMENT_BIT
					, "OpaqueResolve" + std::to_string( programIndex )
					, dropqrslv::createPixelProgram( m_device.renderSystem, m_scene, config, m_scene.getPassesType(), m_result.getExtent() ) } );
			m_programs[programIndex]->stages = { makeShaderState( m_device, m_programs[programIndex]->vertexShader )
				, makeShaderState( m_device, m_programs[programIndex]->pixelShader ) };
		}

		return *m_programs[programIndex];
	}

	crg::FramePass const & OpaqueResolvePass::doCreatePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating opaque resolve pass" );
		auto & engine = *getEngine();
		auto & passBuffer = engine.getMaterialCache().getPassBuffer();
		auto & modelBuffer = m_scene.getModelBuffer().getBuffer();
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
					.programCreator( { dropqrslv::ResolveProgramConfig::MaxProgramsCount
						, [this]( uint32_t programIndex )
						{
							auto & program = doCreateProgram( programIndex );
							return crg::makeVkArray< VkPipelineShaderStageCreateInfo >( program.stages );
						} } )
					.build( framePass, context, runnableGraph, { uint32_t( m_programs.size() ) } );
				engine.registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );

		passBuffer.createPassBinding( pass
			, uint32_t( dropqrslv::ResolveBind::eMaterials ) );
		pass.addInputStorageBuffer( { modelBuffer, "Models" }
			, uint32_t( dropqrslv::ResolveBind::eModels )
			, 0u
			, uint32_t( modelBuffer.getSize() ) );
		m_sceneUbo.createPassBinding( pass
			, uint32_t( dropqrslv::ResolveBind::eScene ) );
		m_gpInfoUbo.createPassBinding( pass
			, uint32_t( dropqrslv::ResolveBind::eGpInfo ) );
		m_hdrConfigUbo.createPassBinding( pass
			, uint32_t( dropqrslv::ResolveBind::eHdrConfig ) );

		pass.addSampledView( m_depthObj.sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eDepthObj ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eNmlOcc].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eNmlOcc ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eColMtl].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eColMtl ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eSpcRgh].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eSpcRgh ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eEmsTrn].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eEmsTrn ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eClrCot].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eClrCot ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eCrTsIr].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eCrTsIr ) );
		pass.addSampledView( m_opaquePassResult[DsTexture::eSheen].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eSheen ) );
		pass.addSampledView( m_ssaoResult.sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eSsao ) );
		pass.addSampledView( m_device.renderSystem.getPrefilteredBrdfTexture().sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eBrdf )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
		pass.addSampledView( m_subsurfaceScattering.sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eDirectDiffuse ) );
		pass.addSampledView( m_lighting[LpTexture::eSpecular].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eDirectSpecular ) );
		pass.addSampledView( m_lighting[LpTexture::eScattering].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eDirectScattering ) );
		pass.addSampledView( m_lighting[LpTexture::eCoatingSpecular].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eDirectCoatingSpecular ) );
		pass.addSampledView( m_lighting[LpTexture::eSheen].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eDirectSheen ) );
		pass.addSampledView( m_lighting[LpTexture::eIndirectDiffuse].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eIndirectDiffuse ) );
		pass.addSampledView( m_lighting[LpTexture::eIndirectSpecular].sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eIndirectSpecular ) );

		pass.addSampledView( m_scene.getEnvironmentMap().getColourId().sampledViewId
			, uint32_t( dropqrslv::ResolveBind::eEnvironment )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
		auto & background = *m_scene.getBackground();
		background.initialise( m_device );
		auto index = uint32_t( dropqrslv::ResolveBind::eBackground );
		background.addPassBindings( pass, *m_result.imageId.data, index );

		pass.addInOutColourView( m_result.targetViewId );
		return pass;
	}

	void OpaqueResolvePass::update( CpuUpdater & updater )
	{
		dropqrslv::ResolveProgramConfig config{ m_scene, m_ssao };
		m_programIndex = config.index;
	}

	void OpaqueResolvePass::accept( PipelineVisitorBase & visitor )
	{
		auto & program = m_programs[m_programIndex];

		if ( program )
		{
			visitor.visit( program->vertexShader );
			visitor.visit( program->pixelShader );
		}
	}
}

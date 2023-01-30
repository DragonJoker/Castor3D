#include "Castor3D/Render/Opaque/IndirectLightingPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, IndirectLightingPass )

namespace castor3d
{
	namespace drindirlgtpass
	{
		static ShaderPtr getVertexShaderSource()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPixelShaderSource( RenderSystem const & renderSystem
			, IndirectLightingPass::Config const & config )
		{
			using namespace sdw;
			FragmentWriter writer;
			shader::Utils utils{ writer };
			shader::BRDFHelpers brdf{ writer };

			// Shader outputs
			auto outIndirectDiffuse = writer.declOutput< Vec3 >( "outIndirectDiffuse", 0 );
			auto outIndirectSpecular = writer.declOutput< Vec3 >( "outIndirectSpecular", 1 );

			// Shader inputs
			C3D_ModelsData( writer, IndirectLightingPass::eModels, 0u );
			C3D_GpInfo( writer, IndirectLightingPass::eGpInfo, 0u );
			C3D_Scene( writer, IndirectLightingPass::eScene, 0u );
			auto index = uint32_t( IndirectLightingPass::eCount );
			shader::PassShaders passShaders{ renderSystem.getEngine()->getPassComponentsRegister()
				, TextureCombine{}
				, ComponentModeFlag::eDiffuseLighting | ComponentModeFlag::eSpecularLighting
				, utils };
			shader::Materials materials{ *renderSystem.getEngine()
				, writer
				, passShaders
				, IndirectLightingPass::eMaterials
				, 0u
				, index };
			uint32_t vctIndex = uint32_t( IndirectLightingPass::eVctStart );
			uint32_t lpvIndex = uint32_t( IndirectLightingPass::eLpvStart );
			uint32_t llpvIndex = uint32_t( IndirectLightingPass::eLayeredLpvStart );
			shader::GlobalIllumination indirect{ writer
				, utils
				, uint32_t( IndirectLightingPass::eVoxelData )
				, uint32_t( IndirectLightingPass::eLpvGridConfig )
				, uint32_t( IndirectLightingPass::eLayeredLpvGridConfig )
				, vctIndex
				, lpvIndex
				, llpvIndex
				, 0u
				, config.sceneFlags
				, true};
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", uint32_t( IndirectLightingPass::eDepthObj ), 0u );
			auto c3d_mapNmlOcc = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eNmlOcc ), uint32_t( IndirectLightingPass::eNmlOcc ), 0u );
			auto c3d_mapColMtl = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eColMtl ), uint32_t( IndirectLightingPass::eColMtl ), 0u );
			auto c3d_mapSpcRgh = writer.declCombinedImg< FImg2DRgba32 >( getTextureName( DsTexture::eSpcRgh ), uint32_t( IndirectLightingPass::eSpcRgh ), 0u );
			auto c3d_brdfMap = writer.declCombinedImg< FImg2DRgba32 >( "c3d_brdfMap", uint32_t( IndirectLightingPass::eBrdf ), 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, c3d_gpInfoData.calcTexCoord( utils
							, in.fragCoord.xy() ) );
					auto depthObj = writer.declLocale( "depthObj"
						, c3d_mapDepthObj.lod( texCoord, 0.0_f ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( depthObj.z() ) );

					IF( writer, nodeId == 0u )
					{
						writer.demote();
					}
					FI;

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( modelData.getMaterialId() ) );
					auto shadowReceiver = writer.declLocale( "shadowReceiver"
						, modelData.isShadowReceiver() );
					auto nmlOcc = writer.declLocale( "nmlOcc"
						, c3d_mapNmlOcc.lod( texCoord, 0.0_f ) );
					auto colMtl = writer.declLocale( "colMtl"
						, c3d_mapColMtl.lod( texCoord, 0.0_f ) );
					auto spcRgh = writer.declLocale( "spcRgh"
						, c3d_mapSpcRgh.lod( texCoord, 0.0_f ) );
					auto depth = writer.declLocale( "depth"
						, depthObj.x() );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_gpInfoData.projToView( utils, texCoord, depth ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );

					IF( writer, material.lighting != 0_u )
					{
						auto clrCot = writer.declLocale( "clrCot"
							, vec4( 0.0_f ) );
						auto crTsIr = writer.declLocale( "crTsIr"
							, vec4( 0.0_f ) );
						auto sheen = writer.declLocale( "sheen"
							, vec4( 0.0_f ) );
						auto wsNormal = writer.declLocale( "wsNormal"
							, normalize( nmlOcc.xyz() ) );
						auto surface = writer.declLocale( "surface"
							, shader::Surface{ vec3( in.fragCoord.xy(), depth )
								, vsPosition
								, wsPosition
								, wsNormal } );
						materials.fill( colMtl.rgb(), spcRgh, colMtl, crTsIr, sheen, material );
						auto components = writer.declLocale( "components"
							, shader::BlendComponents{ materials
								, material
								, surface
								, clrCot } );

						auto lightSurface = shader::LightSurface::create( writer
							, "lightSurface"
							, c3d_sceneData.cameraPosition()
							, surface.worldPosition.xyz()
							, surface.viewPosition.xyz()
							, surface.clipPosition
							, surface.normal
							, false, false, false );
						auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
							, 1.0_f );
						auto indirectDiffuse = indirect.computeDiffuse( config.sceneFlags
							, lightSurface
							, indirectOcclusion );
						auto indirectSpecular = indirect.computeSpecular( config.sceneFlags
							, lightSurface
							, components.roughness
							, indirectOcclusion
							, indirectDiffuse.w()
							, c3d_brdfMap );
						outIndirectDiffuse = indirectDiffuse.xyz();
						outIndirectSpecular = indirectSpecular;
					}
					ELSE
					{
						outIndirectDiffuse = vec3( 0.0_f, 0.0_f, 0.0_f );
						outIndirectSpecular = vec3( 0.0_f, 0.0_f, 0.0_f );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static uint32_t getIndex( SceneFlags sceneFlags )
		{
			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				return 1u;
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI )
				&& !checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				return 2u;
			}

			if ( !checkFlag( sceneFlags, SceneFlag::eLpvGI )
				&& checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				return 3u;
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI )
				&& checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				return 4u;
			}

			return 0u;
		}

		static SceneFlags adjustSceneFlags( SceneFlags sceneFlags )
		{
			remFlag( sceneFlags, SceneFlag::eFogLinear );
			remFlag( sceneFlags, SceneFlag::eFogExponential );
			remFlag( sceneFlags, SceneFlag::eFogSquaredExponential );
			remFlag( sceneFlags, SceneFlag::eShadowAny );
			return sceneFlags;
		}
	}

	//*********************************************************************************************

	IndirectLightingPass::Config::Config( SceneFlags const & psceneFlags )
		: sceneFlags{ drindirlgtpass::adjustSceneFlags( psceneFlags ) }
		, voxels{ checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) }
		, lpv{ checkFlag( sceneFlags, SceneFlag::eLpvGI ) }
		, llpv{ checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) }
		, index{ drindirlgtpass::getIndex( sceneFlags ) }
	{
	}

	IndirectLightingPass::Config::Config( uint32_t pindex )
		: sceneFlags{ SceneFlag::eNone }
		, voxels{ pindex == uint32_t( ProgramType::eVCT ) }
		, lpv{ ( pindex == uint32_t( ProgramType::eLPV ) ) || ( pindex == uint32_t( ProgramType::eLPVLLPV ) ) }
		, llpv{ ( pindex == uint32_t( ProgramType::eLLPV ) ) || ( pindex == uint32_t( ProgramType::eLPVLLPV ) ) }
		, index{ pindex }
	{
		if ( voxels )
		{
			addFlag( sceneFlags, SceneFlag::eVoxelConeTracing );
		}

		if ( lpv )
		{
			addFlag( sceneFlags, SceneFlag::eLpvGI );
		}

		if ( llpv )
		{
			addFlag( sceneFlags, SceneFlag::eLayeredLpvGI );
		}
	}

	//*********************************************************************************************

	IndirectLightingPass::Program::Program( RenderDevice const & device
		, Scene const & scene
		, Config const & config )
		: vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, "IndirectLightingPass"
			, drindirlgtpass::getVertexShaderSource() }
		, pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, "IndirectLightingPass"
			, drindirlgtpass::getPixelShaderSource( device.renderSystem, config ) }
		, stages{ makeShaderState( device, vertexShader )
			, makeShaderState( device, pixelShader ) }
	{
	}

	//*********************************************************************************************

	IndirectLightingPass::IndirectLightingPass( RenderDevice const & device
		, ProgressBar * progress
		, Scene const & scene
		, crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, Texture const & brdf
		, Texture const & depthObj
		, OpaquePassResult const & gpResult
		, LightPassResult const & lpResult
		, LightVolumePassResult const & lpvResult
		, LightVolumePassResultArray const & llpvResult
		, Texture const & vctFirstBounce
		, Texture const & vctSecondaryBounce
		, SceneUbo const & sceneUbo
		, GpInfoUbo const & gpInfoUbo
		, LpvGridConfigUbo const & lpvConfigUbo
		, LayeredLpvGridConfigUbo const & llpvConfigUbo
		, VoxelizerUbo const & vctConfigUbo )
		: m_device{ device }
		, m_scene{ scene }
		, m_brdf{ brdf }
		, m_depthObj{ depthObj }
		, m_gpResult{ gpResult }
		, m_lpResult{ lpResult }
		, m_lpvResult{ lpvResult }
		, m_llpvResult{ llpvResult }
		, m_vctFirstBounce{ vctFirstBounce }
		, m_vctSecondaryBounce{ vctSecondaryBounce }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
		, m_vctConfigUbo{ vctConfigUbo }
		, m_group{ graph.createPassGroup( "IndirectLighting" ) }
	{
		m_programs.resize( size_t( IndirectLightingPass::ProgramType::eCount ) );
		m_lastPass = &doCreateLightingPass( m_group, scene, previousPass, progress );
		m_group.addGroupOutput( m_lpResult[LpTexture::eIndirectDiffuse].wholeViewId );
		m_group.addGroupOutput( m_lpResult[LpTexture::eIndirectSpecular].wholeViewId );
	}

	void IndirectLightingPass::update( CpuUpdater & updater )
	{
		m_programIndex = Config{ updater.camera->getScene()->getFlags() }.index;
		m_enabled = m_programIndex > 0u;
	}

	void IndirectLightingPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( "Light Indirect Diffuse"
			, m_lpResult[LpTexture::eIndirectDiffuse]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Indirect Specular"
			, m_lpResult[LpTexture::eIndirectSpecular]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
	}

	crg::FramePass const & IndirectLightingPass::doCreateLightingPass( crg::FramePassGroup & graph
		, Scene const & scene
		, crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating indirect light pass" );
		auto & engine = *m_device.renderSystem.getEngine();
		auto & modelBuffer = scene.getModelBuffer().getBuffer();
		auto & pass = graph.createPass( "IndirectLighting"
			, [this, progress, &engine]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising indirect light pass" );
				auto result = crg::RenderQuadBuilder{}
					.renderSize( makeExtent2D( ( *m_lpResult.begin() )->getExtent() ) )
					.programCreator( { uint32_t( IndirectLightingPass::ProgramType::eCount )
						, [this]( uint32_t programIndex )
						{
							auto & program = doCreateProgram( programIndex );
							return crg::makeVkArray< VkPipelineShaderStageCreateInfo >( program.stages );
						} } )
					.passIndex( &m_programIndex )
					.enabled( &m_enabled )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{ uint32_t( m_programs.size() ) }
							.implicitAction( m_lpResult[LpTexture::eIndirectDiffuse].targetViewId
								, crg::RecordContext::clearAttachment( m_lpResult[LpTexture::eIndirectDiffuse].targetViewId, getClearValue( LpTexture::eIndirectDiffuse ) ) )
							.implicitAction( m_lpResult[LpTexture::eIndirectSpecular].targetViewId
								, crg::RecordContext::clearAttachment( m_lpResult[LpTexture::eIndirectSpecular].targetViewId, getClearValue( LpTexture::eIndirectSpecular ) ) ) );
				engine.registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );
		pass.addInputStorageBuffer( { modelBuffer, "Models" }
			, uint32_t( IndirectLightingPass::eModels )
			, 0u
			, uint32_t( modelBuffer.getSize() ) );
		m_gpInfoUbo.createPassBinding( pass
			, uint32_t( IndirectLightingPass::eGpInfo ) );
		m_sceneUbo.createPassBinding( pass
			, uint32_t( IndirectLightingPass::eScene ) );
		engine.getMaterialCache().getPassBuffer().createPassBinding( pass
			, uint32_t( IndirectLightingPass::eMaterials ) );
		m_lpvConfigUbo.createPassBinding( pass
			, uint32_t( IndirectLightingPass::eLpvGridConfig ) );

		if ( m_device.renderSystem.hasLLPV() )
		{
			m_llpvConfigUbo.createPassBinding( pass
				, uint32_t( IndirectLightingPass::eLayeredLpvGridConfig ) );
		}

		m_vctConfigUbo.createPassBinding( pass
			, uint32_t( IndirectLightingPass::eVoxelData ) );
		pass.addSampledView( m_depthObj.sampledViewId
			, uint32_t( IndirectLightingPass::eDepthObj ) );
		pass.addSampledView( m_gpResult[DsTexture::eNmlOcc].sampledViewId
			, uint32_t( IndirectLightingPass::eNmlOcc ) );
		pass.addSampledView( m_gpResult[DsTexture::eColMtl].sampledViewId
			, uint32_t( IndirectLightingPass::eColMtl ) );
		pass.addSampledView( m_gpResult[DsTexture::eSpcRgh].sampledViewId
			, uint32_t( IndirectLightingPass::eSpcRgh ) );
		pass.addSampledView( m_brdf.sampledViewId
			, uint32_t( IndirectLightingPass::eBrdf ) );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR };
		pass.addSampledView( m_vctFirstBounce.sampledViewId
			, uint32_t( IndirectLightingPass::eVctFirstBounce )
			, linearSampler );
		pass.addSampledView( m_vctSecondaryBounce.sampledViewId
			, uint32_t( IndirectLightingPass::eVctSecondBounce )
			, linearSampler );
		pass.addSampledView( m_lpvResult[LpvTexture::eR].sampledViewId
			, uint32_t( IndirectLightingPass::eLpvR )
			, linearSampler );
		pass.addSampledView( m_lpvResult[LpvTexture::eG].sampledViewId
			, uint32_t( IndirectLightingPass::eLpvG )
			, linearSampler );
		pass.addSampledView( m_lpvResult[LpvTexture::eB].sampledViewId
			, uint32_t( IndirectLightingPass::eLpvB )
			, linearSampler );

		if ( m_device.renderSystem.hasLLPV() )
		{
			auto & lpvResult0 = *m_llpvResult[0];
			pass.addSampledView( lpvResult0[LpvTexture::eR].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv1R )
				, linearSampler );
			pass.addSampledView( lpvResult0[LpvTexture::eG].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv1G )
				, linearSampler );
			pass.addSampledView( lpvResult0[LpvTexture::eB].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv1B )
				, linearSampler );
			auto & lpvResult1 = *m_llpvResult[1];
			pass.addSampledView( lpvResult1[LpvTexture::eR].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv2R )
				, linearSampler );
			pass.addSampledView( lpvResult1[LpvTexture::eG].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv2G )
				, linearSampler );
			pass.addSampledView( lpvResult1[LpvTexture::eB].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv2B )
				, linearSampler );
			auto & lpvResult2 = *m_llpvResult[2];
			pass.addSampledView( lpvResult2[LpvTexture::eR].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv3R )
				, linearSampler );
			pass.addSampledView( lpvResult2[LpvTexture::eG].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv3G )
				, linearSampler );
			pass.addSampledView( lpvResult2[LpvTexture::eB].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv3B )
				, linearSampler );
		}

		auto index = uint32_t( IndirectLightingPass::eCount );
		engine.createSpecificsBuffersPassBindings( pass, index );
		pass.addOutputColourView( m_lpResult[LpTexture::eIndirectDiffuse].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eIndirectSpecular].targetViewId );

		return pass;
	}

	IndirectLightingPass::Program & IndirectLightingPass::doCreateProgram( uint32_t programIndex )
	{
		CU_Require( programIndex < uint32_t( IndirectLightingPass::ProgramType::eCount ) );

		if ( !m_programs[programIndex] )
		{
			IndirectLightingPass::Config config{ programIndex };

			if ( !config.llpv
				|| m_device.renderSystem.hasLLPV() )
			{
				m_programs[programIndex] = std::make_unique< Program >( m_device, m_scene, config );
				m_programs[programIndex]->stages = { makeShaderState( m_device, m_programs[programIndex]->vertexShader )
					, makeShaderState( m_device, m_programs[programIndex]->pixelShader ) };
			}
		}

		return *m_programs[programIndex];
	}

	//*********************************************************************************************
}

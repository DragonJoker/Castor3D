#include "Castor3D/Render/Technique/Opaque/IndirectLightingPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, IndirectLightingPass )

namespace castor3d
{
	namespace
	{
		ShaderPtr getVertexShaderSource()
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

		ShaderPtr getPixelShaderSource( PassTypeID passType
			, RenderSystem const & renderSystem
			, IndirectLightingPass::Config const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			shader::Utils utils{ writer, *renderSystem.getEngine() };
			utils.declareCalcTexCoord();
			utils.declareCalcVSPosition();
			utils.declareCalcWSPosition();
			utils.declareDecodeReceiver();
			utils.declareInvertVec2Y();

			// Shader outputs
			auto pxl_indirectDiffuse = writer.declOutput< Vec3 >( "pxl_indirectDiffuse", 0 );
			auto pxl_indirectSpecular = writer.declOutput< Vec3 >( "pxl_indirectSpecular", 1 );

			// Shader inputs
			UBO_GPINFO( writer, uint32_t( IndirectLightingPass::eGpInfo ), 0u );
			UBO_SCENE( writer, uint32_t( IndirectLightingPass::eScene ), 0u );
			shader::GlobalIllumination indirect{ writer, utils, true };
			uint32_t vctIndex = uint32_t( IndirectLightingPass::eVctStart );
			uint32_t lpvIndex = uint32_t( IndirectLightingPass::eLpvStart );
			uint32_t llpvIndex = uint32_t( IndirectLightingPass::eLayeredLpvStart );
			indirect.declare( uint32_t( IndirectLightingPass::eVoxelData )
				, uint32_t( IndirectLightingPass::eLpvGridConfig )
				, uint32_t( IndirectLightingPass::eLayeredLpvGridConfig )
				, vctIndex
				, lpvIndex
				, llpvIndex
				, 0u
				, config.sceneFlags );
			auto lightingModel = utils.createLightingModel( shader::getLightingModelName( *renderSystem.getEngine(), passType )
				, {}
				, true );
			auto c3d_mapData0 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData0 ), uint32_t( IndirectLightingPass::eDepth ), 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), uint32_t( IndirectLightingPass::eData1 ), 0u );
			auto c3d_mapData2 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData2 ), uint32_t( IndirectLightingPass::eData2 ), 0u );
			auto c3d_mapData3 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData3 ), uint32_t( IndirectLightingPass::eData3 ), 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto texCoord = writer.declLocale( "texCoord"
						, c3d_gpInfoData.calcTexCoord( utils
							, in.fragCoord.xy() ) );
					auto data1 = writer.declLocale( "data1"
						, c3d_mapData1.lod( texCoord, 0.0_f ) );
					auto flags = writer.declLocale( "flags"
						, writer.cast< Int >( data1.w() ) );
					auto shadowReceiver = writer.declLocale( "shadowReceiver"
						, 0_i );
					auto lightingReceiver = writer.declLocale( "lightingReceiver"
						, 0_i );
					utils.decodeReceiver( flags, shadowReceiver, lightingReceiver );

					IF( writer, lightingReceiver )
					{
						auto data2 = writer.declLocale( "data2"
							, c3d_mapData2.lod( texCoord, 0.0_f ) );
						auto data3 = writer.declLocale( "data3"
							, c3d_mapData3.lod( texCoord, 0.0_f ) );
						auto lightMat = lightingModel->declMaterial( "lightMat" );
						lightMat->create( data2.rgb(), data3, data2 );
						auto eye = writer.declLocale( "eye"
							, c3d_sceneData.cameraPosition );
						auto depth = writer.declLocale( "depth"
							, c3d_mapData0.lod( texCoord, 0.0_f ).x() );
						auto vsPosition = writer.declLocale( "vsPosition"
							, c3d_gpInfoData.projToView( utils, texCoord, depth ) );
						auto wsPosition = writer.declLocale( "wsPosition"
							, c3d_gpInfoData.projToWorld( utils, texCoord, depth ) );
						auto wsNormal = writer.declLocale( "wsNormal"
							, data1.xyz() );
						auto surface = writer.declLocale< shader::Surface >( "surface" );
						surface.create( in.fragCoord.xy(), vsPosition, wsPosition, wsNormal );

						//auto occlusion = indirect.computeOcclusion( sceneFlags
						//	, lightType
						//	, surface );
						auto occlusion = writer.declLocale( "occlusion"
							, 1.0_f );
						auto indirectDiffuse = indirect.computeDiffuse( config.sceneFlags
							, surface
							, occlusion );
						auto indirectSpecular = indirect.computeSpecular( config.sceneFlags
							, eye
							, c3d_sceneData.getPosToCamera( surface.worldPosition )
							, surface
							, lightMat->specular
							, lightMat->getRoughness()
							, occlusion
							, indirectDiffuse.w() );
						pxl_indirectDiffuse = indirectDiffuse.xyz();
						pxl_indirectSpecular = indirectSpecular;
					}
					ELSE
					{
						pxl_indirectDiffuse = vec3( 0.0_f, 0.0_f, 0.0_f );
						pxl_indirectSpecular = vec3( 0.0_f, 0.0_f, 0.0_f );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		uint32_t getIndex( SceneFlags sceneFlags )
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

		SceneFlags adjustSceneFlags( SceneFlags sceneFlags )
		{
			remFlag( sceneFlags, SceneFlag::eFogLinear );
			remFlag( sceneFlags, SceneFlag::eFogExponential );
			remFlag( sceneFlags, SceneFlag::eFogSquaredExponential );
			remFlag( sceneFlags, SceneFlag::eShadowAny );
			return sceneFlags;
		}

		std::vector< IndirectLightingPass::Program > createPrograms( RenderDevice const & device
			, Scene const & scene )
		{
			std::vector< IndirectLightingPass::Program > result;

			for ( uint32_t i = 0u; i < uint32_t( IndirectLightingPass::ProgramType::eCount ); ++i )
			{
				IndirectLightingPass::Config config{ i };

				if ( !config.llpv
					|| device.renderSystem.hasLLPV() )
				{
					result.emplace_back( device, scene, config );
				}
			}

			return result;
		}

		std::vector< crg::VkPipelineShaderStageCreateInfoArray > convertPrograms( std::vector< IndirectLightingPass::Program > const & programs )
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

	IndirectLightingPass::Config::Config( SceneFlags const & sceneFlags )
		: sceneFlags{ adjustSceneFlags( sceneFlags ) }
		, voxels{ checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) }
		, lpv{ checkFlag( sceneFlags, SceneFlag::eLpvGI ) }
		, llpv{ checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) }
		, index{ getIndex( sceneFlags ) }
	{
	}

	IndirectLightingPass::Config::Config( uint32_t index )
		: sceneFlags{ SceneFlag::eNone }
		, voxels{ index == uint32_t( ProgramType::eVCT ) }
		, lpv{ ( index == uint32_t( ProgramType::eLPV ) ) || ( index == uint32_t( ProgramType::eLPVLLPV ) ) }
		, llpv{ ( index == uint32_t( ProgramType::eLLPV ) ) || ( index == uint32_t( ProgramType::eLPVLLPV ) ) }
		, index{ index }
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
			, getVertexShaderSource() }
		, pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, "IndirectLightingPass"
			, getPixelShaderSource( scene.getPassesType()
				, device.renderSystem
				, config ) }
		, stages{ makeShaderState( device, vertexShader )
			, makeShaderState( device, pixelShader ) }
	{
	}

	//*********************************************************************************************

	IndirectLightingPass::IndirectLightingPass( RenderDevice const & device
		, ProgressBar * progress
		, Scene const & scene
		, crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
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
		, m_programs{ createPrograms( device, scene ) }
	{
		previousPass = &doCreateLightingPass( graph, *previousPass, progress );
	}

	void IndirectLightingPass::update( CpuUpdater & updater )
	{
		m_programIndex = Config{ updater.camera->getScene()->getFlags() }.index;
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

	crg::FramePass const & IndirectLightingPass::doCreateLightingPass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating indirect light pass" );
		auto & engine = *m_device.renderSystem.getEngine();
		auto & pass = graph.createPass( "IndirectLighting"
			, [this, progress, &engine]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising indirect light pass" );
				auto result = crg::RenderQuadBuilder{}
					.renderSize( makeExtent2D( m_lpResult.begin()->getExtent() ) )
					.programs( convertPrograms( m_programs ) )
					.passIndex( &m_programIndex )
					.build( pass, context, graph, uint32_t( m_programs.size() ) );
				engine.registerTimer( graph.getName() + "/IndirectLighting"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );
		m_gpInfoUbo.createPassBinding( pass
			, uint32_t( IndirectLightingPass::eGpInfo ) );
		m_sceneUbo.createPassBinding( pass
			, uint32_t( IndirectLightingPass::eScene ) );
		m_lpvConfigUbo.createPassBinding( pass
			, uint32_t( IndirectLightingPass::eLpvGridConfig ) );

		if ( m_device.renderSystem.hasLLPV() )
		{
			m_llpvConfigUbo.createPassBinding( pass
				, uint32_t( IndirectLightingPass::eLayeredLpvGridConfig ) );
		}

		m_vctConfigUbo.createPassBinding( pass
			, uint32_t( IndirectLightingPass::eVoxelData ) );
		pass.addSampledView( m_gpResult[DsTexture::eData0].sampledViewId
			, uint32_t( IndirectLightingPass::eDepth )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData1].sampledViewId
			, uint32_t( IndirectLightingPass::eData1 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData2].sampledViewId
			, uint32_t( IndirectLightingPass::eData2 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData3].sampledViewId
			, uint32_t( IndirectLightingPass::eData3 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR };
		pass.addSampledView( m_vctFirstBounce.sampledViewId
			, uint32_t( IndirectLightingPass::eVctFirstBounce )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( m_vctSecondaryBounce.sampledViewId
			, uint32_t( IndirectLightingPass::eVctSecondBounce )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( m_lpvResult[LpvTexture::eR].sampledViewId
			, uint32_t( IndirectLightingPass::eLpvR )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( m_lpvResult[LpvTexture::eG].sampledViewId
			, uint32_t( IndirectLightingPass::eLpvG )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( m_lpvResult[LpvTexture::eB].sampledViewId
			, uint32_t( IndirectLightingPass::eLpvB )
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );

		if ( m_device.renderSystem.hasLLPV() )
		{
			auto & lpvResult0 = *m_llpvResult[0];
			pass.addSampledView( lpvResult0[LpvTexture::eR].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv1R )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
			pass.addSampledView( lpvResult0[LpvTexture::eG].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv1G )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
			pass.addSampledView( lpvResult0[LpvTexture::eB].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv1B )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
			auto & lpvResult1 = *m_llpvResult[1];
			pass.addSampledView( lpvResult1[LpvTexture::eR].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv2R )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
			pass.addSampledView( lpvResult1[LpvTexture::eG].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv2G )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
			pass.addSampledView( lpvResult1[LpvTexture::eB].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv2B )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
			auto & lpvResult2 = *m_llpvResult[2];
			pass.addSampledView( lpvResult2[LpvTexture::eR].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv3R )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
			pass.addSampledView( lpvResult2[LpvTexture::eG].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv3G )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
			pass.addSampledView( lpvResult2[LpvTexture::eB].sampledViewId
				, uint32_t( IndirectLightingPass::eLayeredLpv3B )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearSampler );
		}

		pass.addOutputColourView( m_lpResult[LpTexture::eIndirectDiffuse].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eIndirectSpecular].targetViewId );

		return pass;
	}

	//*********************************************************************************************
}

#include "WaterRenderPass.hpp"

#include "WaterUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderQueue.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp>
#include <Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp>
#include <Castor3D/Render/Node/BillboardRenderNode.hpp>
#include <Castor3D/Render/Node/SubmeshRenderNode.hpp>
#include <Castor3D/Render/Node/QueueCulledRenderNodes.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslFog.hpp>
#include <Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureAnimation.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/BillboardUbo.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>
#include <Castor3D/Shader/Ubos/ModelUbo.hpp>
#include <Castor3D/Shader/Ubos/MorphingUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

#include <ashespp/Image/StagingTexture.hpp>

namespace water
{
	namespace
	{
		enum WaterIdx : uint32_t
		{
			eLightBuffer = uint32_t( castor3d::PassUboIdx::eCount ),
			eWaterUbo,
			eWaterNormals1,
			eWaterNormals2,
			eWaterNoise,
			eSceneNormals,
			eSceneDepth,
			eSceneResult,
		};

		void bindTexture( VkImageView view
			, VkSampler sampler
			, ashes::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			writes.push_back( { index++
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, { { sampler
				, view
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } } );
		}

		void loadImage( castor3d::Parameters const & params
			, castor::String const & prefix
			, castor::String const & name
			, castor3d::Engine & engine
			, crg::FrameGraph & graph
			, castor::ImageLoader & loader
			, crg::ImageId & img
			, crg::ImageViewId & view
			, ashes::ImageView & result )
		{
			castor::Path folder;
			castor::Path relative;

			if ( params.get( prefix + "Folder", folder )
				&& params.get( prefix + "Relative", relative ) )
			{
				auto image = loader.load( relative, folder / relative, {} );
				auto format = image.getPixelFormat();

				if ( format == castor::PixelFormat::eR8G8B8_UNORM )
				{
					auto buffer = castor::PxBufferBase::create( image.getDimensions()
						, castor::PixelFormat::eR8G8B8A8_UNORM
						, image.getPxBuffer().getConstPtr()
						, image.getPxBuffer().getFormat()
						, image.getPxBuffer().getAlign() );
					image = castor::Image{ image.getName()
						, image.getPath()
						, *buffer };
				}

				auto dim = image.getDimensions();
				img = graph.createImage( crg::ImageData{ name + prefix
					, 0u
					, VK_IMAGE_TYPE_2D
					, VkFormat( format )
					, { dim.getWidth(), dim.getHeight(), 1u }
					, ( VK_IMAGE_USAGE_SAMPLED_BIT
						| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
					, image.getLevels() } );

				view = graph.createView( crg::ImageViewData{ name + prefix + "Target"
					, img
					, 0u
					, VK_IMAGE_VIEW_TYPE_2D
					, img.data->info.format
					, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, image.getLevels(), 0u, 1u } } );
				auto buffer = image.getPixels();

				engine.pushGpuJob( [format, dim, buffer, &graph, &img, &view, &result]( castor3d::RenderDevice const & device
					, castor3d::QueueData const & queue )
					{
						auto & context = device.makeContext();
						auto staging = device->createStagingTexture( VkFormat( format )
							, VkExtent2D{ dim.getWidth(), dim.getHeight() }
							, buffer->getLevels() );
						ashes::ImagePtr noiseImg = std::make_unique< ashes::Image >( *device
							, graph.getHandler().createImage( context, img )
							, img.data->info );
						result = ashes::ImageView{ ashes::ImageViewCreateInfo{ *noiseImg, view.data->info }
							, graph.getHandler().createImageView( context, view )
							, noiseImg.get() };
						auto data = device.graphicsData();
						staging->uploadTextureData( *queue.queue
							, *queue.commandPool
							, VkFormat( format )
							, buffer->getConstPtr()
							, result );
					} );
			}
		}
	}

	castor::String const WaterRenderPass::Type = cuT( "c3d.water" );
	castor::String const WaterRenderPass::Name = cuT( "Water" );
	castor::String const WaterRenderPass::FullName = cuT( "Water rendering" );
	castor::String const WaterRenderPass::Param = cuT( "WaterConfig" );
	castor::String const WaterRenderPass::Normals1 = cuT( "WaterNormals1" );
	castor::String const WaterRenderPass::Normals2 = cuT( "WaterNormals2" );
	castor::String const WaterRenderPass::Noise = cuT( "WaterNoise" );

	WaterRenderPass::WaterRenderPass( castor3d::RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, castor::String const & category
		, std::shared_ptr< castor3d::Texture > colourInput
		, std::shared_ptr< castor3d::Texture > depthInput
		, castor3d::SceneRenderPassDesc const & renderPassDesc
		, castor3d::RenderTechniquePassDesc const & techniquePassDesc
		, std::shared_ptr< IsRenderPassEnabled > isEnabled )
		: castor3d::RenderTechniquePass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, category
			, "Water"
			, renderPassDesc
			, techniquePassDesc }
		, m_isEnabled{ std::move( isEnabled ) }
		, m_colourInput{ std::move( colourInput ) }
		, m_depthInput{ std::move( depthInput ) }
		, m_linearWrapSampler{ device->createSampler( getName()
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR ) }
		, m_pointClampSampler{ device->createSampler( getName()
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST
			, VK_SAMPLER_MIPMAP_MODE_NEAREST ) }
		, m_ubo{ device }
	{
		m_isEnabled->setPass( *this );
		auto params = getEngine()->getRenderPassTypeConfiguration( Type );

		if ( params.get( Param, m_configuration ) )
		{
			auto & data = m_ubo.getUbo().getData();
			data = m_configuration;
		}

		auto & loader = getEngine()->getImageLoader();
		loadImage( params, Normals1, getName(), *getEngine(), pass.graph, loader, m_normals1Img, m_normals1View, m_normals1 );
		loadImage( params, Normals2, getName(), *getEngine(), pass.graph, loader, m_normals2Img, m_normals2View, m_normals2 );
		loadImage( params, Noise, getName(), *getEngine(), pass.graph, loader, m_noiseImg, m_noiseView, m_noise );
	}

	WaterRenderPass::~WaterRenderPass()
	{
		m_colourInput->destroy();
		m_colourInput.reset();
		m_depthInput->destroy();
		m_depthInput.reset();
	}

	crg::FramePassArray WaterRenderPass::create( castor3d::RenderDevice const & device
		, castor3d::RenderTechnique & technique
		, castor3d::TechniquePasses & renderPasses
		, crg::FramePassArray previousPasses )
	{
		std::string name{ Name };
		auto isEnabled = std::make_shared< IsRenderPassEnabled >();
		auto extent = getExtent( technique.getResultImg() );
		auto & graph = technique.getRenderTarget().getGraph().createPassGroup( name );
		auto colourInput = std::make_shared< castor3d::Texture >( device
			, graph.getHandler()
			, name +"Colour"
			, 0u
			, extent
			, 1u
			, 1u
			, technique.getResultImg().data->info.format
			, ( VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT ) );
		colourInput->create();
		auto & blitColourPass = graph.createPass( name + "CopyColour"
			, [name, extent, &device, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = std::make_unique< crg::ImageCopy >( framePass
					, context
					, runnableGraph
					, extent
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
				device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/" + framePass.name
					, result->getTimer() );
				return result;
			} );
		blitColourPass.addDependencies( previousPasses );
		blitColourPass.addTransferInputView( technique.getResultImgView() );
		blitColourPass.addTransferOutputView( colourInput->sampledViewId );

		auto depthInput = std::make_shared< castor3d::Texture >( device
			, graph.getHandler()
			, name + "Depth"
			, 0u
			, extent
			, 1u
			, 1u
			, technique.getDepthImg().data->info.format
			, ( VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT ) );
		depthInput->create();
		auto & blitDepthPass = graph.createPass( name + "CopyDepth"
			, [name, extent, &device, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = std::make_unique< crg::ImageCopy >( framePass
					, context
					, runnableGraph
					, extent
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
				device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/" + framePass.name
					, result->getTimer() );
				return result;
			} );
		blitDepthPass.addDependencies( previousPasses );
		blitDepthPass.addTransferInputView( technique.getDepthSampledView() );
		blitDepthPass.addTransferOutputView( depthInput->sampledViewId );

		auto & result = graph.createPass( name
			, [name, extent, colourInput, depthInput, &device, &technique, &renderPasses, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
		{
			auto res = std::make_unique< WaterRenderPass >( &technique
				, framePass
				, context
				, runnableGraph
				, device
				, name
				, std::move( colourInput )
				, std::move( depthInput )
				, castor3d::SceneRenderPassDesc{ extent
					, technique.getMatrixUbo()
					, technique.getRenderTarget().getCuller() }.safeBand( true )
				, castor3d::RenderTechniquePassDesc{ false, technique.getSsaoConfig() }
					.lpvConfigUbo( technique.getLpvConfigUbo() )
					.llpvConfigUbo( technique.getLlpvConfigUbo() )
					.vctConfigUbo( technique.getVctConfigUbo() )
					.lpvResult( technique.getLpvResult() )
					.llpvResult( technique.getLlpvResult() )
					.vctFirstBounce( technique.getFirstVctBounce() )
					.vctSecondaryBounce( technique.getSecondaryVctBounce() )
				, isEnabled );
			renderPasses[size_t( Event )].push_back( res.get() );
			device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/" + name
				, res->getTimer() );
			return res;
		} );

		result.addDependency( blitColourPass );
		result.addDependency( blitDepthPass );
		result.addImplicitColourView( colourInput->sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addImplicitDepthView( depthInput->sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addInOutDepthStencilView( technique.getDepthTargetView() );
		result.addInOutColourView( technique.getResultTargetView() );
		return { &result, &blitDepthPass };
	}

	castor3d::TextureFlags WaterRenderPass::getTexturesMask()const
	{
		return castor3d::TextureFlags{ castor3d::TextureFlag::eAll };
	}

	void WaterRenderPass::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID )
		{
#if Water_Debug
			visitor.visit( cuT( "Debug" )
				, m_configuration.debug
				, getWaterDisplayDataNames()
				, nullptr );
#endif
			visitor.visit( cuT( "Dampening factor" )
				, m_configuration.dampeningFactor
				, nullptr );
			visitor.visit( cuT( "Depth softening distance" )
				, m_configuration.depthSofteningDistance
				, nullptr );
			visitor.visit( cuT( "Refraction ratio" )
				, m_configuration.refractionRatio
				, nullptr );
			visitor.visit( cuT( "Refraction distortion factor" )
				, m_configuration.refractionDistortionFactor
				, nullptr );
			visitor.visit( cuT( "Refraction height factor" )
				, m_configuration.refractionHeightFactor
				, nullptr );
			visitor.visit( cuT( "Refraction distance factor" )
				, m_configuration.refractionDistanceFactor
				, nullptr );
			visitor.visit( cuT( "Normal map scroll speed" )
				, m_configuration.normalMapScrollSpeed
				, nullptr );
			visitor.visit( cuT( "Normal map scroll" )
				, m_configuration.normalMapScroll
				, nullptr );
			visitor.visit( cuT( "SSR step size" )
				, m_configuration.ssrStepSize
				, nullptr );
			visitor.visit( cuT( "SSR forward steps count" )
				, m_configuration.ssrForwardStepsCount
				, nullptr );
			visitor.visit( cuT( "SSR backward steps count" )
				, m_configuration.ssrBackwardStepsCount
				, nullptr );
			visitor.visit( cuT( "SSR depth mult." )
				, m_configuration.ssrDepthMult
				, nullptr );
		}
	}

	void WaterRenderPass::doUpdateUbos( castor3d::CpuUpdater & updater )
	{
		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );
		m_configuration.time += float( tslf.count() ) / 100.0f;
		m_ubo.cpuUpdate( m_configuration );
		RenderTechniquePass::doUpdateUbos( updater );
	}

	bool WaterRenderPass::doIsValidPass( castor3d::Pass const & pass )const
	{
		return pass.getRenderPassInfo()
			&& pass.getRenderPassInfo()->name == Type
			&& doAreValidPassFlags( pass.getPassFlags() );
	}

	void WaterRenderPass::doFillAdditionalBindings( castor3d::PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		bindings.emplace_back( getCuller().getScene().getLightCache().createLayoutBinding( WaterIdx::eLightBuffer ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eWaterUbo
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_VERTEX_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_FRAGMENT_BIT ) ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eWaterNormals1
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eWaterNormals2
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eWaterNoise
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eSceneNormals
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eSceneDepth
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eSceneResult
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto index = uint32_t( WaterIdx::eSceneResult ) + 1u;

		for ( uint32_t j = 0u; j < uint32_t( castor3d::LightType::eCount ); ++j )
		{
			if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag( uint8_t( castor3d::SceneFlag::eShadowBegin ) << j ) ) )
			{
				bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// light type shadow depth
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// light type shadow variance
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}
		}

		if ( checkFlag( flags.passFlags, castor3d::PassFlag::eImageBasedLighting ) )
		{
			bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_mapBrdf
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_mapIrradiance
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_mapPrefiltered
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eVoxelConeTracing ) )
		{
			CU_Require( m_vctConfigUbo );
			CU_Require( m_vctFirstBounce );
			CU_Require( m_vctSecondaryBounce );
			bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// Voxel UBO
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_mapVoxelsFirstBounce
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_mapVoxelsSecondaryBounce
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}
		else
		{
			if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvConfigUbo );
				bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// LPV Grid UBO
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvConfigUbo );
				bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// LLPV Grid UBO
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eLpvGI ) )
			{
				CU_Require( m_lpvResult );
				bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_lpvAccumulationR
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_lpvAccumulationG
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_lpvAccumulationB
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( m_llpvResult );

				for ( size_t i = 0u; i < m_llpvResult->size(); ++i )
				{
					bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_lpvAccumulationRn
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, VK_SHADER_STAGE_FRAGMENT_BIT ) );
					bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_lpvAccumulationGn
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, VK_SHADER_STAGE_FRAGMENT_BIT ) );
					bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++	// c3d_lpvAccumulationBn
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				}
			}
		}
	}

	ashes::PipelineDepthStencilStateCreateInfo WaterRenderPass::doCreateDepthStencilState( castor3d::PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo
		{
			0u,
			VK_TRUE,
			VK_TRUE,
		};
	}

	ashes::PipelineColorBlendStateCreateInfo WaterRenderPass::doCreateBlendState( castor3d::PipelineFlags const & flags )const
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments
		{
			VkPipelineColorBlendAttachmentState
			{
				VK_TRUE,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				castor3d::defaultColorWriteMask,
			},
		};
		return ashes::PipelineColorBlendStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			std::move( attachments ),
		};
	}

	namespace
	{
		void fillAdditionalDescriptor( crg::RunnableGraph & graph
			, castor3d::RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, castor3d::Scene const & scene
			, castor3d::SceneNode const & sceneNode
			, castor3d::ShadowMapLightTypeArray const & shadowMaps
			, castor3d::LpvGridConfigUbo const * lpvConfigUbo
			, castor3d::LayeredLpvGridConfigUbo const * llpvConfigUbo
			, castor3d::VoxelizerUbo const * vctConfigUbo
			, castor3d::LightVolumePassResult const * lpvResult
			, castor3d::LightVolumePassResultArray const * llpvResult
			, castor3d::Texture const * vctFirstBounce
			, castor3d::Texture const * vctSecondaryBounce
			, WaterUbo const & waveUbo
			, VkSampler linearWrapSampler
			, VkSampler pointClampSampler
			, VkImageView const & normals1View
			, VkImageView const & normals2View
			, VkImageView const & noiseView
			, VkImageView const & normals
			, VkImageView const & depth
			, VkImageView const & colour )
		{
			auto & flags = pipeline.getFlags();
			descriptorWrites.push_back( scene.getLightCache().getBinding( WaterIdx::eLightBuffer ) );
			descriptorWrites.push_back( waveUbo.getDescriptorWrite( WaterIdx::eWaterUbo ) );
			auto index = uint32_t( WaterIdx::eWaterNormals1 );
			bindTexture( normals1View, linearWrapSampler, descriptorWrites, index );
			bindTexture( normals2View, linearWrapSampler, descriptorWrites, index );
			bindTexture( noiseView, linearWrapSampler, descriptorWrites, index );
			bindTexture( normals, pointClampSampler, descriptorWrites, index );
			bindTexture( depth, pointClampSampler, descriptorWrites, index );
			bindTexture( colour, pointClampSampler, descriptorWrites, index );

			castor3d::bindShadowMaps( graph
				, pipeline.getFlags()
				, shadowMaps
				, descriptorWrites
				, index );

			if ( checkFlag( flags.passFlags, castor3d::PassFlag::eImageBasedLighting ) )
			{
				auto & background = *scene.getBackground();

				if ( background.hasIbl() )
				{
					auto & ibl = background.getIbl();
					bindTexture( ibl.getPrefilteredBrdfTexture().wholeView
						, ibl.getPrefilteredBrdfSampler()
						, descriptorWrites
						, index );
					bindTexture( ibl.getIrradianceTexture().wholeView
						, ibl.getIrradianceSampler()
						, descriptorWrites
						, index );
					bindTexture( ibl.getPrefilteredEnvironmentTexture().wholeView
						, ibl.getPrefilteredEnvironmentSampler()
						, descriptorWrites
						, index );
				}
			}

			if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eVoxelConeTracing ) )
			{
				CU_Require( vctConfigUbo );
				CU_Require( vctFirstBounce );
				CU_Require( vctSecondaryBounce );
				descriptorWrites.push_back( vctConfigUbo->getDescriptorWrite( index++ ) );
				bindTexture( vctFirstBounce->wholeView
					, *vctFirstBounce->sampler
					, descriptorWrites
					, index );
				bindTexture( vctSecondaryBounce->wholeView
					, *vctSecondaryBounce->sampler
					, descriptorWrites
					, index );
			}
			else
			{
				if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eLpvGI ) )
				{
					CU_Require( lpvConfigUbo );
					descriptorWrites.push_back( lpvConfigUbo->getDescriptorWrite( index++ ) );
				}

				if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eLayeredLpvGI ) )
				{
					CU_Require( llpvConfigUbo );
					descriptorWrites.push_back( llpvConfigUbo->getDescriptorWrite( index++ ) );
				}

				if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eLpvGI ) )
				{
					CU_Require( lpvResult );
					auto & lpv = *lpvResult;
					bindTexture( lpv[castor3d::LpvTexture::eR].wholeView
						, *lpv[castor3d::LpvTexture::eR].sampler
						, descriptorWrites
						, index );
					bindTexture( lpv[castor3d::LpvTexture::eG].wholeView
						, *lpv[castor3d::LpvTexture::eG].sampler
						, descriptorWrites
						, index );
					bindTexture( lpv[castor3d::LpvTexture::eB].wholeView
						, *lpv[castor3d::LpvTexture::eB].sampler
						, descriptorWrites
						, index );
				}

				if ( checkFlag( flags.sceneFlags, castor3d::SceneFlag::eLayeredLpvGI ) )
				{
					CU_Require( llpvResult );

					for ( auto & plpv : *llpvResult )
					{
						auto & lpv = *plpv;
						bindTexture( lpv[castor3d::LpvTexture::eR].wholeView
							, *lpv[castor3d::LpvTexture::eR].sampler
							, descriptorWrites
							, index );
						bindTexture( lpv[castor3d::LpvTexture::eG].wholeView
							, *lpv[castor3d::LpvTexture::eG].sampler
							, descriptorWrites
							, index );
						bindTexture( lpv[castor3d::LpvTexture::eB].wholeView
							, *lpv[castor3d::LpvTexture::eB].sampler
							, descriptorWrites
							, index );
					}
				}
			}
		}
	}

	void WaterRenderPass::doFillAdditionalDescriptor( castor3d::RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, castor3d::BillboardRenderNode & node
		, castor3d::ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( m_graph
			, pipeline
			, descriptorWrites
			, m_scene
			, node.sceneNode
			, shadowMaps
			, m_lpvConfigUbo
			, m_llpvConfigUbo
			, m_vctConfigUbo
			, m_lpvResult
			, m_llpvResult
			, m_vctFirstBounce
			, m_vctSecondaryBounce
			, m_ubo
			, *m_linearWrapSampler
			, *m_pointClampSampler
			, m_normals1
			, m_normals2
			, m_noise
			, m_parent->getNormalTexture().sampledView
			, m_parent->getDepthTexture().sampledView
			, m_parent->getResultTexture().sampledView );
	}

	void WaterRenderPass::doFillAdditionalDescriptor( castor3d::RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, castor3d::SubmeshRenderNode & node
		, castor3d::ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( m_graph
			, pipeline
			, descriptorWrites
			, m_scene
			, node.sceneNode
			, shadowMaps
			, m_lpvConfigUbo
			, m_llpvConfigUbo
			, m_vctConfigUbo
			, m_lpvResult
			, m_llpvResult
			, m_vctFirstBounce
			, m_vctSecondaryBounce
			, m_ubo
			, *m_linearWrapSampler
			, *m_pointClampSampler
			, m_normals1
			, m_normals2
			, m_noise
			, m_parent->getNormalTexture().sampledView
			, m_depthInput->sampledView
			, m_colourInput->sampledView );
	}

	void WaterRenderPass::doUpdateFlags( castor3d::PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, castor3d::ProgramFlag::eLighting );
		addFlag( flags.programFlags, castor3d::ProgramFlag::eForceTexCoords );

		remFlag( flags.programFlags, castor3d::ProgramFlag::eInstantiation );
		remFlag( flags.programFlags, castor3d::ProgramFlag::eMorphing );
		remFlag( flags.programFlags, castor3d::ProgramFlag::eSkinning );
		remFlag( flags.programFlags, castor3d::ProgramFlag::eSecondaryUV );

		remFlag( flags.passFlags, castor3d::PassFlag::eReflection );
		remFlag( flags.passFlags, castor3d::PassFlag::eRefraction );
		remFlag( flags.passFlags, castor3d::PassFlag::eParallaxOcclusionMappingOne );
		remFlag( flags.passFlags, castor3d::PassFlag::eParallaxOcclusionMappingRepeat );
		remFlag( flags.passFlags, castor3d::PassFlag::eDistanceBasedTransmittance );
		remFlag( flags.passFlags, castor3d::PassFlag::eSubsurfaceScattering );
		remFlag( flags.passFlags, castor3d::PassFlag::eAlphaBlending );
		remFlag( flags.passFlags, castor3d::PassFlag::eAlphaTest );
		remFlag( flags.passFlags, castor3d::PassFlag::eBlendAlphaTest );
	}

	void WaterRenderPass::doUpdatePipeline( castor3d::RenderPipeline & pipeline )
	{
	}

	castor3d::ShaderPtr WaterRenderPass::doGetHullShaderSource( castor3d::PipelineFlags const & flags )const
	{
		return nullptr;
	}

	castor3d::ShaderPtr WaterRenderPass::doGetDomainShaderSource( castor3d::PipelineFlags const & flags )const
	{
		return nullptr;
	}

	castor3d::ShaderPtr WaterRenderPass::doGetGeometryShaderSource( castor3d::PipelineFlags const & flags )const
	{
		return nullptr;
	}

	castor3d::ShaderPtr WaterRenderPass::doGetPixelShaderSource( castor3d::PipelineFlags const & flags )const
	{
#if Water_Debug
#	define displayDebugData( WaterDataType, RGB, A )\
	IF( writer, c3d_waterData.debug.x() == sdw::UInt( WaterDataType ) )\
	{\
		pxl_colour = vec4( RGB, A );\
		writer.returnStmt();\
	}\
	FI
#else
#	define displayDebugData( WaterDataType, RGB, A )
#endif

		using namespace sdw;
		using namespace castor3d;
		FragmentWriter writer;

		auto & renderSystem = *getEngine()->getRenderSystem();
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();
		bool hasDiffuseGI = checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing )
			|| checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
			|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );

		shader::Utils utils{ writer, *getEngine() };
		shader::CookTorranceBRDF cookTorrance{ writer, utils };

		shader::Materials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials )
			, RenderPipeline::eBuffers );
		shader::TextureConfigurations textureConfigs{ writer };
		shader::TextureAnimations textureAnims{ writer };

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexConfigs )
				, RenderPipeline::eBuffers );
			textureAnims.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexAnims )
				, RenderPipeline::eBuffers );
		}

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );

		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		UBO_MATRIX( writer
			, uint32_t( PassUboIdx::eMatrix )
			, RenderPipeline::eAdditional );
		UBO_SCENE( writer
			, uint32_t( PassUboIdx::eScene )
			, RenderPipeline::eAdditional );
		auto index = uint32_t( PassUboIdx::eCount );
		auto lightsIndex = index++;
		UBO_WATER( writer
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_waveNormals1 = writer.declSampledImage< FImg2DRgba32 >( "c3d_waveNormals1"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_waveNormals2 = writer.declSampledImage< FImg2DRgba32 >( "c3d_waveNormals2"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_waveNoise = writer.declSampledImage< FImg2DR32 >( "c3d_waveNoise"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_normals = writer.declSampledImage< FImg2DRgba32 >( "c3d_normals"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_depth = writer.declSampledImage< FImg2DR32 >( "c3d_depth"
			, index++
			, RenderPipeline::eAdditional );
		auto c3d_colour = writer.declSampledImage< FImg2DRgba32 >( "c3d_colour"
			, index++
			, RenderPipeline::eAdditional );
		auto lightingModel = shader::LightingModel::createModel( utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, lightsIndex
			, RenderPipeline::eAdditional
			, shader::ShadowOptions{ flags.sceneFlags, false }
			, index
			, RenderPipeline::eAdditional
			, false
			, renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto reflections = lightingModel->getReflectionModel( flags.passFlags
			, index
			, uint32_t( RenderPipeline::eAdditional ) );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eAdditional
			, flags.sceneFlags );
		shader::Fog fog{ writer };

		// Fragment Outputs
		auto pxl_colour( writer.declOutput< Vec4 >( "pxl_colour", 0 ) );

		writer.implementMainT< castor3d::shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< castor3d::shader::FragmentSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, true }
			, FragmentOut{ writer }
			, [&]( FragmentInT< castor3d::shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent ) );

				auto normalMapCoords1 = writer.declLocale( "normalMapCoords1"
					, in.texture0.xy() + c3d_waterData.time * c3d_waterData.normalMapScroll.xy() * c3d_waterData.normalMapScrollSpeed.x() );
				auto normalMapCoords2 = writer.declLocale( "normalMapCoords2"
					, in.texture0.xy() + c3d_waterData.time * c3d_waterData.normalMapScroll.zw() * c3d_waterData.normalMapScrollSpeed.y() );
				auto hdrCoords = writer.declLocale( "hdrCoords"
					, in.fragCoord.xy() / vec2( c3d_colour.getSize( 0_i ) ) );

				auto normalMap1 = writer.declLocale( "normalMap1"
					, ( c3d_waveNormals1.sample( normalMapCoords1 ).rgb() * 2.0 ) - 1.0 );
				auto normalMap2 = writer.declLocale( "normalMap2"
					, ( c3d_waveNormals2.sample( normalMapCoords2 ).rgb() * 2.0 ) - 1.0 );
				auto texSpace = writer.declLocale( "texSpace"
					, mat3( tangent, bitangent, normal ) );
				auto finalNormal = writer.declLocale( "finalNormal"
					, normalize( texSpace * normalMap1.xyz() ) );
				finalNormal += normalize( texSpace * normalMap2.xyz() );
				finalNormal = normalize( finalNormal );
				displayDebugData( eFinalNormal, finalNormal, 1.0_f );

				if ( checkFlag( flags.programFlags, castor3d::ProgramFlag::eInvertNormals ) )
				{
					finalNormal = -finalNormal;
				}

				auto material = writer.declLocale( "material"
					, materials.getMaterial( in.material ) );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive ) );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_sceneData.cameraPosition );
				auto lightMat = lightingModel->declMaterial( "lightMat" );
				lightMat->create( material );
				displayDebugData( eMatSpecular, lightMat->specular, 1.0_f );

				if ( checkFlag( flags.passFlags, PassFlag::eLighting ) )
				{
					// Direct Lighting
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular };
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( in.fragCoord.xy(), in.viewPosition.xyz(), in.worldPosition.xyz(), finalNormal );
					lightingModel->computeCombined( *lightMat
						, c3d_sceneData
						, surface
						, worldEye
						, c3d_modelData.isShadowReceiver()
						, output );
					lightMat->adjustDirectSpecular( lightSpecular );
					displayDebugData( eLightDiffuse, lightDiffuse, 1.0_f );
					displayDebugData( eLightSpecular, lightSpecular, 1.0_f );
					// Standard lighting models don't necessarily translate all that well to water.
					// It can end up looking very glossy and plastic-like, having much more form than it really should.
					// So here, I'm sampling some noise with three different sets of texture coordinates to try and achieve
					// that sparkling look that defines water specularity.
					auto specularNoise = writer.declLocale( "specularNoise"
						, c3d_waveNoise.sample( normalMapCoords1 * 0.5 ) );
					specularNoise *= c3d_waveNoise.sample( normalMapCoords2 * 0.5 );
					specularNoise *= c3d_waveNoise.sample( in.texture0.xy() * 0.5 );
					lightSpecular *= specularNoise;
					displayDebugData( eNoisedSpecular, lightSpecular, 1.0_f );
					displayDebugData( eSpecularNoise, vec3( specularNoise ), 1.0_f );


					// Indirect Lighting
					auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
						, 1.0_f );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.sceneFlags
						, surface
						, indirectOcclusion );
					displayDebugData( eIndirectOcclusion, vec3( indirectOcclusion ), 1.0_f );
					displayDebugData( eLightIndirectDiffuse, lightIndirectDiffuse.xyz(), 1.0_f );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.sceneFlags
						, worldEye
						, c3d_sceneData.getPosToCamera( surface.worldPosition )
						, surface
						, lightMat->specular
						, lightMat->getRoughness()
						, indirectOcclusion
						, lightIndirectDiffuse.w() );
					displayDebugData( eLightIndirectSpecular, lightIndirectSpecular, 1.0_f );
					auto indirectAmbient = writer.declLocale( "indirectAmbient"
						, lightMat->getIndirectAmbient( indirect.computeAmbient( flags.sceneFlags, lightIndirectDiffuse.xyz() ) ) );
					displayDebugData( eIndirectAmbient, indirectAmbient, 1.0_f );
					auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
						, ( hasDiffuseGI
							? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
								, c3d_sceneData.cameraPosition
								, surface.worldNormal
								, lightMat->specular
								, lightMat->getMetalness()
								, surface )
							: vec3( 0.0_f ) ) );
					displayDebugData( eIndirectDiffuse, indirectDiffuse, 1.0_f );


					// Reflection
					auto backgroundReflection = writer.declLocale( "backgroundReflection"
						, reflections->computeForward( *lightMat
							, surface
							, c3d_sceneData ) );
					displayDebugData( eBackgroundReflection, backgroundReflection, 1.0_f );
					auto ssrResult = writer.declLocale( "ssrResult"
						, reflections->computeScreenSpace( c3d_matrixData
							, surface.viewPosition
							, finalNormal
							, hdrCoords
							, c3d_waterData.ssrSettings
							, c3d_depth
							, c3d_normals
							, c3d_colour ) );
					displayDebugData( eSSRResult, ssrResult.xyz(), 1.0_f );
					displayDebugData( eSSRFactor, ssrResult.www(), 1.0_f );
					displayDebugData( eSSRResultFactor, ssrResult.xyz() * ssrResult.www(), 1.0_f );
					auto reflectionResult = writer.declLocale( "reflectionResult"
						, mix( backgroundReflection, ssrResult.xyz(), ssrResult.www() ) );
					displayDebugData( eReflection, reflectionResult, 1.0_f );


					// Refraction
					// Wobbly refractions
					auto distortedTexCoord = writer.declLocale( "distortedTexCoord"
						, ( hdrCoords + ( ( finalNormal.xz() + finalNormal.xy() ) * 0.5 ) * c3d_waterData.refractionDistortionFactor ) );
					auto distortedDepth = writer.declLocale( "distortedDepth"
						, c3d_depth.sample( distortedTexCoord ) );
					auto distortedPosition = writer.declLocale( "distortedPosition"
						, c3d_matrixData.curProjToWorld( utils, distortedTexCoord, distortedDepth ) );
					auto refractionTexCoord = writer.declLocale( "refractionTexCoord"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedTexCoord, hdrCoords ) );
					auto refractionResult = writer.declLocale( "refractionResult"
						, c3d_colour.sample( refractionTexCoord ).rgb() * material.transmission );
					displayDebugData( eRefraction, refractionResult, 1.0_f );
					//  Retrieve non distorted scene colour.
					auto sceneDepth = writer.declLocale( "sceneDepth"
						, c3d_depth.sample( hdrCoords ) );
					auto scenePosition = writer.declLocale( "scenePosition"
						, c3d_matrixData.curProjToWorld( utils, hdrCoords, sceneDepth ) );
					// Depth softening, to fade the alpha of the water where it meets the scene geometry by some predetermined distance. 
					auto depthSoftenedAlpha = writer.declLocale( "depthSoftenedAlpha"
						, clamp( distance( scenePosition, in.worldPosition.xyz() ) / c3d_waterData.depthSofteningDistance, 0.0_f, 1.0_f ) );
					displayDebugData( eDepthSoftenedAlpha, vec3( depthSoftenedAlpha ), 1.0_f );
					auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedPosition, scenePosition ) );
					auto waterTransmission = writer.declLocale( "waterTransmission"
						, material.transmission.rgb() * ( indirectAmbient + indirectDiffuse ) );
					auto heightFactor = writer.declLocale( "heightFactor"
						, c3d_waterData.refractionHeightFactor * ( c3d_sceneData.farPlane - c3d_sceneData.nearPlane ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, vec3( clamp( ( in.worldPosition.y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
					displayDebugData( eHeightMixedRefraction, refractionResult, 1.0_f );
					auto distanceFactor = writer.declLocale( "distanceFactor"
						, c3d_waterData.refractionDistanceFactor * ( c3d_sceneData.farPlane - c3d_sceneData.nearPlane ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, utils.saturate( vec3( utils.saturate( length( in.viewPosition ) / distanceFactor ) ) ) );
					displayDebugData( eDistanceMixedRefraction, refractionResult, 1.0_f );


					//Combine all that
					auto fresnelFactor = writer.declLocale( "fresnelFactor"
						, vec3( reflections->computeFresnel( *lightMat
							, surface
							, c3d_sceneData
							, c3d_waterData.refractionRatio ) ) );
					displayDebugData( eFresnelFactor, vec3( fresnelFactor ), 1.0_f );
					reflectionResult *= fresnelFactor;
					displayDebugData( eFinalReflection, reflectionResult, 1.0_f );
					refractionResult *= vec3( 1.0_f ) - fresnelFactor;
					displayDebugData( eFinalRefraction, refractionResult, 1.0_f );

					pxl_colour = vec4( lightSpecular + lightIndirectSpecular
							+ emissive
							+ ( refractionResult )
							+ ( reflectionResult * indirectAmbient )
						, depthSoftenedAlpha );

				}
				else
				{
					pxl_colour = vec4( lightMat->albedo, 1.0_f );
				}

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_colour = fog.apply( c3d_sceneData.getBackgroundColour( utils )
						, pxl_colour
						, length( in.viewPosition )
						, in.viewPosition.y()
						, c3d_sceneData );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}

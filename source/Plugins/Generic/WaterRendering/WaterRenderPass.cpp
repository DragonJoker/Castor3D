#include "WaterRenderPass.hpp"

#include "WaterUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderQueue.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp>
#include <Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp>
#include <Castor3D/Render/Node/BillboardRenderNode.hpp>
#include <Castor3D/Render/Node/SubmeshRenderNode.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/Program.hpp>
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
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
#include <Castor3D/Shader/Ubos/ModelDataUbo.hpp>
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
			eLightBuffer = uint32_t( castor3d::GlobalBuffersIdx::eCount ),
			eWaterUbo,
			eWaterNormals1,
			eWaterNormals2,
			eWaterNoise,
			eSceneNormals,
			eSceneDepth,
			eSceneResult,
			eBrdf,
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

				engine.postEvent( castor3d::makeGpuFunctorEvent( castor3d::EventType::ePreRender
					, [format, dim, buffer, &graph, &img, &view, &result]( castor3d::RenderDevice const & device
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
					} ) );
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
		, castor3d::RenderNodesPassDesc const & renderPassDesc
		, castor3d::RenderTechniquePassDesc const & techniquePassDesc
		, std::shared_ptr< IsRenderPassEnabled > isEnabled )
		: castor3d::RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, category
			, "Water"
			, parent->getResultImg().data
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
		auto & blitColourPass = graph.createPass( "CopyColour"
			, [extent, &device, isEnabled]( crg::FramePass const & framePass
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
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
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
		auto & blitDepthPass = graph.createPass( "CopyDepth"
			, [extent, &device, isEnabled]( crg::FramePass const & framePass
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
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		blitDepthPass.addDependencies( previousPasses );
		blitDepthPass.addTransferInputView( technique.getDepthSampledView() );
		blitDepthPass.addTransferOutputView( depthInput->sampledViewId );

		auto & result = graph.createPass( "NodesPass"
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
				, castor3d::RenderNodesPassDesc{ extent
					, technique.getMatrixUbo()
					, technique.getSceneUbo()
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
			device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
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
	}

	bool WaterRenderPass::doIsValidPass( castor3d::Pass const & pass )const
	{
		return pass.getRenderPassInfo()
			&& pass.getRenderPassInfo()->name == Type
			&& doAreValidPassFlags( pass.getPassFlags() );
	}

	void WaterRenderPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
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
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaterIdx::eBrdf
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto index = uint32_t( WaterIdx::eBrdf ) + 1u;
		doAddShadowBindings( bindings, index );
		doAddEnvBindings( bindings, index );
		doAddBackgroundBindings( bindings, index );
		doAddGIBindings( bindings, index );
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

	void WaterRenderPass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, castor3d::ShadowMapLightTypeArray const & shadowMaps )
	{
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( WaterIdx::eLightBuffer ) );
		descriptorWrites.push_back( m_ubo.getDescriptorWrite( WaterIdx::eWaterUbo ) );
		auto index = uint32_t( WaterIdx::eWaterNormals1 );
		bindTexture( m_normals1, *m_linearWrapSampler, descriptorWrites, index );
		bindTexture( m_normals2, *m_linearWrapSampler, descriptorWrites, index );
		bindTexture( m_noise, *m_linearWrapSampler, descriptorWrites, index );
		bindTexture( m_parent->getNormalTexture().sampledView, *m_pointClampSampler, descriptorWrites, index );
		bindTexture( m_depthInput->sampledView, *m_pointClampSampler, descriptorWrites, index );
		bindTexture( m_colourInput->sampledView, *m_pointClampSampler, descriptorWrites, index );
		bindTexture( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampledView
			, *getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampler
			, descriptorWrites
			, index );
		doAddShadowDescriptor( descriptorWrites, shadowMaps, index );
		doAddEnvDescriptor( descriptorWrites, shadowMaps, index );
		doAddBackgroundDescriptor( descriptorWrites, *m_targetImage, shadowMaps, index );
		doAddGIDescriptor( descriptorWrites, shadowMaps, index );
	}

	castor3d::SubmeshFlags WaterRenderPass::doAdjustSubmeshFlags( castor3d::SubmeshFlags flags )const
	{
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords1 );
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords2 );
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords3 );
		return flags;
	}

	castor3d::PassFlags WaterRenderPass::doAdjustPassFlags( castor3d::PassFlags flags )const
	{
		remFlag( flags, castor3d::PassFlag::eParallaxOcclusionMappingOne );
		remFlag( flags, castor3d::PassFlag::eParallaxOcclusionMappingRepeat );
		remFlag( flags, castor3d::PassFlag::eDistanceBasedTransmittance );
		remFlag( flags, castor3d::PassFlag::eSubsurfaceScattering );
		remFlag( flags, castor3d::PassFlag::eAlphaBlending );
		remFlag( flags, castor3d::PassFlag::eAlphaTest );
		remFlag( flags, castor3d::PassFlag::eBlendAlphaTest );
		return flags;
	}

	castor3d::ProgramFlags WaterRenderPass::doAdjustProgramFlags( castor3d::ProgramFlags flags )const
	{
		remFlag( flags, castor3d::ProgramFlag::eInstantiation );
		addFlag( flags, castor3d::ProgramFlag::eForceTexCoords );
		addFlag( flags, castor3d::ProgramFlag::eLighting );
		return flags;
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

		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();
		bool hasDiffuseGI = checkFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing )
			|| checkFlag( flags.sceneFlags, SceneFlag::eLpvGI )
			|| checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );

		shader::Utils utils{ writer };
		shader::CookTorranceBRDF cookTorrance{ writer, utils };
		shader::Fog fog{ writer };

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, hasTextures };
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = index++;
		C3D_Water( writer
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_waveNormals1 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_waveNormals1"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_waveNormals2 = writer.declCombinedImg< FImg2DRgba32 >( "c3d_waveNormals2"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_waveNoise = writer.declCombinedImg< FImg2DR32 >( "c3d_waveNoise"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_normals = writer.declCombinedImg< FImg2DRgba32 >( "c3d_normals"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_depth = writer.declCombinedImg< FImg2DR32 >( "c3d_depth"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_colour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_colour"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRg32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eBuffers );
		auto lightingModel = shader::LightingModel::createModel( *getEngine()
			, utils
			, getScene().getLightingModel()
			, lightsIndex
			, RenderPipeline::eBuffers
			, shader::ShadowOptions{ flags.sceneFlags, true, false }
			, nullptr
			, index
			, RenderPipeline::eBuffers
			, false );
		auto reflections = lightingModel->getReflectionModel( index
			, uint32_t( RenderPipeline::eBuffers ) );
		auto backgroundModel = shader::BackgroundModel::createModel( getScene()
			, writer
			, utils
			, castor3d::makeExtent2D( m_size )
			, index
			, RenderPipeline::eBuffers );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eBuffers
			, flags.sceneFlags );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		// Fragment Outputs
		auto pxl_colour( writer.declOutput< Vec4 >( "pxl_colour", 0 ) );

		writer.implementMainT< castor3d::shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< castor3d::shader::FragmentSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, true }
			, FragmentOut{ writer }
			, [&]( FragmentInT< castor3d::shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( in.nodeId ) - 1u] );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent )
					, checkFlag( flags.submeshFlags, SubmeshFlag::eTangents ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent )
					, checkFlag( flags.submeshFlags, SubmeshFlag::eTangents ) );

				auto normalMapCoords1 = writer.declLocale( "normalMapCoords1"
					, in.texture0.xy() + c3d_waterData.time * c3d_waterData.normalMapScroll.xy() * c3d_waterData.normalMapScrollSpeed.x() );
				auto normalMapCoords2 = writer.declLocale( "normalMapCoords2"
					, in.texture0.xy() + c3d_waterData.time * c3d_waterData.normalMapScroll.zw() * c3d_waterData.normalMapScrollSpeed.y() );
				auto hdrCoords = writer.declLocale( "hdrCoords"
					, in.fragCoord.xy() / vec2( c3d_colour.getSize( 0_i ) ) );

				auto normalMap1 = writer.declLocale( "normalMap1"
					, ( c3d_waveNormals1.sample( normalMapCoords1 ).rgb() * 2.0_f ) - 1.0_f );
				auto normalMap2 = writer.declLocale( "normalMap2"
					, ( c3d_waveNormals2.sample( normalMapCoords2 ).rgb() * 2.0_f ) - 1.0_f );
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
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive() ) );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_sceneData.cameraPosition );
				auto lightMat = lightingModel->declMaterial( "lightMat" );
				lightMat->create( in.colour
					, material );
				displayDebugData( eMatSpecular, lightMat->specular, 1.0_f );

				IF( writer, material.lighting() != 0_u )
				{
					// Direct Lighting
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					auto lightScattering = writer.declLocale( "lightScattering"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular, lightScattering };
					auto surface = writer.declLocale< shader::Surface >( "surface" );
					surface.create( in.fragCoord.xyz()
						, in.viewPosition.xyz()
						, in.worldPosition.xyz()
						, finalNormal );
					lightingModel->computeCombined( *lightMat
						, c3d_sceneData
						, *backgroundModel
						, surface
						, worldEye
						, modelData.isShadowReceiver()
						, output );
					lightMat->adjustDirectSpecular( lightSpecular );
					displayDebugData( eLightDiffuse, lightDiffuse, 1.0_f );
					displayDebugData( eLightSpecular, lightSpecular, 1.0_f );
					displayDebugData( eLightScattering, lightScattering, 1.0_f );
					// Standard lighting models don't necessarily translate all that well to water.
					// It can end up looking very glossy and plastic-like, having much more form than it really should.
					// So here, I'm sampling some noise with three different sets of texture coordinates to try and achieve
					// that sparkling look that defines water specularity.
					auto specularNoise = writer.declLocale( "specularNoise"
						, c3d_waveNoise.sample( normalMapCoords1 * 0.5_f ) );
					specularNoise *= c3d_waveNoise.sample( normalMapCoords2 * 0.5_f );
					specularNoise *= c3d_waveNoise.sample( in.texture0.xy() * 0.5_f );
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
						, lightMat->getRoughness()
						, indirectOcclusion
						, lightIndirectDiffuse.w()
						, c3d_mapBrdf );
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
					auto backgroundReflection = reflections->computeForward( *lightMat
						, surface
						, c3d_sceneData
						, *backgroundModel
						, material.hasReflection() );
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
						, ( hdrCoords + ( ( finalNormal.xz() + finalNormal.xy() ) * 0.5_f ) * c3d_waterData.refractionDistortionFactor ) );
					auto distortedDepth = writer.declLocale( "distortedDepth"
						, c3d_depth.sample( distortedTexCoord ) );
					auto distortedPosition = writer.declLocale( "distortedPosition"
						, c3d_matrixData.curProjToWorld( utils, distortedTexCoord, distortedDepth ) );
					auto refractionTexCoord = writer.declLocale( "refractionTexCoord"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedTexCoord, hdrCoords ) );
					auto refractionResult = writer.declLocale( "refractionResult"
						, c3d_colour.sample( refractionTexCoord ).rgb() * material.transmission() );
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
						, material.transmission().rgb() * ( indirectAmbient + indirectDiffuse ) * lightDiffuse );
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
							+ lightScattering
						, depthSoftenedAlpha );

				}
				ELSE
				{
					pxl_colour = vec4( lightMat->albedo, 1.0_f );
				}
				FI;

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_colour = fog.apply( c3d_sceneData.getBackgroundColour( utils )
						, pxl_colour
						, in.worldPosition.xyz()
						, c3d_sceneData );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}

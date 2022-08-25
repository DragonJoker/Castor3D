#include "OceanRenderPass.hpp"

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
#include <Castor3D/Shader/Ubos/ObjectIdsUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

#include <ashespp/Image/StagingTexture.hpp>

#pragma clang diagnostic ignored "-Wunused-member-function"

#define Ocean_DebugPixelShader 0

namespace ocean
{
	namespace
	{
		enum WaveIdx : uint32_t
		{
			eLightBuffer = uint32_t( castor3d::GlobalBuffersIdx::eCount ),
			eWavesUbo,
			eWaveFoam,
			eWaveNormals1,
			eWaveNormals2,
			eWaveNoise,
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

		struct WaveResult
			: sdw::StructInstance
		{
			WaveResult( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
				, position{ getMember< sdw::Vec3 >( "position" ) }
				, normal{ getMember< sdw::Vec3 >( "normal" ) }
				, bitangent{ getMember< sdw::Vec3 >( "bitangent" ) }
				, tangent{ getMember< sdw::Vec3 >( "tangent" ) }
			{
			}

			SDW_DeclStructInstance( , WaveResult );

			static sdw::type::StructPtr makeType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
					, "WaveResult" );

				if ( result->empty() )
				{
					result->declMember( "position", sdw::type::Kind::eVec3F, sdw::type::NotArray );
					result->declMember( "normal", sdw::type::Kind::eVec3F, sdw::type::NotArray );
					result->declMember( "bitangent", sdw::type::Kind::eVec3F, sdw::type::NotArray );
					result->declMember( "tangent", sdw::type::Kind::eVec3F, sdw::type::NotArray );
					result->end();
				}

				return result;
			}

			sdw::Vec3 position;
			sdw::Vec3 normal;
			sdw::Vec3 bitangent;
			sdw::Vec3 tangent;
		};

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

	castor::String const OceanRenderPass::Type = cuT( "c3d.ocean" );
	castor::String const OceanRenderPass::Name = cuT( "Ocean" );
	castor::String const OceanRenderPass::FullName = cuT( "Ocean rendering" );
	castor::String const OceanRenderPass::Param = cuT( "OceanConfig" );
	castor::String const OceanRenderPass::Foam = cuT( "WavesFoam" );
	castor::String const OceanRenderPass::Normals1 = cuT( "WavesNormals1" );
	castor::String const OceanRenderPass::Normals2 = cuT( "WavesNormals2" );
	castor::String const OceanRenderPass::Noise = cuT( "WavesNoise" );

	OceanRenderPass::OceanRenderPass( castor3d::RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
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
		loadImage( params, Foam, getName(), *getEngine(), pass.graph, loader, m_foamImg, m_foamView, m_foam );
		loadImage( params, Noise, getName(), *getEngine(), pass.graph, loader, m_noiseImg, m_noiseView, m_noise );
		loadImage( params, Normals1, getName(), *getEngine(), pass.graph, loader, m_normals1Img, m_normals1View, m_normals1 );
		loadImage( params, Normals2, getName(), *getEngine(), pass.graph, loader, m_normals2Img, m_normals2View, m_normals2 );
	}

	OceanRenderPass::~OceanRenderPass()
	{
		m_colourInput->destroy();
		m_colourInput.reset();
		m_depthInput->destroy();
		m_depthInput.reset();
	}

	crg::FramePassArray OceanRenderPass::create( castor3d::RenderDevice const & device
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
			, name +"/Colour"
			, 0u
			, extent
			, 1u
			, 1u
			, technique.getResultImg().data->info.format
			, ( VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT ) );
		colourInput->create();
		auto & blitColourPass = graph.createPass( "CopyColour"
			, [extent, isEnabled, &device]( crg::FramePass const & framePass
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
			, [extent, isEnabled, &device]( crg::FramePass const & framePass
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
			, [extent, colourInput, depthInput, isEnabled, &device, &technique, &renderPasses]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
		{
			auto res = std::make_unique< OceanRenderPass >( &technique
				, framePass
				, context
				, runnableGraph
				, device
				, std::move( colourInput )
				, std::move( depthInput )
				, castor3d::RenderNodesPassDesc{ extent
					, technique.getMatrixUbo()
					, technique.getSceneUbo()
					, technique.getRenderTarget().getCuller() }.safeBand( true )
				, castor3d::RenderTechniquePassDesc{ false, technique.getSsaoConfig() }
					.ssao( technique.getSsaoResult() )
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

	castor3d::TextureFlags OceanRenderPass::getTexturesMask()const
	{
		return castor3d::TextureFlags{ castor3d::TextureFlag::eAll };
	}

	void OceanRenderPass::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID )
		{
#if Ocean_Debug
			visitor.visit( cuT( "Debug" )
				, m_configuration.debug
				, getOceanDisplayDataNames()
				, nullptr );
#endif
			visitor.visit( cuT( "Tessellation factor" )
				, m_configuration.tessellationFactor
				, nullptr );
			visitor.visit( cuT( "Dampening factor" )
				, m_configuration.dampeningFactor
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
			visitor.visit( cuT( "Depth softening distance" )
				, m_configuration.depthSofteningDistance
				, nullptr );
			visitor.visit( cuT( "Foam height start" )
				, m_configuration.foamHeightStart
				, nullptr );
			visitor.visit( cuT( "Foam fade distance" )
				, m_configuration.foamFadeDistance
				, nullptr );
			visitor.visit( cuT( "Foam tiling" )
				, m_configuration.foamTiling
				, nullptr );
			visitor.visit( cuT( "Foam angle exponent" )
				, m_configuration.foamAngleExponent
				, nullptr );
			visitor.visit( cuT( "Foam brightness" )
				, m_configuration.foamBrightness
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
			visitor.visit( cuT( "Wave count" )
				, m_configuration.numWaves
				, nullptr );
			uint32_t index = 0u;

			for ( auto & wave : m_configuration.waves )
			{
				auto prefix = cuT( "Wave " ) + castor::string::toString( index++ );
				visitor.visit( prefix + cuT( " direction" )
					, wave.direction
					, nullptr );
				visitor.visit( prefix + cuT( " steepness" )
					, wave.steepness
					, nullptr );
				visitor.visit( prefix + cuT( " length" )
					, wave.length
					, nullptr );
				visitor.visit( prefix + cuT( " amplitude" )
					, wave.amplitude
					, nullptr );
				visitor.visit( prefix + cuT( " speed" )
					, wave.speed
					, nullptr );
			}
		}
	}

	void OceanRenderPass::doUpdateUbos( castor3d::CpuUpdater & updater )
	{
		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );
		m_configuration.time += float( tslf.count() ) / 1000.0f;
		m_ubo.cpuUpdate( m_configuration );
	}

	bool OceanRenderPass::doIsValidPass( castor3d::Pass const & pass )const
	{
		return pass.getRenderPassInfo()
			&& pass.getRenderPassInfo()->name == Type
			&& areValidPassFlags( pass.getPassFlags() );
	}

	void OceanRenderPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		bindings.emplace_back( getCuller().getScene().getLightCache().createLayoutBinding( WaveIdx::eLightBuffer ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eWavesUbo
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_VERTEX_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_FRAGMENT_BIT ) ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eWaveFoam
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eWaveNormals1
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eWaveNormals2
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eWaveNoise
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eSceneNormals
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eSceneDepth
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eSceneResult
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( WaveIdx::eBrdf
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto index = uint32_t( WaveIdx::eBrdf ) + 1u;
		doAddShadowBindings( bindings, index );
		doAddEnvBindings( bindings, index );
		doAddBackgroundBindings( bindings, index );
		doAddGIBindings( bindings, index );
	}

	ashes::PipelineDepthStencilStateCreateInfo OceanRenderPass::doCreateDepthStencilState( castor3d::PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo
		{
			0u,
			VK_TRUE,
			VK_TRUE,
		};
	}

	ashes::PipelineColorBlendStateCreateInfo OceanRenderPass::doCreateBlendState( castor3d::PipelineFlags const & flags )const
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments
		{
			VkPipelineColorBlendAttachmentState
			{
				VK_TRUE,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ZERO,
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

	void OceanRenderPass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, castor3d::ShadowMapLightTypeArray const & shadowMaps )
	{
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( WaveIdx::eLightBuffer ) );
		descriptorWrites.push_back( m_ubo.getDescriptorWrite( WaveIdx::eWavesUbo ) );
		auto index = uint32_t( WaveIdx::eWaveFoam );
		bindTexture( m_foam, *m_linearWrapSampler, descriptorWrites, index );
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

	castor3d::SubmeshFlags OceanRenderPass::doAdjustSubmeshFlags( castor3d::SubmeshFlags flags )const
	{
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords2 );
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords3 );
		return flags;
	}

	castor3d::PassFlags OceanRenderPass::doAdjustPassFlags( castor3d::PassFlags flags )const
	{
		remFlag( flags, castor3d::PassFlag::eParallaxOcclusionMappingOne );
		remFlag( flags, castor3d::PassFlag::eParallaxOcclusionMappingRepeat );
		remFlag( flags, castor3d::PassFlag::eDistanceBasedTransmittance );
		remFlag( flags, castor3d::PassFlag::eSubsurfaceScattering );
		remFlag( flags, castor3d::PassFlag::eAlphaBlending );
		remFlag( flags, castor3d::PassFlag::eAlphaTest );
		return flags;
	}

	castor3d::ProgramFlags OceanRenderPass::doAdjustProgramFlags( castor3d::ProgramFlags flags )const
	{
		remFlag( flags, castor3d::ProgramFlag::eInstantiation );
		return flags;
	}

	void OceanRenderPass::doAdjustFlags( castor3d::PipelineFlags & flags )const
	{
#if !Ocean_DebugPixelShader
		flags.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		flags.patchVertices = 3u;
#endif
	}

	castor3d::ShaderPtr OceanRenderPass::doGetVertexShaderSource( castor3d::PipelineFlags const & flags )const
	{
		using namespace sdw;
		using namespace castor3d;
		VertexWriter writer;

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< castor3d::shader::VertexSurfaceT, castor3d::shader::FragmentSurfaceT >( sdw::VertexInT< castor3d::shader::VertexSurfaceT >{ writer
				, flags }
			, sdw::VertexOutT< castor3d::shader::FragmentSurfaceT >{ writer
				, flags }
			, [&]( VertexInT< castor3d::shader::VertexSurfaceT > in
				, VertexOutT< castor3d::shader::FragmentSurfaceT > out )
		{
			out.vtx.position = in.position;
			out.texture0 = in.texture0;
			out.texture1 = in.texture1;
			out.colour = in.colour;
			auto nodeId = writer.declLocale( "nodeId"
				, shader::getNodeId( c3d_objectIdsData
					, in
					, pipelineID
					, writer.cast< sdw::UInt >( in.drawID )
					, flags ) );
			auto modelData = writer.declLocale( "modelData"
				, c3d_modelsData[nodeId - 1u] );
			out.nodeId = writer.cast< sdw::Int >( nodeId );

#if Ocean_DebugPixelShader
			auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
				, c3d_modelData.getCurModelMtx( flags.programFlags
					, skinningData
					, in.boneIds0
					, in.boneIds1
					, in.boneWeights0
					, in.boneWeights1
					, in.transform ) );
			out.worldPosition = curMtxModel * out.vtx.position;
			out.viewPosition = c3d_matrixData.worldToCurView( out.worldPosition );
			out.vtx.position = c3d_matrixData.viewToProj( out.viewPosition );
			auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
				, c3d_modelData.getNormalMtx( flags.programFlags, curMtxModel ) );
			out.computeTangentSpace( flags.submeshFlags
				, flags.programFlags
				, c3d_sceneData.cameraPosition
				, out.worldPosition.xyz()
				, mtxNormal
				, v4Normal
				, v4Tangent );
#endif
		} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	castor3d::ShaderPtr OceanRenderPass::doGetBillboardShaderSource( castor3d::PipelineFlags const & flags )const
	{
		using namespace sdw;
		using namespace castor3d;
		VertexWriter writer;

		// Shader inputs
		auto position = writer.declInput< Vec4 >( "position", 0u );
		auto uv = writer.declInput< Vec2 >( "uv", 1u );
		auto center = writer.declInput< Vec3 >( "center", 2u );

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		C3D_Billboard( writer
			, GlobalBuffersIdx::eBillboardsData
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< VoidT, castor3d::shader::FragmentSurfaceT >( sdw::VertexInT< sdw::VoidT >{ writer }
			, sdw::VertexOutT< castor3d::shader::FragmentSurfaceT >{ writer
				, flags }
			, [&]( VertexInT< VoidT > in
				, VertexOutT< castor3d::shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID ) ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				out.nodeId = writer.cast< sdw::Int >( nodeId );

				auto curBbcenter = writer.declLocale( "curBbcenter"
					, modelData.modelToCurWorld( vec4( center, 1.0_f ) ).xyz() );
				auto prvBbcenter = writer.declLocale( "prvBbcenter"
					, modelData.modelToPrvWorld( vec4( center, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_sceneData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId - 1u] );
				auto right = writer.declLocale( "right"
					, billboardData.getCameraRight( c3d_matrixData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( c3d_matrixData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( c3d_sceneData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( c3d_sceneData ) );
				auto scaledRight = writer.declLocale( "scaledRight"
					, right * position.x() * width );
				auto scaledUp = writer.declLocale( "scaledUp"
					, up * position.y() * height );
				auto worldPos = writer.declLocale( "worldPos"
					, ( curBbcenter + scaledRight + scaledUp ) );
				out.texture0 = vec3( uv, 1.0_f );
				out.texture1 = vec3( uv * 50.0_f, 1.0_f );
				out.colour = vec3( 1.0_f );
				out.vtx.position = modelData.worldToModel( vec4( worldPos, 1.0_f ) );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	castor3d::ShaderPtr OceanRenderPass::doGetHullShaderSource( castor3d::PipelineFlags const & flags )const
	{
#if Ocean_DebugPixelShader
		return nullptr;
#else
		using namespace sdw;
		using namespace castor3d;
		TessellationControlWriter writer;

		auto index = uint32_t( GlobalBuffersIdx::eCount );
		index++; // lights buffer
		C3D_Ocean( writer
			, index
			, RenderPipeline::eBuffers );

		writer.implementPatchRoutineT< castor3d::shader::FragmentSurfaceT, 3u, VoidT >( TessControlListInT< castor3d::shader::FragmentSurfaceT, 3u >{ writer
				, false
				, flags }
			, TrianglesTessPatchOutT< VoidT >{ writer, 9u }
			, [&]( sdw::TessControlPatchRoutineIn in
				, sdw::TessControlListInT< castor3d::shader::FragmentSurfaceT, 3u > listIn
				, sdw::TrianglesTessPatchOut patchOut )
			{
				patchOut.tessLevelOuter[0] = c3d_oceanData.tessellationFactor;
				patchOut.tessLevelOuter[1] = c3d_oceanData.tessellationFactor;
				patchOut.tessLevelOuter[2] = c3d_oceanData.tessellationFactor;

				patchOut.tessLevelInner[0] = c3d_oceanData.tessellationFactor;
			} );

		writer.implementMainT< castor3d::shader::FragmentSurfaceT, 3u, castor3d::shader::FragmentSurfaceT >( TessControlListInT< castor3d::shader::FragmentSurfaceT, 3u >{ writer
				, true
				, flags }
			, TrianglesTessControlListOutT< castor3d::shader::FragmentSurfaceT >{ writer
				, ast::type::Partitioning::eFractionalOdd
				, ast::type::OutputTopology::eTriangle
				, ast::type::PrimitiveOrdering::eCCW
				, 3u
				, flags }
			, [&]( TessControlMainIn in
				, TessControlListInT< castor3d::shader::FragmentSurfaceT, 3u > listIn
				, TrianglesTessControlListOutT< castor3d::shader::FragmentSurfaceT > listOut )
			{
				listOut.vtx.position = listIn[in.invocationID].vtx.position;
				listOut.nodeId = listIn[in.invocationID].nodeId;
				listOut.texture0 = listIn[in.invocationID].texture0;
				listOut.texture1 = listIn[in.invocationID].texture1;
				listOut.colour = listIn[in.invocationID].colour;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
#endif
	}

	castor3d::ShaderPtr OceanRenderPass::doGetDomainShaderSource( castor3d::PipelineFlags const & flags )const
	{
#if Ocean_DebugPixelShader
		return nullptr;
#else
		using namespace sdw;
		using namespace castor3d;
		TessellationEvaluationWriter writer;

		castor3d::shader::Utils utils{ writer };

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		index++; // lights buffer
		C3D_Ocean( writer
			, index
			, RenderPipeline::eBuffers );

		auto calculateWave = writer.implementFunction< WaveResult >( "calculateWave"
			, [&]( Wave wave
				, Vec3 wavePosition
				, Float edgeDampen
				, Float numWaves
				, Float timeIndex )
			{
				auto result = writer.declLocale< WaveResult >( "result" );

				auto frequency = writer.declLocale( "frequency"
					, 2.0_f / wave.length );
				auto phaseConstant = writer.declLocale( "phaseConstant"
					, wave.speed * frequency );
				auto qi = writer.declLocale( "qi"
					, wave.steepness / ( wave.amplitude * frequency * numWaves ) );
				auto rad = writer.declLocale( "rad"
					, frequency * dot( wave.direction.xz(), wavePosition.xz() ) + timeIndex * phaseConstant );
				auto sinR = writer.declLocale( "sinR"
					, sin( rad ) );
				auto cosR = writer.declLocale( "cosR"
					, cos( rad ) );

				result.position.x() = wavePosition.x() + qi * wave.amplitude * wave.direction.x() * cosR * edgeDampen;
				result.position.z() = wavePosition.z() + qi * wave.amplitude * wave.direction.z() * cosR * edgeDampen;
				result.position.y() = wave.amplitude * sinR * edgeDampen;

				auto waFactor = writer.declLocale( "waFactor"
					, frequency * wave.amplitude );
				auto radN = writer.declLocale( "radN"
					, frequency * dot( wave.direction, result.position ) + timeIndex * phaseConstant );
				auto sinN = writer.declLocale( "sinN"
					, sin( radN ) );
				auto cosN = writer.declLocale( "cosN"
					, cos( radN ) );

				result.bitangent.x() = 1.0_f - ( qi * wave.direction.x() * wave.direction.x() * waFactor * sinN );
				result.bitangent.z() = -1.0_f * ( qi * wave.direction.x() * wave.direction.z() * waFactor * sinN );
				result.bitangent.y() = wave.direction.x() * waFactor * cosN;

				result.tangent.x() = -1.0_f * ( qi * wave.direction.x() * wave.direction.z() * waFactor * sinN );
				result.tangent.z() = 1.0_f - ( qi * wave.direction.z() * wave.direction.z() * waFactor * sinN );
				result.tangent.y() = wave.direction.z() * waFactor * cosN;

				result.normal.x() = -1.0_f * ( wave.direction.x() * waFactor * cosN );
				result.normal.z() = -1.0_f * ( wave.direction.z() * waFactor * cosN );
				result.normal.y() = 1.0_f - ( qi * waFactor * sinN );

				result.bitangent = normalize( result.bitangent );
				result.tangent = normalize( result.tangent );
				result.normal = normalize( result.normal );

				writer.returnStmt( result );
			}
			, InWave{ writer, "wave" }
			, sdw::InVec3{ writer, "wavePosition" }
			, sdw::InFloat{ writer, "edgeDampen" }
			, sdw::InFloat{ writer, "numWaves" }
			, sdw::InFloat{ writer, "timeIndex" } );

		writer.implementMainT< castor3d::shader::FragmentSurfaceT, 3u, VoidT, castor3d::shader::FragmentSurfaceT >( TessEvalListInT< castor3d::shader::FragmentSurfaceT, 3u >{ writer
				, ast::type::PatchDomain::eTriangles
				, type::Partitioning::eFractionalOdd
				, type::PrimitiveOrdering::eCCW
				, flags }
			, TrianglesTessPatchInT< VoidT >{ writer, 9u }
			, TessEvalDataOutT< castor3d::shader::FragmentSurfaceT >{ writer
				, flags }
			, [&]( TessEvalMainIn mainIn
				, TessEvalListInT< castor3d::shader::FragmentSurfaceT, 3u > listIn
				, TrianglesTessPatchInT< VoidT > patchIn
				, TessEvalDataOutT< castor3d::shader::FragmentSurfaceT > out )
			{
				out.vtx.position = patchIn.tessCoord.x() * listIn[0].vtx.position
					+ patchIn.tessCoord.y() * listIn[1].vtx.position
					+ patchIn.tessCoord.z() * listIn[2].vtx.position;
				out.texture1 = patchIn.tessCoord.x() * listIn[0].texture1
					+ patchIn.tessCoord.y() * listIn[1].texture1
					+ patchIn.tessCoord.z() * listIn[2].texture1;
				out.colour = patchIn.tessCoord.x() * listIn[0].colour
					+ patchIn.tessCoord.y() * listIn[1].colour
					+ patchIn.tessCoord.z() * listIn[2].colour;
				out.nodeId = listIn[0].nodeId;
				auto texcoord = writer.declLocale( "texcoord"
					, patchIn.tessCoord.x() * listIn[0].texture0
					+ patchIn.tessCoord.y() * listIn[1].texture0
					+ patchIn.tessCoord.z() * listIn[2].texture0 );

				auto dampening = writer.declLocale( "dampening"
					, 1.0_f - pow( utils.saturate( abs( texcoord.x() - 0.5_f ) / 0.5_f ), c3d_oceanData.dampeningFactor ) );
				dampening *= 1.0_f - pow( utils.saturate( abs( texcoord.y() - 0.5_f ) / 0.5_f ), c3d_oceanData.dampeningFactor );

				auto finalWaveResult = writer.declLocale< WaveResult >( "finalWaveResult" );
				finalWaveResult.position = vec3( 0.0_f );
				finalWaveResult.normal = vec3( 0.0_f );
				finalWaveResult.tangent = vec3( 0.0_f );
				finalWaveResult.bitangent = vec3( 0.0_f );

				auto numWaves = writer.declLocale( "numWaves"
					, writer.cast< UInt >( c3d_oceanData.numWaves ) );

				IF( writer, numWaves > 0_u )
				{
					FOR( writer, UInt, waveId, 0_u, waveId < numWaves, waveId++ )
					{
						auto waveResult = writer.declLocale( "waveResult"
							, calculateWave( c3d_oceanData.waves[waveId]
								, out.vtx.position.xyz()
								, dampening
								, c3d_oceanData.numWaves
								, c3d_oceanData.time ) );
						finalWaveResult.position += waveResult.position;
						finalWaveResult.normal += waveResult.normal;
						finalWaveResult.tangent += waveResult.tangent;
						finalWaveResult.bitangent += waveResult.bitangent;
					}
					ROF;

					finalWaveResult.position -= out.vtx.position.xyz() * ( c3d_oceanData.numWaves - 1.0_f );
					finalWaveResult.normal = normalize( finalWaveResult.normal );
					finalWaveResult.tangent = normalize( finalWaveResult.tangent );
					finalWaveResult.bitangent = normalize( finalWaveResult.bitangent );
				}
				ELSE
				{
					finalWaveResult.position = out.vtx.position.xyz();
					finalWaveResult.normal = vec3( 0.0_f, 1.0_f, 0.0_f );
					finalWaveResult.tangent = vec3( 1.0_f, 0.0_f, 0.0_f );
					finalWaveResult.bitangent = vec3( 0.0_f, 0.0_f, 1.0_f );
				}
				FI;

				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( out.nodeId ) - 1u] );
				auto height = writer.declLocale( "height"
					, finalWaveResult.position.y() - out.vtx.position.y() );
				auto mtxModel = writer.declLocale( "mtxModel"
					, modelData.getModelMtx() );
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, modelData.getNormalMtx( flags, mtxModel ) );

				out.normal = normalize( mtxNormal * finalWaveResult.normal );
				out.tangent = normalize( mtxNormal * finalWaveResult.tangent );
				out.bitangent = normalize( mtxNormal * finalWaveResult.bitangent );
				out.worldPosition = mtxModel * vec4( finalWaveResult.position, 1.0_f );
				out.viewPosition = c3d_matrixData.worldToCurView( out.worldPosition );
				out.worldPosition.w() = height;
				out.vtx.position = c3d_matrixData.viewToProj( out.viewPosition );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
#endif
	}

	castor3d::ShaderPtr OceanRenderPass::doGetPixelShaderSource( castor3d::PipelineFlags const & flags )const
	{
#if Ocean_Debug
#	define displayDebugData( OceanDataType, RGB, A )\
	IF( writer, c3d_oceanData.debug.x() == sdw::UInt( OceanDataType ) )\
	{\
		pxl_colour = vec4( RGB, A );\
		writer.returnStmt();\
	}\
	FI
#else
#	define displayDebugData( OceanDataType, RGB, A )
#endif

		using namespace sdw;
		using namespace castor3d;
		FragmentWriter writer;

		bool hasDiffuseGI = flags.hasDiffuseGI();

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
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = index++;
		C3D_Ocean( writer
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_waveFoam = writer.declCombinedImg< FImg2DRgba32 >( "c3d_waveFoam"
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
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
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
			, flags.getGlobalIlluminationFlags() );

		// Fragment Outputs
		auto pxl_colour( writer.declOutput< Vec4 >( "pxl_colour", 0 ) );

		writer.implementMainT< castor3d::shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< castor3d::shader::FragmentSurfaceT >{ writer
				, flags }
			, FragmentOut{ writer }
			, [&]( FragmentInT< castor3d::shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent ) );

				auto normalMapCoords1 = writer.declLocale( "normalMapCoords1"
					, in.texture1.xy() + c3d_oceanData.time * c3d_oceanData.normalMapScroll.xy() * c3d_oceanData.normalMapScrollSpeed.x() );
				auto normalMapCoords2 = writer.declLocale( "normalMapCoords2"
					, in.texture1.xy() + c3d_oceanData.time * c3d_oceanData.normalMapScroll.zw() * c3d_oceanData.normalMapScrollSpeed.y() );
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

				if ( flags.hasInvertNormals() )
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
					specularNoise *= c3d_waveNoise.sample( in.texture1.xy() * 0.5_f );
					lightSpecular *= specularNoise;
					displayDebugData( eNoisedSpecular, lightSpecular, 1.0_f );
					displayDebugData( eSpecularNoise, vec3( specularNoise ), 1.0_f );


					// Indirect Lighting
					auto indirectOcclusion = writer.declLocale( "indirectOcclusion"
						, 1.0_f );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.getGlobalIlluminationFlags()
						, surface
						, indirectOcclusion );
					displayDebugData( eIndirectOcclusion, vec3( indirectOcclusion ), 1.0_f );
					displayDebugData( eLightIndirectDiffuse, lightIndirectDiffuse.xyz(), 1.0_f );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.getGlobalIlluminationFlags()
						, worldEye
						, c3d_sceneData.getPosToCamera( surface.worldPosition )
						, surface
						, lightMat->getRoughness()
						, indirectOcclusion
						, lightIndirectDiffuse.w()
						, c3d_mapBrdf );
					displayDebugData( eLightIndirectSpecular, lightIndirectSpecular, 1.0_f );
					auto indirectAmbient = writer.declLocale( "indirectAmbient"
						, lightMat->getIndirectAmbient( indirect.computeAmbient( flags.getGlobalIlluminationFlags(), lightIndirectDiffuse.xyz() ) ) );
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
					auto backgroundReflection = reflections->computeReflections( *lightMat
						, surface
						, c3d_sceneData
						, *backgroundModel
						, modelData.getEnvMapIndex()
						, material.hasReflection() );
					displayDebugData( eBackgroundReflection, backgroundReflection, 1.0_f );
					auto ssrResult = writer.declLocale( "ssrResult"
						, reflections->computeScreenSpace( c3d_matrixData
							, surface.viewPosition
							, finalNormal
							, hdrCoords
							, c3d_oceanData.ssrSettings
							, c3d_depth
							, c3d_normals
							, c3d_colour ) );
					displayDebugData( eSSRResult, ssrResult.xyz(), 1.0_f );
					displayDebugData( eSSRFactor, ssrResult.www(), 1.0_f );
					displayDebugData( eSSRResultFactor, ssrResult.xyz() * ssrResult.www(), 1.0_f );
					auto reflectionResult = writer.declLocale( "reflectionResult"
						, mix( backgroundReflection, ssrResult.xyz(), ssrResult.www() ) );
					displayDebugData( eReflection, reflectionResult, 1.0_f );


					// Wobbly refractions
					auto distortedTexCoord = writer.declLocale( "distortedTexCoord"
						, ( hdrCoords + ( ( finalNormal.xz() + finalNormal.xy() ) * 0.5_f ) * c3d_oceanData.refractionDistortionFactor ) );
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
						, clamp( distance( scenePosition, in.worldPosition.xyz() ) / c3d_oceanData.depthSofteningDistance, 0.0_f, 1.0_f ) );
					displayDebugData( eDepthSoftenedAlpha, vec3( depthSoftenedAlpha ), 1.0_f );
					auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedPosition, scenePosition ) );
					auto waterTransmission = writer.declLocale( "waterTransmission"
						, material.transmission().rgb() * ( indirectAmbient + indirectDiffuse ) * lightDiffuse );
					auto heightFactor = writer.declLocale( "heightFactor"
						, c3d_oceanData.refractionHeightFactor * ( c3d_sceneData.farPlane - c3d_sceneData.nearPlane ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, vec3( clamp( ( in.worldPosition.y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
					displayDebugData( eHeightMixedRefraction, refractionResult, 1.0_f );
					auto distanceFactor = writer.declLocale( "distanceFactor"
						, c3d_oceanData.refractionDistanceFactor * ( c3d_sceneData.farPlane - c3d_sceneData.nearPlane ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, utils.saturate( vec3( utils.saturate( length( in.viewPosition ) / distanceFactor ) ) ) );
					displayDebugData( eDistanceMixedRefraction, refractionResult, 1.0_f );

					// Now apply some foam on top of waves
					auto foamColor = writer.declLocale( "foamColor"
						, c3d_waveFoam.sample( ( normalMapCoords1 + normalMapCoords2 ) * c3d_oceanData.foamTiling ).rgb() );
					auto foamNoise = writer.declLocale( "foamNoise"
						, c3d_waveNoise.sample( in.texture1.xy() * c3d_oceanData.foamTiling ) );
					auto foamAmount = writer.declLocale( "foamAmount"
						, utils.saturate( ( in.worldPosition.w() - c3d_oceanData.foamHeightStart ) / c3d_oceanData.foamFadeDistance ) * pow( utils.saturate( dot( in.normal.xyz(), vec3( 0.0_f, 1.0_f, 0.0_f ) ) ), c3d_oceanData.foamAngleExponent ) * foamNoise );
					foamAmount += pow( ( 1.0_f - depthSoftenedAlpha ), 3.0_f );
					auto foamResult = writer.declLocale( "foamResult"
						, lightDiffuse * mix( vec3( 0.0_f )
							, foamColor * c3d_oceanData.foamBrightness
							, vec3( utils.saturate( foamAmount ) * depthSoftenedAlpha ) ) );


					//Combine all that
					auto fresnelFactor = writer.declLocale( "fresnelFactor"
						, vec3( reflections->computeFresnel( *lightMat
							, surface
							, c3d_sceneData
							, c3d_oceanData.refractionRatio ) ) );
					displayDebugData( eFresnelFactor, vec3( fresnelFactor ), 1.0_f );
					reflectionResult *= fresnelFactor;
					displayDebugData( eFinalReflection, reflectionResult, 1.0_f );
					refractionResult *= vec3( 1.0_f ) - fresnelFactor;
					displayDebugData( eFinalRefraction, refractionResult, 1.0_f );

					pxl_colour = vec4( lightSpecular + lightIndirectSpecular
							+ emissive
							+ refractionResult
							+ ( reflectionResult * indirectAmbient )
							+ foamResult
							+ lightScattering
						, depthSoftenedAlpha );
				}
				ELSE
				{
					pxl_colour = vec4( lightMat->albedo, 1.0_f );
				}
				FI;

				if ( flags.hasFog() )
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

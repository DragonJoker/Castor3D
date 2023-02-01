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
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/Program.hpp>
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include <Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslFog.hpp>
#include <Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureAnimation.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/BillboardUbo.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>
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
	namespace rdpass
	{
		static uint32_t constexpr OutputVertices = 3u;

		template< ast::var::Flag FlagT >
		using PatchT = castor3d::shader::FragmentSurfaceT< FlagT >;

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

		static void bindTexture( VkImageView view
			, VkSampler sampler
			, ashes::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			writes.push_back( castor3d::makeImageViewDescriptorWrite( view, sampler, index++ ) );
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

		static void loadImage( castor3d::Parameters const & params
			, castor::String const & prefix
			, castor::String const & name
			, castor3d::Engine & engine
			, crg::FrameGraph & graph
			, crg::RunnableGraph & runnable
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
					, [format, dim, buffer, &img, &view, &result, &runnable]( castor3d::RenderDevice const & device
						, castor3d::QueueData const & queue )
					{
						auto staging = device->createStagingTexture( VkFormat( format )
							, VkExtent2D{ dim.getWidth(), dim.getHeight() }
							, buffer->getLevels() );
						ashes::ImagePtr noiseImg = std::make_unique< ashes::Image >( *device
							, runnable.createImage( img )
							, img.data->info );
						result = ashes::ImageView{ ashes::ImageViewCreateInfo{ *noiseImg, view.data->info }
							, runnable.createImageView( view )
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
		, crg::ImageViewIdArray targetImage
		, crg::ImageViewIdArray targetDepth
		, castor3d::RenderNodesPassDesc const & renderPassDesc
		, castor3d::RenderTechniquePassDesc const & techniquePassDesc
		, std::shared_ptr< castor3d::IsRenderPassEnabled > isEnabled )
		: castor3d::RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, std::move( targetImage )
			, std::move( targetDepth )
			, renderPassDesc
			, techniquePassDesc }
		, m_isEnabled{ std::move( isEnabled ) }
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
		rdpass::loadImage( params, Foam, getName(), *getEngine(), pass.graph, graph, loader, m_foamImg, m_foamView, m_foam );
		rdpass::loadImage( params, Noise, getName(), *getEngine(), pass.graph, graph, loader, m_noiseImg, m_noiseView, m_noise );
		rdpass::loadImage( params, Normals1, getName(), *getEngine(), pass.graph, graph, loader, m_normals1Img, m_normals1View, m_normals1 );
		rdpass::loadImage( params, Normals2, getName(), *getEngine(), pass.graph, graph, loader, m_normals2Img, m_normals2View, m_normals2 );
	}

	OceanRenderPass::~OceanRenderPass()
	{
	}

	crg::FramePassArray OceanRenderPass::create( castor3d::RenderDevice const & device
		, castor3d::RenderTechnique & technique
		, castor3d::TechniquePasses & renderPasses
		, crg::FramePassArray previousPasses )
	{
		std::string name{ Name };
		auto isEnabled = std::make_shared< castor3d::IsRenderPassEnabled >();
		auto extent = technique.getTargetExtent();
		auto & graph = technique.getRenderTarget().getGraph().createPassGroup( name );

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
		blitColourPass.addTransferInputView( technique.getSampledResult() );
		blitColourPass.addTransferOutputView( technique.getSampledIntermediate() );

		auto targetResult = technique.getTargetResult();
		auto targetDepth = technique.getTargetDepth();
		auto & result = graph.createPass( "NodesPass"
			, [extent, targetResult, targetDepth, isEnabled, &device, &technique, &renderPasses]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
		{
			auto res = std::make_unique< OceanRenderPass >( &technique
				, framePass
				, context
				, runnableGraph
				, device
				, targetResult
				, targetDepth
				, castor3d::RenderNodesPassDesc{ extent
					, technique.getCameraUbo()
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
		result.addDependency( technique.getGetLastDepthPass() );
		result.addDependency( technique.getGetLastOpaquePass() );
		result.addImplicitColourView( technique.getSampledIntermediate()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addImplicitColourView( technique.getDepthObj().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addImplicitColourView( technique.getNormal().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addInOutDepthStencilView( technique.getTargetDepth() );
		result.addInOutColourView( technique.getTargetResult() );
		return { &result };
	}

	castor3d::ComponentModeFlags OceanRenderPass::getComponentsMask()const
	{
		return castor3d::ComponentModeFlag::eColour
			| castor3d::ComponentModeFlag::eNormals
			| castor3d::ComponentModeFlag::eGeometry
			| castor3d::ComponentModeFlag::eOcclusion
			| castor3d::ComponentModeFlag::eDiffuseLighting
			| castor3d::ComponentModeFlag::eSpecularLighting
			| castor3d::ComponentModeFlag::eSpecifics;
	}

	castor3d::ShaderFlags OceanRenderPass::getShaderFlags()const
	{
		return castor3d::ShaderFlag::eWorldSpace
			| castor3d::ShaderFlag::eViewSpace
			| castor3d::ShaderFlag::eNormal
			| castor3d::ShaderFlag::eTangentSpace
			| castor3d::ShaderFlag::eLighting
			| castor3d::ShaderFlag::eTessellation
			| castor3d::ShaderFlag::eForceTexCoords
			| castor3d::ShaderFlag::eColour;
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

	void OceanRenderPass::doFillAdditionalBindings( castor3d::PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		bindings.emplace_back( getCuller().getScene().getLightCache().createLayoutBinding( rdpass::WaveIdx::eLightBuffer ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eWavesUbo
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_VERTEX_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_FRAGMENT_BIT ) ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eWaveFoam
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eWaveNormals1
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eWaveNormals2
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eWaveNoise
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eSceneNormals
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eSceneDepth
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eSceneResult
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::WaveIdx::eBrdf
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto index = uint32_t( rdpass::WaveIdx::eBrdf ) + 1u;
		doAddShadowBindings( flags, bindings, index );
		doAddEnvBindings( flags, bindings, index );
		doAddBackgroundBindings( flags, bindings, index );
		doAddGIBindings( flags, bindings, index );
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

	void OceanRenderPass::doFillAdditionalDescriptor( castor3d::PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, castor3d::ShadowMapLightTypeArray const & shadowMaps )
	{
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( rdpass::WaveIdx::eLightBuffer ) );
		descriptorWrites.push_back( m_ubo.getDescriptorWrite( rdpass::WaveIdx::eWavesUbo ) );
		auto index = uint32_t( rdpass::WaveIdx::eWaveFoam );
		rdpass::bindTexture( m_foam, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_normals1, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_normals2, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_noise, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_parent->getNormal().sampledView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( getTechnique().getDepthObj().wholeView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( getTechnique().getIntermediate().wholeView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampledView
			, *getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampler
			, descriptorWrites
			, index );
		doAddShadowDescriptor( flags, descriptorWrites, shadowMaps, index );
		doAddEnvDescriptor( flags, descriptorWrites, index );
		doAddBackgroundDescriptor( flags, descriptorWrites, m_targetImage, index );
		doAddGIDescriptor( flags, descriptorWrites, index );
	}

	castor3d::SubmeshFlags OceanRenderPass::doAdjustSubmeshFlags( castor3d::SubmeshFlags flags )const
	{
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords2 );
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords3 );
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
		flags.patchVertices = rdpass::OutputVertices;
#endif
	}

	castor3d::ShaderPtr OceanRenderPass::doGetVertexShaderSource( castor3d::PipelineFlags const & flags )const
	{
		using namespace sdw;
		using namespace castor3d;
		VertexWriter writer;

		castor3d::shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< castor3d::shader::VertexSurfaceT, rdpass::PatchT >( sdw::VertexInT< castor3d::shader::VertexSurfaceT >{ writer
				, flags }
			, sdw::VertexOutT< rdpass::PatchT >{ writer
				, passShaders
				, flags }
			, [&]( VertexInT< castor3d::shader::VertexSurfaceT > in
				, VertexOutT< rdpass::PatchT > out )
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
			out.nodeId = writer.cast< sdw::Int >( nodeId );

#if Ocean_DebugPixelShader
			auto modelData = writer.declLocale( "modelData"
				, c3d_modelsData[nodeId - 1u] );
			auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
				, c3d_modelData.getCurModelMtx( flags.programFlags
					, skinningData
					, in.boneIds0
					, in.boneIds1
					, in.boneWeights0
					, in.boneWeights1
					, in.transform ) );
			out.worldPosition = curMtxModel * out.vtx.position;
			out.viewPosition = c3d_cameraData.worldToCurView( out.worldPosition );
			out.vtx.position = c3d_cameraData.viewToProj( out.viewPosition );
			auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
				, c3d_modelData.getNormalMtx( flags.programFlags, curMtxModel ) );
			out.computeTangentSpace( flags.submeshFlags
				, flags.programFlags
				, c3d_cameraData.position()
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

		castor3d::shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		// Shader inputs
		auto position = writer.declInput< Vec4 >( "position", 0u );
		auto uv = writer.declInput< Vec2 >( "uv", 1u );
		auto center = writer.declInput< Vec3 >( "center", 2u );

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
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

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< VoidT, rdpass::PatchT >( sdw::VertexInT< sdw::VoidT >{ writer }
			, sdw::VertexOutT< rdpass::PatchT >{ writer
				, passShaders
				, flags }
			, [&]( VertexInT< VoidT > in
				, VertexOutT< rdpass::PatchT > out )
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
					, c3d_cameraData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId - 1u] );
				auto right = writer.declLocale( "right"
					, billboardData.getCameraRight( c3d_cameraData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( c3d_cameraData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( c3d_cameraData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( c3d_cameraData ) );
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

		castor3d::shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		auto index = uint32_t( GlobalBuffersIdx::eCount );
		index++; // lights buffer
		C3D_Ocean( writer
			, index
			, RenderPipeline::eBuffers );

		writer.implementPatchRoutineT< rdpass::PatchT, rdpass::OutputVertices, VoidT >( TessControlListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
				, false
				, passShaders
				, flags }
			, TrianglesTessPatchOutT< VoidT >{ writer, 9u }
			, [&]( sdw::TessControlPatchRoutineIn in
				, sdw::TessControlListInT< rdpass::PatchT, rdpass::OutputVertices > listIn
				, sdw::TrianglesTessPatchOut patchOut )
			{
				patchOut.tessLevelOuter[0] = c3d_oceanData.tessellationFactor();
				patchOut.tessLevelOuter[1] = c3d_oceanData.tessellationFactor();
				patchOut.tessLevelOuter[2] = c3d_oceanData.tessellationFactor();

				patchOut.tessLevelInner[0] = c3d_oceanData.tessellationFactor();
			} );

		writer.implementMainT< rdpass::PatchT, rdpass::OutputVertices, rdpass::PatchT >( TessControlListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
				, true
				, passShaders
				, flags }
			, TrianglesTessControlListOutT< rdpass::PatchT >{ writer
				, ast::type::Partitioning::eFractionalEven
				, ast::type::OutputTopology::eTriangle
				, ast::type::PrimitiveOrdering::eCCW
				, rdpass::OutputVertices
				, passShaders
				, flags }
			, [&]( TessControlMainIn in
				, TessControlListInT< rdpass::PatchT, rdpass::OutputVertices > listIn
				, TrianglesTessControlListOutT< rdpass::PatchT > listOut )
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
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		index++; // lights buffer
		C3D_Ocean( writer
			, index
			, RenderPipeline::eBuffers );

		auto calculateWave = writer.implementFunction< rdpass::WaveResult >( "calculateWave"
			, [&]( Wave wave
				, Vec3 wavePosition
				, Float edgeDampen
				, UInt numWaves
				, Float timeIndex )
			{
				auto result = writer.declLocale< rdpass::WaveResult >( "result" );

				auto frequency = writer.declLocale( "frequency"
					, 2.0_f / wave.length() );
				auto phaseConstant = writer.declLocale( "phaseConstant"
					, wave.speed() * frequency );
				auto qi = writer.declLocale( "qi"
					, wave.steepness() / ( wave.amplitude() * frequency * writer.cast< sdw::Float >( numWaves ) ) );
				auto rad = writer.declLocale( "rad"
					, frequency * dot( wave.direction().xz(), wavePosition.xz() ) + timeIndex * phaseConstant );
				auto sinR = writer.declLocale( "sinR"
					, sin( rad ) );
				auto cosR = writer.declLocale( "cosR"
					, cos( rad ) );

				result.position.x() = wavePosition.x() + qi * wave.amplitude() * wave.direction().x() * cosR * edgeDampen;
				result.position.y() = wave.amplitude() * sinR * edgeDampen;
				result.position.z() = wavePosition.z() + qi * wave.amplitude() * wave.direction().z() * cosR * edgeDampen;

				auto waFactor = writer.declLocale( "waFactor"
					, frequency * wave.amplitude() );
				auto radN = writer.declLocale( "radN"
					, frequency * dot( wave.direction(), result.position ) + timeIndex * phaseConstant );
				auto sinN = writer.declLocale( "sinN"
					, sin( radN ) );
				auto cosN = writer.declLocale( "cosN"
					, cos( radN ) );

				result.bitangent.x() = 1.0_f - ( qi * wave.direction().x() * wave.direction().x() * waFactor * sinN );
				result.bitangent.y() = wave.direction().x() * waFactor * cosN;
				result.bitangent.z() = -1.0_f * ( qi * wave.direction().x() * wave.direction().z() * waFactor * sinN );

				result.tangent.x() = -1.0_f * ( qi * wave.direction().x() * wave.direction().z() * waFactor * sinN );
				result.tangent.y() = wave.direction().z() * waFactor * cosN;
				result.tangent.z() = 1.0_f - ( qi * wave.direction().z() * wave.direction().z() * waFactor * sinN );

				result.normal.x() = -1.0_f * ( wave.direction().x() * waFactor * cosN );
				result.normal.y() = 1.0_f - ( qi * waFactor * sinN );
				result.normal.z() = -1.0_f * ( wave.direction().z() * waFactor * cosN );

				result.bitangent = normalize( result.bitangent );
				result.tangent = normalize( result.tangent );
				result.normal = normalize( result.normal );

				writer.returnStmt( result );
			}
			, InWave{ writer, "wave" }
			, sdw::InVec3{ writer, "wavePosition" }
			, sdw::InFloat{ writer, "edgeDampen" }
			, sdw::InUInt{ writer, "numWaves" }
			, sdw::InFloat{ writer, "timeIndex" } );

		writer.implementMainT< rdpass::PatchT, rdpass::OutputVertices, VoidT, castor3d::shader::FragmentSurfaceT >( TessEvalListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
				, ast::type::PatchDomain::eTriangles
				, type::Partitioning::eFractionalEven
				, type::PrimitiveOrdering::eCCW
				, passShaders
				, flags }
			, TrianglesTessPatchInT< VoidT >{ writer, 9u }
			, TessEvalDataOutT< castor3d::shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, [&]( TessEvalMainIn mainIn
				, TessEvalListInT< rdpass::PatchT, rdpass::OutputVertices > listIn
				, TrianglesTessPatchInT< VoidT > patchIn
				, TessEvalDataOutT< castor3d::shader::FragmentSurfaceT > out )
			{
				auto position = writer.declLocale( "position"
					, patchIn.tessCoord.x() * listIn[0].vtx.position
					+ patchIn.tessCoord.y() * listIn[1].vtx.position
					+ patchIn.tessCoord.z() * listIn[2].vtx.position );
				auto texcoord = writer.declLocale( "texcoord"
					, patchIn.tessCoord.x() * listIn[0].texture0
					+ patchIn.tessCoord.y() * listIn[1].texture0
					+ patchIn.tessCoord.z() * listIn[2].texture0 );
				auto nodeId = writer.declLocale( "nodeId"
					, listIn[0].nodeId );

				auto dampening = writer.declLocale( "dampening"
					, 1.0_f - pow( utils.saturate( abs( texcoord.x() - 0.5_f ) / 0.5_f ), c3d_oceanData.dampeningFactor() ) );
				dampening *= 1.0_f - pow( utils.saturate( abs( texcoord.y() - 0.5_f ) / 0.5_f ), c3d_oceanData.dampeningFactor() );

				auto finalWaveResult = writer.declLocale< rdpass::WaveResult >( "finalWaveResult" );
				finalWaveResult.position = vec3( 0.0_f );
				finalWaveResult.normal = vec3( 0.0_f );
				finalWaveResult.tangent = vec3( 0.0_f );
				finalWaveResult.bitangent = vec3( 0.0_f );

				auto numWaves = writer.declLocale( "numWaves"
					, c3d_oceanData.numWaves() );

				IF( writer, numWaves > 0_u )
				{
					FOR( writer, UInt, waveId, 0_u, waveId < numWaves, waveId++ )
					{
						auto waveResult = writer.declLocale( "waveResult"
							, calculateWave( c3d_oceanData.waves()[waveId]
								, position.xyz()
								, dampening
								, c3d_oceanData.numWaves()
								, c3d_oceanData.time() ) );
						finalWaveResult.position += waveResult.position;
						finalWaveResult.normal += waveResult.normal;
						finalWaveResult.tangent += waveResult.tangent;
						finalWaveResult.bitangent += waveResult.bitangent;
					}
					ROF;

					finalWaveResult.position -= position.xyz() * ( writer.cast< sdw::Float >( c3d_oceanData.numWaves() - 1_u ) );
					finalWaveResult.normal = normalize( finalWaveResult.normal );
					finalWaveResult.tangent = normalize( finalWaveResult.tangent );
					finalWaveResult.bitangent = normalize( finalWaveResult.bitangent );
				}
				ELSE
				{
					finalWaveResult.position = position.xyz();
					finalWaveResult.normal = vec3( 0.0_f, 1.0_f, 0.0_f );
					finalWaveResult.tangent = vec3( 1.0_f, 0.0_f, 0.0_f );
					finalWaveResult.bitangent = vec3( 0.0_f, 0.0_f, 1.0_f );
				}
				FI;

				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
				auto height = writer.declLocale( "height"
					, finalWaveResult.position.y() - position.y() );
				auto mtxModel = writer.declLocale( "mtxModel"
					, modelData.getModelMtx() );
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, modelData.getNormalMtx( flags, mtxModel ) );

				out.texture1 = patchIn.tessCoord.x() * listIn[0].texture1
					+ patchIn.tessCoord.y() * listIn[1].texture1
					+ patchIn.tessCoord.z() * listIn[2].texture1;
				out.colour = patchIn.tessCoord.x() * listIn[0].colour
					+ patchIn.tessCoord.y() * listIn[1].colour
					+ patchIn.tessCoord.z() * listIn[2].colour;
				out.texture0 = texcoord;
				out.nodeId = nodeId;
				out.normal = normalize( mtxNormal * finalWaveResult.normal );
				out.tangent = normalize( mtxNormal * finalWaveResult.tangent );
				out.bitangent = normalize( mtxNormal * finalWaveResult.bitangent );
				out.worldPosition = mtxModel * vec4( finalWaveResult.position, 1.0_f );
				out.viewPosition = c3d_cameraData.worldToCurView( out.worldPosition );
				out.worldPosition.w() = height;
				out.vtx.position = c3d_cameraData.viewToProj( out.viewPosition );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
#endif
	}

	castor3d::ShaderPtr OceanRenderPass::doGetPixelShaderSource( castor3d::PipelineFlags const & flags )const
	{
#if Ocean_Debug
#	define displayDebugData( OceanDataType, RGB, A )\
	IF( writer, c3d_oceanData.debug().x() == sdw::UInt( OceanDataType ) )\
	{\
		outColour = vec4( RGB, A );\
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
		shader::BRDFHelpers brdf{ writer };
		shader::CookTorranceBRDF cookTorrance{ writer, brdf };
		shader::Fog fog{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
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
		auto c3d_depthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_depthObj"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_colour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_colour"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, RenderPipeline::eBuffers );
		shader::Lights lights{ *getEngine()
			, flags.lightingModelId
			, flags.backgroundModelId
			, materials
			, brdf
			, utils
			, shader::ShadowOptions{ flags.getShadowFlags(), true, false }
			, nullptr
			, lightsIndex /* lightBinding */
			, RenderPipeline::eBuffers /* lightSet */
			, index /* shadowMapBinding */
			, RenderPipeline::eBuffers /* shadowMapSet */
			, false /* enableVolumetric */ };
		shader::ReflectionModel reflections{ writer
			, utils
			, index
			, uint32_t( RenderPipeline::eBuffers )
			, lights.hasIblSupport() };
		auto backgroundModel = shader::BackgroundModel::createModel( getScene()
			, writer
			, utils
			, castor3d::makeExtent2D( m_size )
			, true
			, index
			, RenderPipeline::eBuffers );
		shader::GlobalIllumination indirect{ writer, utils };
		indirect.declare( index
			, RenderPipeline::eBuffers
			, flags.getGlobalIlluminationFlags() );

		// Fragment Outputs
		auto outColour( writer.declOutput< Vec4 >( "outColour", 0 ) );

		writer.implementMainT< castor3d::shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< castor3d::shader::FragmentSurfaceT >{ writer
				, passShaders
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
					, in.texture1.xy() + c3d_oceanData.time() * c3d_oceanData.normalMapScroll().xy() * c3d_oceanData.normalMapScrollSpeed().x() );
				auto normalMapCoords2 = writer.declLocale( "normalMapCoords2"
					, in.texture1.xy() + c3d_oceanData.time() * c3d_oceanData.normalMapScroll().zw() * c3d_oceanData.normalMapScrollSpeed().y() );
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
				auto surface = writer.declLocale( "surface"
					, shader::Surface{ in.fragCoord.xyz()
						, in.viewPosition.xyz()
						, in.worldPosition.xyz()
						, finalNormal } );
				auto components = writer.declLocale( "components"
					, shader::BlendComponents{ materials
						, material
						, surface } );
				displayDebugData( eMatSpecular, components.specular, 1.0_f );

				if ( auto lightingModel = lights.getLightingModel() )
				{
					// Direct Lighting
					shader::OutputComponents output{ writer, false };
					auto rastComponents = writer.declLocale( "rastComponents"
						, shader::BlendComponents{ components, materials } );
					auto lightSurface = shader::LightSurface::create( writer
						, "lightSurface"
						, c3d_cameraData.position()
						, surface.worldPosition.xyz()
						, surface.viewPosition.xyz()
						, surface.clipPosition
						, surface.normal );
					lights.computeCombinedDifSpec( rastComponents
						, *backgroundModel
						, lightSurface
						, modelData.isShadowReceiver()
						, output );
					lightingModel->adjustDirectSpecular( components
						, output.m_specular );
					displayDebugData( eLightDiffuse, output.m_diffuse, 1.0_f );
					displayDebugData( eLightSpecular, output.m_specular, 1.0_f );
					displayDebugData( eLightScattering, output.m_scattering, 1.0_f );
					// Standard lighting models don't necessarily translate all that well to water.
					// It can end up looking very glossy and plastic-like, having much more form than it really should.
					// So here, I'm sampling some noise with three different sets of texture coordinates to try and achieve
					// that sparkling look that defines water specularity.
					auto specularNoise = writer.declLocale( "specularNoise"
						, c3d_waveNoise.sample( normalMapCoords1 * 0.5_f ) );
					specularNoise *= c3d_waveNoise.sample( normalMapCoords2 * 0.5_f );
					specularNoise *= c3d_waveNoise.sample( in.texture1.xy() * 0.5_f );
					output.m_specular *= specularNoise;
					displayDebugData( eNoisedSpecular, output.m_specular, 1.0_f );
					displayDebugData( eSpecularNoise, vec3( specularNoise ), 1.0_f );


					// Indirect Lighting
					lightSurface.updateL( utils
						, components.normal
						, components.specular
						, components );
					auto indirectOcclusion = indirect.computeOcclusion( flags.getGlobalIlluminationFlags()
						, lightSurface );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.getGlobalIlluminationFlags()
						, lightSurface
						, indirectOcclusion );
					displayDebugData( eIndirectOcclusion, vec3( indirectOcclusion ), 1.0_f );
					displayDebugData( eLightIndirectDiffuse, lightIndirectDiffuse.xyz(), 1.0_f );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.getGlobalIlluminationFlags()
						, lightSurface
						, components.roughness
						, indirectOcclusion
						, lightIndirectDiffuse.w()
						, c3d_mapBrdf );
					displayDebugData( eLightIndirectSpecular, lightIndirectSpecular, 1.0_f );
					auto indirectAmbient = writer.declLocale( "indirectAmbient"
						, indirect.computeAmbient( flags.getGlobalIlluminationFlags(), lightIndirectDiffuse.xyz() ) );
					displayDebugData( eIndirectAmbient, indirectAmbient, 1.0_f );
					auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
						, ( hasDiffuseGI
							? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
								, length( lightIndirectDiffuse.xyz() )
								, lightSurface.difF()
								, components.metalness )
							: vec3( 0.0_f ) ) );
					displayDebugData( eIndirectDiffuse, indirectDiffuse, 1.0_f );


					// Reflection
					auto bgDiffuseReflection = writer.declLocale( "bgDiffuseReflection"
						, vec3( 0.0_f ) );
					auto bgSpecularReflection = writer.declLocale( "bgSpecularReflection"
						, vec3( 0.0_f ) );
					lightSurface.updateN( utils
						, components.normal
						, components.specular
						, components );
					reflections.computeReflections( components
						, lightSurface
						, *backgroundModel
						, modelData.getEnvMapIndex()
						, components.hasReflection
						, bgDiffuseReflection
						, bgSpecularReflection );
					auto backgroundReflection = writer.declLocale( "backgroundReflection"
						, bgDiffuseReflection + bgSpecularReflection );
					auto ssrResult = writer.declLocale( "ssrResult"
						, reflections.computeScreenSpace( c3d_cameraData
							, lightSurface.viewPosition()
							, finalNormal
							, hdrCoords
							, vec4( c3d_oceanData.ssrStepSize()
								, c3d_oceanData.ssrForwardStepsCount()
								, c3d_oceanData.ssrBackwardStepsCount()
								, c3d_oceanData.ssrDepthMult() )
							, c3d_depthObj
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
						, ( hdrCoords + ( ( finalNormal.xz() + finalNormal.xy() ) * 0.5_f ) * c3d_oceanData.refractionDistortionFactor() ) );
					auto distortedDepth = writer.declLocale( "distortedDepth"
						, c3d_depthObj.sample( distortedTexCoord ).r() );
					auto distortedPosition = writer.declLocale( "distortedPosition"
						, c3d_cameraData.curProjToWorld( utils, distortedTexCoord, distortedDepth ) );
					auto refractionTexCoord = writer.declLocale( "refractionTexCoord"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedTexCoord, hdrCoords ) );
					auto refractionResult = writer.declLocale( "refractionResult"
						, c3d_colour.sample( refractionTexCoord ).rgb() * components.colour );
					displayDebugData( eRefraction, refractionResult, 1.0_f );
					//  Retrieve non distorted scene colour.
					auto sceneDepth = writer.declLocale( "sceneDepth"
						, c3d_depthObj.sample( hdrCoords ).r() );
					auto scenePosition = writer.declLocale( "scenePosition"
						, c3d_cameraData.curProjToWorld( utils, hdrCoords, sceneDepth ) );
					// Depth softening, to fade the alpha of the water where it meets the scene geometry by some predetermined distance. 
					auto depthSoftenedAlpha = writer.declLocale( "depthSoftenedAlpha"
						, clamp( distance( scenePosition, in.worldPosition.xyz() ) / c3d_oceanData.depthSofteningDistance(), 0.0_f, 1.0_f ) );
					displayDebugData( eDepthSoftenedAlpha, vec3( depthSoftenedAlpha ), 1.0_f );
					auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedPosition, scenePosition ) );
					auto waterTransmission = writer.declLocale( "waterTransmission"
						, components.colour * ( indirectAmbient + indirectDiffuse ) * output.m_diffuse );
					auto heightFactor = writer.declLocale( "heightFactor"
						, c3d_oceanData.refractionHeightFactor() * ( c3d_cameraData.farPlane() - c3d_cameraData.nearPlane() ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, vec3( clamp( ( in.worldPosition.y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
					displayDebugData( eHeightMixedRefraction, refractionResult, 1.0_f );
					auto distanceFactor = writer.declLocale( "distanceFactor"
						, c3d_oceanData.refractionDistanceFactor() * ( c3d_cameraData.farPlane() - c3d_cameraData.nearPlane() ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, utils.saturate( vec3( utils.saturate( length( in.viewPosition ) / distanceFactor ) ) ) );
					displayDebugData( eDistanceMixedRefraction, refractionResult, 1.0_f );

					// Now apply some foam on top of waves
					auto foamColor = writer.declLocale( "foamColor"
						, c3d_waveFoam.sample( ( normalMapCoords1 + normalMapCoords2 ) * c3d_oceanData.foamTiling() ).rgb() );
					auto foamNoise = writer.declLocale( "foamNoise"
						, c3d_waveNoise.sample( in.texture1.xy() * c3d_oceanData.foamTiling() ) );
					auto foamAmount = writer.declLocale( "foamAmount"
						, utils.saturate( ( in.worldPosition.w() - c3d_oceanData.foamHeightStart() ) / c3d_oceanData.foamFadeDistance() ) * pow( utils.saturate( dot( in.normal.xyz(), vec3( 0.0_f, 1.0_f, 0.0_f ) ) ), c3d_oceanData.foamAngleExponent() ) * foamNoise );
					foamAmount += pow( ( 1.0_f - depthSoftenedAlpha ), 3.0_f );
					auto foamResult = writer.declLocale( "foamResult"
						, output.m_diffuse * mix( vec3( 0.0_f )
							, foamColor * c3d_oceanData.foamBrightness()
							, vec3( utils.saturate( foamAmount ) * depthSoftenedAlpha ) ) );


					//Combine all that
					auto fresnelFactor = writer.declLocale( "fresnelFactor"
						, vec3( utils.fresnelMix( reflections.computeIncident( lightSurface.worldPosition(), c3d_cameraData.position() )
							, components.normal
							, components.roughness
							, c3d_oceanData.refractionRatio() ) ) );
					displayDebugData( eFresnelFactor, vec3( fresnelFactor ), 1.0_f );
					reflectionResult *= fresnelFactor;
					displayDebugData( eFinalReflection, reflectionResult, 1.0_f );
					refractionResult *= vec3( 1.0_f ) - fresnelFactor;
					displayDebugData( eFinalRefraction, refractionResult, 1.0_f );

					outColour = vec4( output.m_specular + lightIndirectSpecular
							+ components.emissiveColour * components.emissiveFactor
							+ refractionResult
							+ ( reflectionResult * indirectAmbient )
							+ foamResult
							+ output.m_scattering
						, depthSoftenedAlpha );
				}
				else
				{
					outColour = vec4( components.colour, 1.0_f );
				}

				if ( flags.hasFog() )
				{
					outColour = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_cameraData.gamma() )
						, outColour
						, in.worldPosition.xyz()
						, c3d_cameraData.position()
						, c3d_sceneData );
				}

				backgroundModel->applyVolume( in.fragCoord.xy()
					, utils.lineariseDepth( in.fragCoord.z(), c3d_cameraData.nearPlane(), c3d_cameraData.farPlane() )
					, vec2( c3d_cameraData.renderSize() )
					, c3d_cameraData.depthPlanes()
					, outColour );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}

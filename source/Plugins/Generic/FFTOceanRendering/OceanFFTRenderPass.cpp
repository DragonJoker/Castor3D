#include "FFTOceanRendering/OceanFFTRenderPass.hpp"

#include "FFTOceanRendering/OceanFFT.hpp"

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
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Scene/Camera.hpp>
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
#include <Castor3D/Shader/Ubos/MorphingUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

#include <ashespp/Image/StagingTexture.hpp>

#define Ocean_DebugFFTGraph 0

namespace ocean_fft
{
	namespace rdpass
	{
#if Ocean_DebugFFTGraph
		class DummyRunnable
			: public crg::RunnablePass
		{
		public:
			DummyRunnable( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
				: crg::RunnablePass{ pass
					, context
					, graph
					, { crg::RunnablePass::InitialiseCallback( [this](){ doInitialise(); } )
						, crg::RunnablePass::GetSemaphoreWaitFlagsCallback( [this](){ return doGetSemaphoreWaitFlags(); } ) } }
			{
			}

		private:
			void doInitialise()
			{
			}

			VkPipelineStageFlags doGetSemaphoreWaitFlags()const
			{
				return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			}
		};

		void createFakeCombinePass( castor::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, castor3d::Texture const & a
			, castor3d::Texture const & b
			, castor3d::Texture const & c )
		{
			auto & result = graph.createPass( "FakeCombine"
				, []( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
			{
				return std::make_unique< DummyRunnable >( framePass
					, context
					, runnableGraph );
			} );

			result.addDependencies( previousPasses );
			result.addImplicitColourView( a.sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( b.sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( c.sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}
#endif

		template< ast::var::Flag FlagT >
		struct PatchT
			: public sdw::StructInstance
		{
			PatchT( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstance{ writer, std::move( expr ), enabled }
				, patchWorldPosition{ getMember< sdw::Vec3 >( "patchWorldPosition" ) }
				, patchLods{ getMember< sdw::Vec4 >( "patchLodsGradNormTex" ) }
				, colour{ getMember< sdw::Vec3 >( "colour" ) }
				, nodeId{ getMember< sdw::Int >( "nodeId" ) }
				, gradNormalTexcoord{ patchLods }
			{
			}

			SDW_DeclStructInstance( , PatchT );

			static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
				, castor3d::PipelineFlags flags )
			{
				auto result = cache.getIOStruct( ast::type::MemoryLayout::eC
					, "C3DORFFT_" + ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Output" }
						: std::string{ "Input" } ) + "Patch"
					, FlagT );

				if ( result->empty() )
				{
					uint32_t index = 0u;
					result->declMember( "patchWorldPosition"
						, ast::type::Kind::eVec3F
						, ast::type::NotArray
						, index++ );
					result->declMember( "patchLodsGradNormTex"
						, ast::type::Kind::eVec4F
						, ast::type::NotArray
						, index++ );
					result->declMember( "colour"
						, ast::type::Kind::eVec3F
						, ast::type::NotArray
						, ( flags.enableColours() ? index++ : 0 )
						, flags.enableColours() );
					result->declMember( "nodeId"
						, ast::type::Kind::eInt
						, ast::type::NotArray
						, index++ );
				}

				return result;
			}

			sdw::Vec3 patchWorldPosition;
			sdw::Vec4 patchLods;
			sdw::Vec3 colour;
			sdw::Int nodeId;
			sdw::Vec4 gradNormalTexcoord;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};

		enum OceanFFTIdx : uint32_t
		{
			eLightBuffer = uint32_t( castor3d::GlobalBuffersIdx::eCount ),
			eOceanUbo,
			eHeightDisplacement,
			eGradientJacobian,
			eNormals,
			eSceneNormals,
			eSceneDepth,
			eSceneResult,
			eSceneBaseColour,
			eSceneDiffuseLighting,
			eBrdf,
			eCount,
		};

		static void bindTexture( VkImageView view
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

		static crg::FramePass const & createCopyPass( castor::String const & name
			, castor3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, crg::ImageViewId input
			, crg::ImageViewId output
			, std::shared_ptr< IsRenderPassEnabled > isEnabled )
		{
			auto extent = getExtent( input );
			auto & result = graph.createPass( name + "Copy"
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
			result.addDependencies( previousPasses );
			result.addTransferInputView( input );
			result.addTransferOutputView( output );
			return result;
		}

		static crg::FramePassArray createNodesPass( castor3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, castor3d::RenderTechnique & technique
			, castor3d::TechniquePasses & renderPasses
			, crg::FramePassArray previousPasses
			, std::shared_ptr< OceanUbo > oceanUbo
			, std::shared_ptr< OceanFFT > oceanFFT
			, castor3d::TexturePtr colourInput
			, castor3d::TexturePtr depthInput
			, std::shared_ptr< IsRenderPassEnabled > isEnabled )
		{
			auto extent = getExtent( technique.getResultImg() );
			auto & result = graph.createPass( "NodesPass"
				, [extent, colourInput, depthInput, oceanUbo, oceanFFT, isEnabled, &device, &technique, &renderPasses]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
			{
					auto res = std::make_unique< OceanRenderPass >( &technique
						, framePass
						, context
						, runnableGraph
						, device
						, std::move( oceanUbo )
						, std::move( oceanFFT )
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
					renderPasses[size_t( OceanRenderPass::Event )].push_back( res.get() );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, res->getTimer() );
					return res;
			} );

			result.addDependencies( previousPasses );
#if Ocean_DebugFFTGraph
			result.addImplicitColourView( colourInput->sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitDepthView( depthInput->sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( technique.getBaseColourResult().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitDepthView( technique.getDiffuseLightingResult().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addInOutDepthStencilView( technique.getDepthTargetView() );
			result.addInOutColourView( technique.getResultTargetView() );
#else
			result.addDependencies( oceanFFT->getLastPasses() );
			result.addImplicitColourView( colourInput->sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitDepthView( depthInput->sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( technique.getBaseColourResult().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitDepthView( technique.getDiffuseLightingResult().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( oceanFFT->getNormals().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( oceanFFT->getHeightDisplacement().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( oceanFFT->getGradientJacobian().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addInOutDepthStencilView( technique.getDepthTargetView() );
			result.addInOutColourView( technique.getResultTargetView() );
#endif
			return { &result };
		}

		static uint32_t constexpr OutputVertices = 1u;
	}

	castor::String const OceanRenderPass::Type = cuT( "c3d.fft_ocean" );
	castor::String const OceanRenderPass::FullName = cuT( "FFT Ocean rendering" );
	castor::String const OceanRenderPass::Param = cuT( "OceanConfig" );
	castor::String const OceanRenderPass::ParamFFT = cuT( "OceanFFTConfig" );

	OceanRenderPass::OceanRenderPass( castor3d::RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, std::shared_ptr< OceanUbo > oceanUbo
		, std::shared_ptr< OceanFFT > oceanFFT
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
		, m_isEnabled{ isEnabled }
		, m_ubo{ oceanUbo }
		, m_oceanFFT{ std::move( oceanFFT ) }
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
	{
		m_isEnabled->setPass( *this );
		auto params = getEngine()->getRenderPassTypeConfiguration( Type );

		if ( params.get( Param, m_configuration ) )
		{
			auto & data = m_ubo->getUbo().getData();
			data = m_configuration;
		}
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
		auto isEnabled = std::make_shared< IsRenderPassEnabled >();
		auto & graph = technique.getRenderTarget().getGraph().createPassGroup( OceanFFT::Name );
		auto extent = getExtent( technique.getResultImg() );
		auto colourInput = std::make_shared< castor3d::Texture >( device
			, graph.getHandler()
			, OceanFFT::Name + "/Colour"
			, 0u
			, extent
			, 1u
			, 1u
			, technique.getResultImg().data->info.format
			, ( VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT ) );
		colourInput->create();
		crg::FramePassArray passes;
		passes.push_back( &rdpass::createCopyPass( "Colour"
			, device
			, graph
			, previousPasses
			, technique.getResultImgView()
			, colourInput->sampledViewId
			, isEnabled ) );

		auto depthInput = std::make_shared< castor3d::Texture >( device
			, graph.getHandler()
			, OceanFFT::Name + "Depth"
			, 0u
			, extent
			, 1u
			, 1u
			, technique.getDepthImg().data->info.format
			, ( VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT ) );
		depthInput->create();
		passes.push_back( &rdpass::createCopyPass( "Depth"
			, device
			, graph
			, previousPasses
			, technique.getDepthWholeView()
			, depthInput->wholeViewId
			, isEnabled ) );

		auto oceanUbo = std::make_shared< OceanUbo >( device );
#if Ocean_DebugFFTGraph
		crg::FrameGraph fftGraph{ graph.getHandler(), OceanFFT::Name };
		auto & group = fftGraph.createPassGroup( OceanFFT::Name );
		auto oceanFFT = std::make_shared< OceanFFT >( device
			, group
			, previousPasses
			, *oceanUbo );
		createFakeCombinePass( OceanFFT::Name
			, group
			, oceanFFT->getLastPasses()
			, oceanFFT->getGradientJacobian()
			, oceanFFT->getHeightDisplacement()
			, oceanFFT->getNormals() );
		auto runnable = fftGraph.compile( device.makeContext() );
		runnable->record();
#else
		auto oceanFFT = std::make_shared< OceanFFT >( device
			, graph
			, previousPasses
			, *oceanUbo
			, isEnabled );
#endif

		return rdpass::createNodesPass( device
			, graph
			, technique
			, renderPasses
			, std::move( passes )
			, std::move( oceanUbo )
			, std::move( oceanFFT )
			, std::move( colourInput )
			, std::move( depthInput )
			, isEnabled );
	}

	castor3d::TextureFlags OceanRenderPass::getTexturesMask()const
	{
		return castor3d::TextureFlags{ castor3d::TextureFlag::eAll };
	}

	castor3d::ShaderFlags OceanRenderPass::getShaderFlags()const
	{
		return castor3d::ShaderFlag::eWorldSpace
			| castor3d::ShaderFlag::eViewSpace
			| castor3d::ShaderFlag::eVelocity
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
			visitor.visit( cuT( "Water density" )
				, m_configuration.density
				, nullptr );

			m_oceanFFT->accept( visitor );
		}
	}

	void OceanRenderPass::doUpdateUbos( castor3d::CpuUpdater & updater )
	{
		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );
		m_configuration.time += float( tslf.count() ) / 1000.0f;
		m_ubo->cpuUpdate( m_configuration
			, m_oceanFFT->getConfig()
			, updater.camera->getParent()->getDerivedPosition() );
	}

	bool OceanRenderPass::doIsValidPass( castor3d::Pass const & pass )const
	{
		return pass.getRenderPassInfo()
			&& pass.getRenderPassInfo()->name == Type
			&& areValidPassFlags( pass.getPassFlags() );
	}

	void OceanRenderPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		bindings.emplace_back( getCuller().getScene().getLightCache().createLayoutBinding( rdpass::OceanFFTIdx::eLightBuffer ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eOceanUbo
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_VERTEX_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_FRAGMENT_BIT ) ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eHeightDisplacement
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ) ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eGradientJacobian
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eNormals
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eSceneNormals
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eSceneDepth
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eSceneResult
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eSceneBaseColour
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eSceneDiffuseLighting
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eBrdf
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto index = uint32_t( rdpass::OceanFFTIdx::eCount );
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
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( rdpass::OceanFFTIdx::eLightBuffer ) );
		descriptorWrites.push_back( m_ubo->getDescriptorWrite( rdpass::OceanFFTIdx::eOceanUbo ) );
		auto index = uint32_t( rdpass::OceanFFTIdx::eHeightDisplacement );
		rdpass::bindTexture( m_oceanFFT->getHeightDisplacement().sampledView, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_oceanFFT->getGradientJacobian().sampledView, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_oceanFFT->getNormals().sampledView, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_parent->getNormalTexture().sampledView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( m_depthInput->sampledView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( m_colourInput->sampledView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( m_parent->getBaseColourResult().sampledView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( m_parent->getDiffuseLightingResult().sampledView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampledView
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
		remFlag( flags, castor3d::SubmeshFlag::eNormals );
		remFlag( flags, castor3d::SubmeshFlag::eTangents );
		remFlag( flags, castor3d::SubmeshFlag::eTexcoords );
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
		flags.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		flags.patchVertices = rdpass::OutputVertices;
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
		C3D_FftOcean( writer
			, rdpass::OceanFFTIdx::eOceanUbo
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< castor3d::shader::VertexSurfaceT, rdpass::PatchT >( sdw::VertexInT< castor3d::shader::VertexSurfaceT >{ writer
				, flags }
			, sdw::VertexOutT< rdpass::PatchT >{ writer
				, flags }
			, [&]( VertexInT< castor3d::shader::VertexSurfaceT > in
				, VertexOutT< rdpass::PatchT > out )
			{
				auto pos = writer.declLocale( "pos"
					, ( ( in.position.xz() / c3d_oceanData.patchSize ) + c3d_oceanData.blockOffset ) * c3d_oceanData.patchSize );
				out.vtx.position = vec4( pos.x(), 0.0_f, pos.y(), 1.0_f );
				out.patchWorldPosition = out.vtx.position.xyz();
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, in
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID )
						, flags ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				out.colour = in.colour;
				out.nodeId = writer.cast< sdw::Int >( nodeId );
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
		C3D_FftOcean( writer
			, rdpass::OceanFFTIdx::eOceanUbo
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< VoidT, rdpass::PatchT >( sdw::VertexInT< sdw::VoidT >{ writer }
			, sdw::VertexOutT< rdpass::PatchT >{ writer
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
				out.colour = vec3( 1.0_f );
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
				out.patchWorldPosition = worldPos;
				out.vtx.position = modelData.worldToModel( vec4( worldPos, 1.0_f ) );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	castor3d::ShaderPtr OceanRenderPass::doGetHullShaderSource( castor3d::PipelineFlags const & flags )const
	{
		using namespace sdw;
		using namespace castor3d;
		TessellationControlWriter writer;

		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::eBuffers );
		C3D_FftOcean( writer
			, rdpass::OceanFFTIdx::eOceanUbo
			, RenderPipeline::eBuffers );

		auto tessLevel1f = writer.implementFunction< Float >( "tessLevel1f"
			, [&]( Float lod
				, Vec2 maxTessLevel )
			{
				writer.returnStmt( maxTessLevel.y() * exp2( -lod ) );
			}
			, sdw::InFloat{ writer, "lod" }
			, sdw::InVec2{ writer, "maxTessLevel" } );

		auto tessLevel4f = writer.implementFunction< Vec4 >( "tessLevel4f"
			, [&]( Vec4 lod
				, Vec2 maxTessLevel )
			{
				writer.returnStmt( maxTessLevel.y() * exp2( -lod ) );
			}
			, sdw::InVec4{ writer, "lod" }
			, sdw::InVec2{ writer, "maxTessLevel" } );

		auto lodFactor = writer.implementFunction< Float >( "lodFactor"
			, [&]( Vec3 worldPos
				, Vec3 worldEye
				, Vec2 tileScale
				, Vec2 maxTessLevel
				, Float distanceMod )
			{
				worldPos.xz() *= tileScale;
				auto distToCam = writer.declLocale( "distToCam"
					, worldEye - worldPos );
				auto level = writer.declLocale( "level"
					, log2( ( length( distToCam ) + 0.0001_f ) * distanceMod ) );
				writer.returnStmt( clamp( level
					, 0.0_f
					, maxTessLevel.x() ) );
			}
			, sdw::InVec3{ writer, "worldPos" }
			, sdw::InVec3{ writer, "worldEye" }
			, sdw::InVec2{ writer, "tileScale" }
			, sdw::InVec2{ writer, "maxTessLevel" }
			, sdw::InFloat{ writer, "distanceMod" } );

		writer.implementPatchRoutineT< rdpass::PatchT, rdpass::OutputVertices, rdpass::PatchT >( TessControlListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
				, false
				, flags }
			, sdw::QuadsTessPatchOutT< rdpass::PatchT >{ writer
				, 9u
				, flags }
			, [&]( sdw::TessControlPatchRoutineIn in
				, sdw::TessControlListInT< rdpass::PatchT, rdpass::OutputVertices > listIn
				, sdw::QuadsTessPatchOutT<rdpass::PatchT > patchOut )
			{
				auto patchSize = writer.declLocale( "patchSize"
					, vec3( c3d_oceanData.patchSize.x(), 0.0_f, c3d_oceanData.patchSize.y() ) );
				auto p0 = writer.declLocale( "p0"
					, listIn[0u].patchWorldPosition );

				auto l0 = writer.declLocale( "l0"
					, lodFactor( p0 + vec3( 0.0_f, 0.0f, 1.0f ) * patchSize
						, c3d_sceneData.cameraPosition
						, c3d_oceanData.tileScale
						, c3d_oceanData.maxTessLevel
						, c3d_oceanData.distanceMod ) );
				auto l1 = writer.declLocale( "l1"
					, lodFactor( p0 + vec3( 0.0_f, 0.0f, 0.0f ) * patchSize
						, c3d_sceneData.cameraPosition
						, c3d_oceanData.tileScale
						, c3d_oceanData.maxTessLevel
						, c3d_oceanData.distanceMod ) );
				auto l2 = writer.declLocale( "l2"
					, lodFactor( p0 + vec3( 1.0_f, 0.0f, 0.0f ) * patchSize
						, c3d_sceneData.cameraPosition
						, c3d_oceanData.tileScale
						, c3d_oceanData.maxTessLevel
						, c3d_oceanData.distanceMod ) );
				auto l3 = writer.declLocale( "l3"
					, lodFactor( p0 + vec3( 1.0_f, 0.0f, 1.0f ) * patchSize
						, c3d_sceneData.cameraPosition
						, c3d_oceanData.tileScale
						, c3d_oceanData.maxTessLevel
						, c3d_oceanData.distanceMod ) );

				auto lods = writer.declLocale( "lods"
					, vec4( l0, l1, l2, l3 ) );
				patchOut.patchWorldPosition = p0;
				patchOut.patchLods = lods;
				patchOut.colour = listIn[0u].colour;
				patchOut.nodeId = listIn[0u].nodeId;

				auto outerLods = writer.declLocale( "outerLods"
					, min( lods.xyzw(), lods.yzwx() ) );
				auto tessLevels = writer.declLocale( "tessLevels"
					, tessLevel4f( outerLods, c3d_oceanData.maxTessLevel ) );
				auto innerLevel = writer.declLocale( "innerLevel"
					, max( max( tessLevels.x(), tessLevels.y() )
						, max( tessLevels.z(), tessLevels.w() ) ) );

				patchOut.tessLevelOuter[0] = tessLevels.x();
				patchOut.tessLevelOuter[1] = tessLevels.y();
				patchOut.tessLevelOuter[2] = tessLevels.z();
				patchOut.tessLevelOuter[3] = tessLevels.w();

				patchOut.tessLevelInner[0] = innerLevel;
				patchOut.tessLevelInner[1] = innerLevel;
			} );

		writer.implementMainT< rdpass::PatchT, rdpass::OutputVertices, VoidT >( TessControlListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
				, true
				, flags }
			, TrianglesTessControlListOut{ writer
				, ast::type::Partitioning::eFractionalEven
				, ast::type::OutputTopology::eQuad
				, ast::type::PrimitiveOrdering::eCW
				, rdpass::OutputVertices }
			, [&]( TessControlMainIn in
				, TessControlListInT< rdpass::PatchT, rdpass::OutputVertices > listIn
				, TrianglesTessControlListOut listOut )
			{
				listOut.vtx.position = listIn[in.invocationID].vtx.position;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	castor3d::ShaderPtr OceanRenderPass::doGetDomainShaderSource( castor3d::PipelineFlags const & flags )const
	{
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
		C3D_FftOcean( writer
			, rdpass::OceanFFTIdx::eOceanUbo
			, RenderPipeline::eBuffers );
		auto c3d_heightDisplacementMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "c3d_heightDisplacementMap"
			, rdpass::OceanFFTIdx::eHeightDisplacement
			, RenderPipeline::eBuffers );

		auto lerpVertex = writer.implementFunction< Vec2 >( "lerpVertex"
			, [&]( Vec3 patchPosBase
				, Vec2 tessCoord
				, Vec2 patchSize )
			{
				writer.returnStmt( fma( tessCoord, patchSize, patchPosBase.xz() ) );
			}
			, sdw::InVec3{ writer, "patchPosBase" }
			, sdw::InVec2{ writer, "tessCoord" }
			, sdw::InVec2{ writer, "patchSize" } );

		auto lodFactor = writer.implementFunction< Vec2 >( "lodFactor"
			, [&]( Vec2 tessCoord
				, Vec4 patchLods )
			{
				// Bilinear interpolation from patch corners.
				auto x = writer.declLocale( "x"
					, mix( patchLods.yx(), patchLods.zw(), tessCoord.xx() ) );
				auto level = writer.declLocale( "level"
					, mix( x.x(), x.y(), tessCoord.y() ) );

				auto floorLevel = writer.declLocale( "floorLevel"
					, floor( level ) );
				auto fractLevel = writer.declLocale( "fractLevel"
					, level - floorLevel );
				writer.returnStmt( vec2( floorLevel, fractLevel ) );
			}
			, sdw::InVec2{ writer, "tessCoord" }
			, sdw::InVec4{ writer, "patchLods" } );

		auto sampleHeightDisplacement = writer.implementFunction< Vec3 >( "sampleHeightDisplacement"
			, [&]( Vec2 uv
				, Vec2 off
				, Vec2 lod )
			{
				writer.returnStmt( mix( c3d_heightDisplacementMap.lod( uv + vec2( 0.5_f ) * off, lod.x() ).xyz()
					, c3d_heightDisplacementMap.lod( uv + vec2( 1.0_f ) * off, lod.x() + 1.0_f ).xyz()
					, vec3( lod.y() ) ) );
			}
			, sdw::InVec2{ writer, "uv" }
			, sdw::InVec2{ writer, "off" }
			, sdw::InVec2{ writer, "lod" } );

		writer.implementMainT< rdpass::PatchT, rdpass::OutputVertices, rdpass::PatchT, castor3d::shader::FragmentSurfaceT >( TessEvalListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
				, ast::type::PatchDomain::eQuads
				, type::Partitioning::eFractionalEven
				, type::PrimitiveOrdering::eCW
				, flags }
			, QuadsTessPatchInT< rdpass::PatchT >{ writer
				, 9u
				, flags }
			, TessEvalDataOutT< castor3d::shader::FragmentSurfaceT >{ writer
				, flags }
			, [&]( TessEvalMainIn mainIn
				, TessEvalListInT< rdpass::PatchT, rdpass::OutputVertices > listIn
				, QuadsTessPatchInT< rdpass::PatchT > patchIn
				, TessEvalDataOutT< castor3d::shader::FragmentSurfaceT > out )
			{
				auto tessCoord = writer.declLocale( "tessCoord"
					, patchIn.tessCoord.xy() );
				auto pos = writer.declLocale( "pos"
					, lerpVertex( patchIn.patchWorldPosition, tessCoord, c3d_oceanData.patchSize ) );
				auto lod = writer.declLocale( "lod"
					, lodFactor( tessCoord, patchIn.patchLods ) );

				auto tex = writer.declLocale( "tex"
					, pos * c3d_oceanData.invHeightmapSize.xy() );
				pos *= c3d_oceanData.tileScale;

				auto deltaMod = writer.declLocale( "deltaMod"
					, exp2( lod.x() ) );
				auto off = writer.declLocale( "off"
					, c3d_oceanData.invHeightmapSize.xy() * deltaMod );

				out.prvPosition = vec4( tex + vec2( 0.5_f ) * c3d_oceanData.invHeightmapSize.xy()
					, tex * c3d_oceanData.normalScale );
				auto heightDisplacement = writer.declLocale( "heightDisplacement"
					, sampleHeightDisplacement( tex, off, lod ) );

				pos += heightDisplacement.yz();

				out.colour = patchIn.colour;
				out.nodeId = patchIn.nodeId;
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( out.nodeId ) - 1u] );
				out.curPosition = vec4( pos.x(), heightDisplacement.x(), pos.y(), 1.0_f );
				out.worldPosition = modelData.modelToWorld( out.curPosition );
				out.viewPosition = c3d_matrixData.worldToCurView( out.worldPosition );
				out.vtx.position = c3d_matrixData.viewToProj( out.viewPosition );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	castor3d::ShaderPtr OceanRenderPass::doGetPixelShaderSource( castor3d::PipelineFlags const & flags )const
	{
#if Ocean_Debug
#	define displayDebugData( OceanDataType, RGB, A )\
	IF( writer, c3d_oceanData.debug == sdw::UInt( OceanDataType ) )\
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
		C3D_FftOcean( writer
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_heightDisplacementMap = writer.declCombinedImg< sdw::CombinedImage2DRgba16 >( "c3d_heightDisplacementMap"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_gradientJacobianMap = writer.declCombinedImg< sdw::CombinedImage2DRgba16 >( "c3d_gradientJacobianMap"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_normalsMap = writer.declCombinedImg< sdw::CombinedImage2DRg32 >( "c3d_normalsMap"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_sceneNormals = writer.declCombinedImg< FImg2DRgba32 >( "c3d_sceneNormals"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_sceneDepth = writer.declCombinedImg< FImg2DR32 >( "c3d_sceneDepth"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_sceneColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_sceneColour"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_sceneBaseColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_sceneBaseColour"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_sceneDiffuseLighting = writer.declCombinedImg< FImg2DRgba32 >( "c3d_sceneDiffuseLighting"
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

		auto getLightAbsorbtion = writer.implementFunction< sdw::Vec3 >( "getLightAbsorbtion"
			, [&]( sdw::Vec2 const & fftTexcoord
				, sdw::Vec3 const & surfacePosition
				, sdw::Vec3 const & refractedPosition
				, sdw::Vec3 const & diffuseLighting )
			{
				writer.returnStmt( diffuseLighting * ( 1.0_f - utils.saturate( sdw::log( surfacePosition.y() - refractedPosition.y() ) * c3d_oceanData.density ) ) );
			}
			, sdw::InVec2{ writer, "fftTexcoord" }
			, sdw::InVec3{ writer, "surfacePosition" }
			, sdw::InVec3{ writer, "refractedPosition" }
			, sdw::InVec3{ writer, "diffuseColour" } );

		writer.implementMainT< castor3d::shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< castor3d::shader::FragmentSurfaceT >{ writer
				, flags }
			, FragmentOut{ writer }
			, [&]( FragmentInT< castor3d::shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				auto hdrCoords = writer.declLocale( "hdrCoords"
					, in.fragCoord.xy() / c3d_sceneData.renderSize );
				auto gradJacobian = writer.declLocale( "gradJacobian"
					, c3d_gradientJacobianMap.sample( in.prvPosition.xy() ).xyz() );
				displayDebugData( eGradJacobian, gradJacobian, 1.0_f );
				auto noiseGradient = writer.declLocale( "noiseGradient"
					, vec2( 0.3_f ) * c3d_normalsMap.sample( in.prvPosition.zw() ) );
				displayDebugData( eNoiseGradient, vec3( noiseGradient, 0.0_f ), 1.0_f );

				auto jacobian = writer.declLocale( "jacobian"
					, gradJacobian.z() );
				displayDebugData( eJacobian, vec3( jacobian ), 1.0_f );
				auto turbulence = writer.declLocale( "turbulence"
					, max( 2.0_f - jacobian + dot( abs( noiseGradient ), vec2( 1.2_f ) )
						, 0.0_f ) );
				displayDebugData( eTurbulence, vec3( turbulence ), 1.0_f );

				// Add low frequency gradient from heightmap with gradient from high frequency noisemap.
				auto finalNormal = writer.declLocale( "finalNormal"
					, vec3( -gradJacobian.x(), 1.0, -gradJacobian.y() ) );
				finalNormal.xz() -= noiseGradient;
				finalNormal = normalize( finalNormal );

				if ( flags.hasInvertNormals() )
				{
					finalNormal = -finalNormal;
				}

				displayDebugData( eNormal, finalNormal, 1.0_f );

				auto worldToCam = writer.declLocale( "worldToCam"
					, c3d_sceneData.getPosToCamera( in.worldPosition.xyz() ) );
				auto NdotV = writer.declLocale( "NdotV"
					, dot( worldToCam, finalNormal ) );

				// Make water brighter based on how turbulent the water is.
				// This is rather "arbitrary", but looks pretty good in practice.
				auto colorMod = writer.declLocale( "colorMod"
					, 1.0_f + 3.0_f * smoothStep( 1.2_f, 1.8_f, turbulence ) );
				displayDebugData( eColorMod, vec3( colorMod ), 1.0_f );

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
						, worldToCam
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
								, finalNormal
								, lightMat->specular
								, lightMat->getMetalness()
								, surface )
							: vec3( 0.0_f ) ) );
					displayDebugData( eIndirectDiffuse, indirectDiffuse, 1.0_f );


					//  Retrieve non distorted scene data.
					auto sceneDepth = writer.declLocale( "sceneDepth"
						, c3d_sceneDepth.sample( hdrCoords ) );
					auto scenePosition = writer.declLocale( "scenePosition"
						, c3d_matrixData.curProjToWorld( utils, hdrCoords, sceneDepth ) );


					// Reflection
					auto reflected = reflections->computeReflections( *lightMat
						, surface
						, c3d_sceneData
						, *backgroundModel
						, modelData.getEnvMapIndex()
						, material.hasReflection() );
					displayDebugData( eRawBackgroundReflection, reflected, 1.0_f );
					reflected = utils.fresnelSchlick( NdotV, lightMat->specular ) * reflected;
					displayDebugData( eFresnelBackgroundReflection, reflected, 1.0_f );
					auto ssrResult = writer.declLocale( "ssrResult"
						, reflections->computeScreenSpace( c3d_matrixData
							, surface.viewPosition
							, finalNormal
							, hdrCoords
							, c3d_oceanData.ssrSettings
							, c3d_sceneDepth
							, c3d_sceneNormals
							, c3d_sceneColour ) );
					displayDebugData( eSSRResult, ssrResult.xyz(), 1.0_f );
					displayDebugData( eSSRFactor, ssrResult.www(), 1.0_f );
					displayDebugData( eSSRResultFactor, ssrResult.xyz() * ssrResult.www(), 1.0_f );
					auto reflectionResult = writer.declLocale( "reflectionResult"
						, mix( reflected, ssrResult.xyz(), ssrResult.www() ) );
					displayDebugData( eCombinedReflection, reflectionResult, 1.0_f );


					// Wobbly refractions
					auto heightFactor = writer.declLocale( "heightFactor"
						, c3d_oceanData.refractionHeightFactor * ( c3d_sceneData.farPlane - c3d_sceneData.nearPlane ) );
					auto distanceFactor = writer.declLocale( "distanceFactor"
						, c3d_oceanData.refractionDistanceFactor * ( c3d_sceneData.farPlane - c3d_sceneData.nearPlane ) );
					auto distortedTexCoord = writer.declLocale( "distortedTexCoord"
						, fma( vec2( ( finalNormal.xz() + finalNormal.xy() ) * 0.5_f )
							, vec2( c3d_oceanData.refractionDistortionFactor
								* utils.saturate( length( scenePosition - surface.worldPosition ) * 0.5_f ) )
							, hdrCoords ) );
					auto distortedDepth = writer.declLocale( "distortedDepth"
						, c3d_sceneDepth.sample( distortedTexCoord ) );
					auto distortedPosition = writer.declLocale( "distortedPosition"
						, c3d_matrixData.curProjToWorld( utils, distortedTexCoord, distortedDepth ) );
					auto refractionTexCoord = writer.declLocale( "refractionTexCoord"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedTexCoord, hdrCoords ) );
					auto refractionResult = writer.declLocale( "refractionResult"
						, c3d_sceneColour.sample( distortedTexCoord ).xyz() );
					displayDebugData( eRawRefraction, refractionResult, 1.0_f );
					auto lightAbsorbtion = writer.declLocale( "lightAbsorbtion"
						, getLightAbsorbtion( in.prvPosition.xy()
							, in.curPosition.xyz()
							, distortedPosition
							, refractionResult ) );
					displayDebugData( eLightAbsorbtion, lightAbsorbtion, 1.0_f );
					auto waterTransmission = writer.declLocale( "waterTransmission"
						, material.transmission() * lightAbsorbtion * ( indirectAmbient + indirectDiffuse ) );
					displayDebugData( eWaterTransmission, waterTransmission, 1.0_f );
					refractionResult *= waterTransmission;
					displayDebugData( eRefractionResult, refractionResult, 1.0_f );
					// Depth softening, to fade the alpha of the water where it meets the scene geometry by some predetermined distance. 
					auto depthSoftenedAlpha = writer.declLocale( "depthSoftenedAlpha"
						, clamp( distance( scenePosition, in.worldPosition.xyz() ) / c3d_oceanData.depthSofteningDistance, 0.0_f, 1.0_f ) );
					displayDebugData( eDepthSoftenedAlpha, vec3( depthSoftenedAlpha ), 1.0_f );
					auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedPosition, scenePosition ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, vec3( clamp( ( in.worldPosition.y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
					displayDebugData( eHeightMixedRefraction, refractionResult, 1.0_f );
					refractionResult = mix( refractionResult
						, waterTransmission
						, utils.saturate( vec3( utils.saturate( length( in.viewPosition.xyz() ) / distanceFactor ) ) ) );
					displayDebugData( eDistanceMixedRefraction, refractionResult, 1.0_f );


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
							+ refractionResult * colorMod
							+ ( reflectionResult * colorMod * indirectAmbient )
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

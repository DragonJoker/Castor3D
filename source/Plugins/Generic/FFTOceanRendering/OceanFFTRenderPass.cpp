#include "FFTOceanRendering/OceanFFTRenderPass.hpp"

#include "FFTOceanRendering/OceanFFT.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp>
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
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/Program.hpp>
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include <Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>
#include <Castor3D/Shader/Shaders/GlslFog.hpp>
#include <Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslMeshVertex.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureAnimation.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/BillboardUbo.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>
#include <Castor3D/Shader/Ubos/ModelDataUbo.hpp>
#include <Castor3D/Shader/Ubos/ObjectIdsUbo.hpp>
#include <Castor3D/Shader/Ubos/MorphingUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

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
						, crg::RunnablePass::GetPipelineStateCallback( [this](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } ) } }
			{
			}

		private:
			void doInitialise()
			{
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
				, sdw::EntryPoint entryPoint
				, castor3d::PipelineFlags flags )
			{
				auto result = cache.getIOStruct( "C3DORFFT_Patch"
					, entryPoint
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

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, castor3d::PipelineFlags flags )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eC
					, "C3DORFFT_Patch" );

				if ( result->empty() )
				{
					result->declMember( "patchWorldPosition"
						, ast::type::Kind::eVec3F
						, ast::type::NotArray );
					result->declMember( "patchLodsGradNormTex"
						, ast::type::Kind::eVec4F
						, ast::type::NotArray );
					result->declMember( "colour"
						, ast::type::Kind::eVec3F
						, ast::type::NotArray
						, flags.enableColours() );
					result->declMember( "nodeId"
						, ast::type::Kind::eInt
						, ast::type::NotArray );
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
			eBrdf,
			eCount,
		};

		static void bindTexture( VkImageView view
			, VkSampler sampler
			, ashes::WriteDescriptorSetArray & writes
			, uint32_t & index )
		{
			writes.push_back( castor3d::makeImageViewDescriptorWrite( view, sampler, index++ ) );
		}

		static crg::FramePassArray createNodesPass( castor3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, castor3d::RenderTechnique & technique
			, castor3d::TechniquePasses & renderPasses
			, crg::FramePassArray previousPasses
			, std::unique_ptr< OceanUbo > oceanUbo
			, std::unique_ptr< OceanFFT > oceanFFT
			, castor3d::IsRenderPassEnabledRPtr isEnabled )
		{
			auto targetResult = technique.getTargetResult();
			auto targetDepth = technique.getTargetDepth();
			auto extent = technique.getTargetExtent();
			auto ocUbo = oceanUbo.release();
			auto ocFFT = oceanFFT.release();
			auto & result = graph.createPass( "NodesPass"
				, [extent, targetResult, targetDepth, ocUbo, ocFFT, isEnabled, &device, &technique, &renderPasses]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
			{
					auto res = std::make_unique< OceanRenderPass >( &technique
						, framePass
						, context
						, runnableGraph
						, device
						, std::unique_ptr< OceanUbo >( ocUbo )
						, std::unique_ptr< OceanFFT >( ocFFT )
						, targetResult
						, targetDepth
						, castor3d::RenderNodesPassDesc{ extent
							, technique.getCameraUbo()
							, technique.getSceneUbo()
							, technique.getRenderTarget().getCuller() }
							.safeBand( true )
							.allowClusteredLighting()
							.componentModeFlags( castor3d::ComponentModeFlag::eColour
								| castor3d::ComponentModeFlag::eNormals
								| castor3d::ComponentModeFlag::eOcclusion
								| castor3d::ComponentModeFlag::eDiffuseLighting
								| castor3d::ComponentModeFlag::eSpecularLighting
								| castor3d::ComponentModeFlag::eSpecifics )
						, castor3d::RenderTechniquePassDesc{ false, technique.getSsaoConfig() }
							.ssao( technique.getSsaoResult() )
							.indirect( technique.getIndirectLighting() )
							.clustersConfig( technique.getClustersConfig() )
						, castor3d::IsRenderPassEnabledUPtr( isEnabled ) );
					renderPasses[size_t( OceanRenderPass::Event )].push_back( res.get() );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, res->getTimer() );
					return res;
			} );

			result.addDependencies( previousPasses );
#if Ocean_DebugFFTGraph
			result.addInOutDepthStencilView( technique.getTargetDepth() );
			result.addInOutColourView( technique.getTargetResult() );
#else
			result.addDependencies( ocFFT->getLastPasses() );
			result.addDependency( technique.getGetLastOpaquePass() );
			result.addImplicitColourView( technique.getSampledIntermediate()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( technique.getDepthObj().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( technique.getNormal().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( ocFFT->getNormals().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( ocFFT->getHeightDisplacement().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addImplicitColourView( ocFFT->getGradientJacobian().sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addInOutDepthStencilView( technique.getTargetDepth() );
			result.addInOutColourView( technique.getTargetResult() );
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
		, std::unique_ptr< OceanUbo > oceanUbo
		, std::unique_ptr< OceanFFT > oceanFFT
		, crg::ImageViewIdArray targetImage
		, crg::ImageViewIdArray targetDepth
		, castor3d::RenderNodesPassDesc const & renderPassDesc
		, castor3d::RenderTechniquePassDesc const & techniquePassDesc
			, castor3d::IsRenderPassEnabledUPtr isEnabled )
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
		, m_ubo{ std::move( oceanUbo ) }
		, m_oceanFFT{ std::move( oceanFFT ) }
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
	}

	crg::FramePassArray OceanRenderPass::create( castor3d::RenderDevice const & device
		, castor3d::RenderTechnique & technique
		, castor3d::TechniquePasses & renderPasses
		, crg::FramePassArray previousPasses )
	{
		auto isEnabled = new castor3d::IsRenderPassEnabled{};
		auto & graph = technique.getGraph().createPassGroup( OceanFFT::Name );
		auto extent = technique.getTargetExtent();
		crg::FramePassArray passes{ previousPasses };

		auto oceanUbo = std::make_unique< OceanUbo >( device );
#if Ocean_DebugFFTGraph
		crg::FrameGraph fftGraph{ graph.getHandler(), OceanFFT::Name };
		auto & group = fftGraph.createPassGroup( OceanFFT::Name );
		auto oceanFFT = std::make_unique< OceanFFT >( device
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
		getEngine()->registerTimer( runnable->getName() + "/Graph"
			, runnable->getTimer() );
		runnable->record();
#else
		auto oceanFFT = std::make_unique< OceanFFT >( device
			, technique.getResources()
			, graph
			, previousPasses
			, *oceanUbo
			, isEnabled );
#endif
		
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
		passes.push_back( &blitColourPass );

		return rdpass::createNodesPass( device
			, graph
			, technique
			, renderPasses
			, std::move( passes )
			, std::move( oceanUbo )
			, std::move( oceanFFT )
			, isEnabled );
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
			visitor.visit( cuT( "Refraction ratio" ), m_configuration.refractionRatio );
			visitor.visit( cuT( "Refraction distortion factor" ), m_configuration.refractionDistortionFactor );
			visitor.visit( cuT( "Refraction height factor" ), m_configuration.refractionHeightFactor );
			visitor.visit( cuT( "Refraction distance factor" ), m_configuration.refractionDistanceFactor );
			visitor.visit( cuT( "Depth softening distance" ), m_configuration.depthSofteningDistance );
			visitor.visit( cuT( "SSR step size" ), m_configuration.ssrStepSize );
			visitor.visit( cuT( "SSR forward steps count" ), m_configuration.ssrForwardStepsCount );
			visitor.visit( cuT( "SSR backward steps count" ), m_configuration.ssrBackwardStepsCount );
			visitor.visit( cuT( "SSR depth mult." ), m_configuration.ssrDepthMult );
			visitor.visit( cuT( "Water density" ), m_configuration.density );

			m_oceanFFT->accept( visitor );

			doAccept( visitor );
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

	void OceanRenderPass::doFillAdditionalBindings( castor3d::PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		bindings.emplace_back( getCuller().getScene().getLightCache().createLayoutBinding( VK_SHADER_STAGE_FRAGMENT_BIT
			, rdpass::OceanFFTIdx::eLightBuffer ) );
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
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( rdpass::OceanFFTIdx::eBrdf
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto index = uint32_t( rdpass::OceanFFTIdx::eCount );
		doAddShadowBindings( m_scene, flags, bindings, index );
		doAddEnvBindings( flags, bindings, index );
		doAddBackgroundBindings( m_scene, bindings, index );
		doAddGIBindings( flags, bindings, index );
		doAddClusteredLightingBindings( m_parent->getRenderTarget(), flags, bindings, index );
	}

	ashes::PipelineDepthStencilStateCreateInfo OceanRenderPass::doCreateDepthStencilState( castor3d::PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, VK_TRUE
			, VK_COMPARE_OP_GREATER };
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
		, castor3d::ShadowMapLightTypeArray const & shadowMaps
		, castor3d::ShadowBuffer const * shadowBuffer )
	{
		descriptorWrites.push_back( m_scene.getLightCache().getBinding( rdpass::OceanFFTIdx::eLightBuffer ) );
		descriptorWrites.push_back( m_ubo->getDescriptorWrite( rdpass::OceanFFTIdx::eOceanUbo ) );
		auto index = uint32_t( rdpass::OceanFFTIdx::eHeightDisplacement );
		rdpass::bindTexture( m_oceanFFT->getHeightDisplacement().sampledView, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_oceanFFT->getGradientJacobian().sampledView, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_oceanFFT->getNormals().sampledView, *m_linearWrapSampler, descriptorWrites, index );
		rdpass::bindTexture( m_parent->getNormal().sampledView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( getTechnique().getDepthObj().wholeView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( getTechnique().getIntermediate().wholeView, *m_pointClampSampler, descriptorWrites, index );
		rdpass::bindTexture( getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampledView
			, *getOwner()->getRenderSystem()->getPrefilteredBrdfTexture().sampler
			, descriptorWrites
			, index );
		doAddShadowDescriptor( m_scene, flags, descriptorWrites, shadowMaps, shadowBuffer, index );
		doAddEnvDescriptor( flags, descriptorWrites, index );
		doAddBackgroundDescriptor( m_scene, descriptorWrites, m_targetImage, index );
		doAddGIDescriptor( flags, descriptorWrites, index );
		doAddClusteredLightingDescriptor( m_parent->getRenderTarget(), flags, descriptorWrites, index );
	}
	
	castor3d::SubmeshComponentCombine OceanRenderPass::doAdjustSubmeshComponents( castor3d::SubmeshComponentCombine submeshCombine )const
	{
		auto & components = getEngine()->getSubmeshComponentsRegister();
		remFlags( submeshCombine, components.getNormalFlag() );
		remFlags( submeshCombine, components.getTangentFlag() );
		remFlags( submeshCombine, components.getBitangentFlag() );
		remFlags( submeshCombine, components.getColourFlag() );
		remFlags( submeshCombine, components.getTexcoord0Flag() );
		remFlags( submeshCombine, components.getTexcoord1Flag() );
		remFlags( submeshCombine, components.getTexcoord2Flag() );
		remFlags( submeshCombine, components.getTexcoord3Flag() );
		submeshCombine.hasNormalFlag = false;
		submeshCombine.hasTangentFlag = false;
		submeshCombine.hasBitangentFlag = false;
		submeshCombine.hasTexcoord0Flag = false;
		submeshCombine.hasTexcoord1Flag = false;
		submeshCombine.hasTexcoord2Flag = false;
		submeshCombine.hasTexcoord3Flag = false;
		return submeshCombine;
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

	void OceanRenderPass::doGetSubmeshShaderSource( castor3d::PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		using namespace castor3d;
		sdw::TraditionalGraphicsWriter writer{ builder };
		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };
		shader::SubmeshShaders submeshShaders{ getEngine()->getSubmeshComponentsRegister()
			, flags };

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
		C3D_FftOcean( writer
			, rdpass::OceanFFTIdx::eOceanUbo
			, RenderPipeline::eBuffers );
		auto c3d_heightDisplacementMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "c3d_heightDisplacementMap"
			, rdpass::OceanFFTIdx::eHeightDisplacement
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		auto tessLevel1f = writer.implementFunction< sdw::Float >( "tessLevel1f"
			, [&]( sdw::Float lod
				, sdw::Vec2 maxTessLevel )
			{
				writer.returnStmt( maxTessLevel.y() * exp2( -lod ) );
			}
			, sdw::InFloat{ writer, "lod" }
			, sdw::InVec2{ writer, "maxTessLevel" } );

		auto tessLevel4f = writer.implementFunction< sdw::Vec4 >( "tessLevel4f"
			, [&]( sdw::Vec4 lod
				, sdw::Vec2 maxTessLevel )
			{
				writer.returnStmt( maxTessLevel.y() * exp2( -lod ) );
			}
			, sdw::InVec4{ writer, "lod" }
			, sdw::InVec2{ writer, "maxTessLevel" } );

		auto lodFactors = writer.implementFunction< sdw::Float >( "lodFactors"
			, [&]( sdw::Vec3 worldPos
				, sdw::Vec3 worldEye
				, sdw::Vec2 tileScale
				, sdw::Vec2 maxTessLevel
				, sdw::Float distanceMod )
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

		auto lerpVertex = writer.implementFunction< sdw::Vec2 >( "lerpVertex"
			, [&]( sdw::Vec3 patchPosBase
				, sdw::Vec2 tessCoord
				, sdw::Vec2 patchSize )
			{
				writer.returnStmt( fma( tessCoord, patchSize, patchPosBase.xz() ) );
			}
			, sdw::InVec3{ writer, "patchPosBase" }
			, sdw::InVec2{ writer, "tessCoord" }
			, sdw::InVec2{ writer, "patchSize" } );

		auto lodFactor = writer.implementFunction< sdw::Vec2 >( "lodFactor"
			, [&]( sdw::Vec2 tessCoord
				, sdw::Vec4 patchLods )
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

		auto sampleHeightDisplacement = writer.implementFunction< sdw::Vec3 >( "sampleHeightDisplacement"
			, [&]( sdw::Vec2 uv
				, sdw::Vec2 off
				, sdw::Vec2 lod )
			{
				writer.returnStmt( mix( c3d_heightDisplacementMap.lod( uv + vec2( 0.5_f ) * off, lod.x() ).xyz()
					, c3d_heightDisplacementMap.lod( uv + vec2( 1.0_f ) * off, lod.x() + 1.0_f ).xyz()
					, vec3( lod.y() ) ) );
			}
			, sdw::InVec2{ writer, "uv" }
			, sdw::InVec2{ writer, "off" }
			, sdw::InVec2{ writer, "lod" } );

		if ( flags.isBillboard() )
		{
			C3D_Billboard( writer
				, GlobalBuffersIdx::eBillboardsData
				, RenderPipeline::eBuffers );

			writer.implementEntryPointT< shader::BillboardSurfaceT, rdpass::PatchT >( sdw::VertexInT< shader::BillboardSurfaceT >{ writer, flags }
				, sdw::VertexOutT< rdpass::PatchT >{ writer, flags }
				, [&]( sdw::VertexInT< shader::BillboardSurfaceT > in
					, sdw::VertexOutT< rdpass::PatchT > out )
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
						, modelData.modelToCurWorld( vec4( in.center, 1.0_f ) ).xyz() );
					auto prvBbcenter = writer.declLocale( "prvBbcenter"
						, modelData.modelToPrvWorld( vec4( in.center, 1.0_f ) ).xyz() );
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
						, right * in.position.x() * width );
					auto scaledUp = writer.declLocale( "scaledUp"
						, up * in.position.y() * height );
					auto worldPos = writer.declLocale( "worldPos"
						, ( curBbcenter + scaledRight + scaledUp ) );
					out.patchWorldPosition = worldPos;
					out.vtx.position = modelData.worldToModel( vec4( worldPos, 1.0_f ) );
				} );
		}
		else
		{
			writer.implementEntryPointT< castor3d::shader::MeshVertexT, rdpass::PatchT >( sdw::VertexInT< castor3d::shader::MeshVertexT >{ writer, submeshShaders }
				, sdw::VertexOutT< rdpass::PatchT >{ writer, flags }
				, [&]( sdw::VertexInT< castor3d::shader::MeshVertexT > in
					, sdw::VertexOutT< rdpass::PatchT > out )
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
		}

		writer.implementPatchRoutineT< rdpass::PatchT, rdpass::OutputVertices, rdpass::PatchT >( sdw::TessControlListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
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
					, lodFactors( p0 + vec3( 0.0_f, 0.0f, 1.0f ) * patchSize
						, c3d_cameraData.position()
						, c3d_oceanData.tileScale
						, c3d_oceanData.maxTessLevel
						, c3d_oceanData.distanceMod ) );
				auto l1 = writer.declLocale( "l1"
					, lodFactors( p0 + vec3( 0.0_f, 0.0f, 0.0f ) * patchSize
						, c3d_cameraData.position()
						, c3d_oceanData.tileScale
						, c3d_oceanData.maxTessLevel
						, c3d_oceanData.distanceMod ) );
				auto l2 = writer.declLocale( "l2"
					, lodFactors( p0 + vec3( 1.0_f, 0.0f, 0.0f ) * patchSize
						, c3d_cameraData.position()
						, c3d_oceanData.tileScale
						, c3d_oceanData.maxTessLevel
						, c3d_oceanData.distanceMod ) );
				auto l3 = writer.declLocale( "l3"
					, lodFactors( p0 + vec3( 1.0_f, 0.0f, 1.0f ) * patchSize
						, c3d_cameraData.position()
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

		writer.implementEntryPointT< rdpass::PatchT, rdpass::OutputVertices, sdw::VoidT >( sdw::TessControlListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
				, true
				, flags }
			, sdw::TrianglesTessControlListOut{ writer
				, ast::type::Partitioning::eFractionalEven
				, ast::type::OutputTopology::eQuad
				, ast::type::PrimitiveOrdering::eCW
				, rdpass::OutputVertices }
			, [&]( sdw::TessControlMainIn in
				, sdw::TessControlListInT< rdpass::PatchT, rdpass::OutputVertices > listIn
				, sdw::TrianglesTessControlListOut listOut )
			{
				listOut.vtx.position = listIn[in.invocationID].vtx.position;
			} );

		writer.implementEntryPointT< rdpass::PatchT, rdpass::OutputVertices, rdpass::PatchT, castor3d::shader::FragmentSurfaceT >( sdw::TessEvalListInT< rdpass::PatchT, rdpass::OutputVertices >{ writer
				, ast::type::PatchDomain::eQuads
				, ast::type::Partitioning::eFractionalEven
				, ast::type::PrimitiveOrdering::eCW
				, flags }
			, sdw::QuadsTessPatchInT< rdpass::PatchT >{ writer
				, 9u
				, flags }
			, sdw::TessEvalDataOutT< castor3d::shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, [&]( sdw::TessEvalMainIn mainIn
				, sdw::TessEvalListInT< rdpass::PatchT, rdpass::OutputVertices > listIn
				, sdw::QuadsTessPatchInT< rdpass::PatchT > patchIn
				, sdw::TessEvalDataOutT< castor3d::shader::FragmentSurfaceT > out )
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
				out.viewPosition = c3d_cameraData.worldToCurView( out.worldPosition );
				out.vtx.position = c3d_cameraData.viewToProj( out.viewPosition );
			} );
	}

	void OceanRenderPass::doGetPixelShaderSource( castor3d::PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		using namespace castor3d;
		sdw::FragmentWriter writer{ builder };

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
		auto c3d_sceneDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_sceneDepthObj"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_sceneColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_sceneColour"
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
			, true /* enableVolumetric */ };
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
		shader::GlobalIllumination indirect{ writer
			, utils
			, index
			, RenderPipeline::eBuffers
			, flags.getGlobalIlluminationFlags()
			, getIndirectLighting() };
		shader::ClusteredLights clusteredLights{ writer
			, index
			, RenderPipeline::eBuffers
			, getClustersConfig() };

		// Fragment Outputs
		auto outColour( writer.declOutput< sdw::Vec4 >( "outColour", 0 ) );

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

		writer.implementMainT< shader::FragmentSurfaceT, sdw::VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, sdw::FragmentOut{ writer }
			, [&]( sdw::FragmentInT< shader::FragmentSurfaceT > in
				, sdw::FragmentOut out )
			{
				shader::DebugOutput output{ getDebugConfig()
					, cuT( "FFTOcean" )
					, c3d_cameraData.debugIndex()
					, outColour
					, areDebugTargetsEnabled() };
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				auto hdrCoords = writer.declLocale( "hdrCoords"
					, in.fragCoord.xy() / vec2( c3d_cameraData.renderSize() ) );
				auto gradJacobian = writer.declLocale( "gradJacobian"
					, c3d_gradientJacobianMap.sample( in.prvPosition.xy() ).xyz() );
				output.registerOutput( "Grad Jacobian", gradJacobian );
				auto noiseGradient = writer.declLocale( "noiseGradient"
					, vec2( 0.3_f ) * c3d_normalsMap.sample( in.prvPosition.zw() ) );
				output.registerOutput( "Noise Gradient", noiseGradient );

				auto jacobian = writer.declLocale( "jacobian"
					, gradJacobian.z() );
				output.registerOutput( "Jacobian", jacobian );
				auto turbulence = writer.declLocale( "turbulence"
					, max( 2.0_f - jacobian + dot( abs( noiseGradient ), vec2( 1.2_f ) )
						, 0.0_f ) );
				output.registerOutput( "Turbulence", turbulence );

				// Add low frequency gradient from heightmap with gradient from high frequency noisemap.
				auto finalNormal = writer.declLocale( "finalNormal"
					, vec3( -gradJacobian.x(), 1.0, -gradJacobian.y() ) );
				finalNormal.xz() -= noiseGradient;
				finalNormal = normalize( finalNormal );

				if ( flags.isFrontCulled() )
				{
					finalNormal = -finalNormal;
				}

				output.registerOutput( "Normal", finalNormal );

				// Make water brighter based on how turbulent the water is.
				// This is rather "arbitrary", but looks pretty good in practice.
				auto colorMod = writer.declLocale( "colorMod"
					, 1.0_f + 3.0_f * smoothStep( 1.2_f, 1.8_f, turbulence ) );
				output.registerOutput( "ColorMod", colorMod );

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
				output.registerOutput( "MatSpecular", components.f0 );

				if ( auto lightingModel = lights.getLightingModel() )
				{
					// Direct Lighting
					auto nml = writer.declLocale( "nml"
						, normalize( components.normal ) );
					shader::OutputComponents lighting{ writer, false };
					auto lightSurface = shader::LightSurface::create( writer
						, "lightSurface"
						, c3d_cameraData.position()
						, surface.worldPosition.xyz()
						, surface.viewPosition.xyz()
						, surface.clipPosition
						, nml );
					lights.computeCombinedDifSpec( clusteredLights
						, components
						, *backgroundModel
						, lightSurface
						, modelData.isShadowReceiver()
						, lightSurface.clipPosition().xy()
						, lightSurface.viewPosition().z()
						, output
						, lighting );
					lightingModel->adjustDirectSpecular( components
						, lighting.specular );


					// Indirect Lighting
					lightSurface.updateL( utils
						, nml
						, components.f0
						, components );
					auto indirectOcclusion = indirect.computeOcclusion( flags.getGlobalIlluminationFlags()
						, lightSurface
						, output );
					auto lightIndirectDiffuse = indirect.computeDiffuse( flags.getGlobalIlluminationFlags()
						, lightSurface
						, indirectOcclusion
						, output );
					auto lightIndirectSpecular = indirect.computeSpecular( flags.getGlobalIlluminationFlags()
						, lightSurface
						, components.roughness
						, indirectOcclusion
						, lightIndirectDiffuse.w()
						, c3d_mapBrdf
						, output );
					auto indirectAmbient = writer.declLocale( "indirectAmbient"
						, indirect.computeAmbient( flags.getGlobalIlluminationFlags()
							, lightIndirectDiffuse.xyz()
							, output ) );
					auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
						, ( hasDiffuseGI
							? cookTorrance.computeDiffuse( lightIndirectDiffuse.xyz()
								, length( lightIndirectDiffuse.xyz() )
								, lightSurface.difF() )
							: vec3( 0.0_f ) ) );
					output.registerOutput( "Indirect", "Diffuse", indirectDiffuse );


					//  Retrieve non distorted scene data.
					auto sceneDepth = writer.declLocale( "sceneDepth"
						, c3d_sceneDepthObj.sample( hdrCoords ).r() );
					auto scenePosition = writer.declLocale( "scenePosition"
						, c3d_cameraData.curProjToWorld( utils, hdrCoords, sceneDepth ) );


					// Reflection
					auto bgDiffuseReflection = writer.declLocale( "bgDiffuseReflection"
						, vec3( 0.0_f ) );
					auto bgSpecularReflection = writer.declLocale( "bgSpecularReflection"
						, vec3( 0.0_f ) );
					lightSurface.updateN( utils
						, nml
						, components.f0
						, components );
					reflections.computeReflections( components
						, lightSurface
						, *backgroundModel
						, modelData.getEnvMapIndex()
						, components.hasReflection
						, bgDiffuseReflection
						, bgSpecularReflection
						, output );
					auto reflected = writer.declLocale( "reflected"
						, bgDiffuseReflection + bgSpecularReflection );
					output.registerOutput( "RawBackgroundReflection", reflected );
					reflected = lightSurface.F() * reflected;
					output.registerOutput( "FresnelBackgroundReflection", reflected );
					auto ssrResult = writer.declLocale( "ssrResult"
						, reflections.computeScreenSpace( c3d_cameraData
							, lightSurface.viewPosition()
							, lightSurface.N()
							, hdrCoords
							, c3d_oceanData.ssrSettings
							, c3d_sceneDepthObj
							, c3d_sceneNormals
							, c3d_sceneColour
							, output ) );
					auto reflectionResult = writer.declLocale( "reflectionResult"
						, mix( reflected, ssrResult.xyz(), ssrResult.www() ) );
					output.registerOutput( "CombinedReflection", reflectionResult );


					// Wobbly refractions
					auto heightFactor = writer.declLocale( "heightFactor"
						, c3d_oceanData.refractionHeightFactor * ( c3d_cameraData.farPlane() - c3d_cameraData.nearPlane() ) );
					auto distanceFactor = writer.declLocale( "distanceFactor"
						, c3d_oceanData.refractionDistanceFactor * ( c3d_cameraData.farPlane() - c3d_cameraData.nearPlane() ) );
					auto distortedTexCoord = writer.declLocale( "distortedTexCoord"
						, fma( vec2( ( finalNormal.xz() + finalNormal.xy() ) * 0.5_f )
							, vec2( c3d_oceanData.refractionDistortionFactor
								* utils.saturate( length( scenePosition - lightSurface.worldPosition() ) * 0.5_f ) )
							, hdrCoords ) );
					auto distortedDepth = writer.declLocale( "distortedDepth"
						, c3d_sceneDepthObj.sample( distortedTexCoord ).r() );
					auto distortedPosition = writer.declLocale( "distortedPosition"
						, c3d_cameraData.curProjToWorld( utils, distortedTexCoord, distortedDepth ) );
					auto refractionTexCoord = writer.declLocale( "refractionTexCoord"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedTexCoord, hdrCoords ) );
					auto refractionResult = writer.declLocale( "refractionResult"
						, c3d_sceneColour.sample( distortedTexCoord ).xyz() );
					output.registerOutput( "RawRefraction", refractionResult );
					auto lightAbsorbtion = writer.declLocale( "lightAbsorbtion"
						, getLightAbsorbtion( in.prvPosition.xy()
							, in.curPosition.xyz()
							, distortedPosition
							, refractionResult ) );
					output.registerOutput( "LightAbsorbtion", lightAbsorbtion );
					auto waterTransmission = writer.declLocale( "waterTransmission"
						, components.colour * lightAbsorbtion * ( indirectAmbient + indirectDiffuse ) );
					output.registerOutput( "WaterTransmission", waterTransmission );
					refractionResult *= waterTransmission;
					output.registerOutput( "RefractionResult", refractionResult );
					// Depth softening, to fade the alpha of the water where it meets the scene geometry by some predetermined distance. 
					auto depthSoftenedAlpha = writer.declLocale( "depthSoftenedAlpha"
						, clamp( distance( scenePosition, in.worldPosition.xyz() ) / c3d_oceanData.depthSofteningDistance, 0.0_f, 1.0_f ) );
					output.registerOutput( "DepthSoftenedAlpha", depthSoftenedAlpha );
					auto waterSurfacePosition = writer.declLocale( "waterSurfacePosition"
						, writer.ternary( distortedPosition.y() < in.worldPosition.y(), distortedPosition, scenePosition ) );
					refractionResult = mix( refractionResult
						, waterTransmission
						, vec3( clamp( ( in.worldPosition.y() - waterSurfacePosition.y() ) / heightFactor, 0.0_f, 1.0_f ) ) );
					output.registerOutput( "HeightMixedRefraction", refractionResult );
					refractionResult = mix( refractionResult
						, waterTransmission
						, utils.saturate( vec3( utils.saturate( length( in.viewPosition.xyz() ) / distanceFactor ) ) ) );
					output.registerOutput( "DistanceMixedRefraction", refractionResult );


					//Combine all that
					auto fresnelFactor = writer.declLocale( "fresnelFactor"
						, vec3( utils.fresnelMix( reflections.computeIncident( lightSurface.worldPosition(), c3d_cameraData.position() )
							, nml
							, c3d_oceanData.refractionRatio ) ) );
					output.registerOutput( "FresnelFactor", fresnelFactor );
					reflectionResult *= fresnelFactor;
					output.registerOutput( "FinalReflection", reflectionResult );
					refractionResult *= vec3( 1.0_f ) - fresnelFactor;
					output.registerOutput( "FinalRefraction", refractionResult );
					outColour = vec4( lighting.specular + lightIndirectSpecular
							+ components.emissiveColour * components.emissiveFactor
							+ refractionResult * colorMod
							+ ( reflectionResult * colorMod * indirectAmbient )
							+ lighting.scattering
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
	}
}

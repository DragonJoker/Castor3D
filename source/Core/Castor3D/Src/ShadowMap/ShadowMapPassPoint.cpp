#include "ShadowMapPassPoint.hpp"

#include "Mesh/Submesh.hpp"
#include "Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Shader/Program.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "Texture/TextureView.hpp"

#include <Graphics/Image.hpp>

#include <RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void doUpdateShadowMatrices( Point3r const & position
			, std::array< Matrix4x4r, size_t( CubeMapFace::eCount ) > & matrices )
		{
			matrices =
			{
				{
					matrix::lookAt( position, position + Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Positive X
					matrix::lookAt( position, position + Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 } ),// Negative X
					matrix::lookAt( position, position + Point3r{ +0, +1, +0 }, Point3r{ +0, +0, +1 } ),// Positive Y
					matrix::lookAt( position, position + Point3r{ +0, -1, +0 }, Point3r{ +0, +0, -1 } ),// Negative Y
					matrix::lookAt( position, position + Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 } ),// Positive Z
					matrix::lookAt( position, position + Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ),// Negative Z
				}
			};
		}
	}

	String const ShadowMapPassPoint::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassPoint::WorldLightPosition = cuT( "c3d_worldLightPosition" );
	String const ShadowMapPassPoint::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
	{
		m_renderQueue.initialise( scene );
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	void ShadowMapPassPoint::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		auto position = light.getParent()->getDerivedPosition();
		light.updateShadow( position
			, m_viewport
			, index );
		doUpdateShadowMatrices( position, m_matrices );
		auto & config = m_shadowConfig->getData();
		config.worldLightPosition = position;
		config.farPlane = m_viewport.getFar();
		doUpdate( queues );
	}

	void ShadowMapPassPoint::updateDeviceDependent( uint32_t index )
	{
		if ( m_initialised )
		{
			m_shadowConfig->upload();
			m_matrixUbo.update( m_matrices[index], m_projection );
			doUpdateNodes( m_renderQueue.getAllRenderNodes() );
		}
	}

	void ShadowMapPassPoint::doUpdateNodes( SceneRenderNodes & nodes )
	{
		RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled );
		RenderPass::doUpdate( nodes.staticNodes.backCulled );
		RenderPass::doUpdate( nodes.skinnedNodes.backCulled );
		RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled );
		RenderPass::doUpdate( nodes.morphingNodes.backCulled );
		RenderPass::doUpdate( nodes.billboardNodes.backCulled );
	}

	bool ShadowMapPassPoint::doInitialise( Size const & size )
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		real const aspect = real( size.getWidth() ) / size.getHeight();
		real const near = 1.0_r;
		real const far = 2000.0_r;
		matrix::perspective( m_projection, 90.0_degrees, aspect, near, far );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 3u );
		renderPass.attachments[0].index = 0u;
		renderPass.attachments[0].format = ShadowMapPoint::RawDepthFormat;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].index = 1u;
		renderPass.attachments[1].format = ShadowMapPoint::LinearDepthFormat;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.attachments[2].index = 2u;
		renderPass.attachments[2].format = ShadowMapPoint::VarianceFormat;
		renderPass.attachments[2].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[2].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[2].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[2].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[2].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[2].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[2].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 1u, renderer::ImageLayout::eColourAttachmentOptimal } );
		renderPass.subpasses[0].colorAttachments.push_back( { 2u, renderer::ImageLayout::eColourAttachmentOptimal } );
		renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		m_shadowConfig = renderer::makeUniformBuffer< Configuration >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent );

		m_viewport.resize( size );
		return true;
	}

	void ShadowMapPassPoint::doCleanup()
	{
		m_matrixUbo.cleanup();
		m_shadowConfig.reset();
		m_onNodeChanged.disconnect();
	}

	void ShadowMapPassPoint::doFillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.descriptorSet->createBinding( layout.getBinding( ShadowMapPassPoint::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassPoint::doFillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.descriptorSet->createBinding( layout.getBinding( ShadowMapPassPoint::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassPoint::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void ShadowMapPassPoint::doPreparePipeline( ShaderProgramSPtr program
		, renderer::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eNone;
			renderer::DepthStencilState dsState;
			auto bdState = renderer::ColourBlendState::createDefault();
			auto & pipeline = *m_backPipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, std::move( bdState )
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;
			pipeline.setVertexLayouts( layouts );

			auto initialise = [this, &pipeline, flags]()
			{
				auto uboBindings = doCreateUboBindings( flags );
				uboBindings.emplace_back( ShadowMapPassPoint::UboBindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
				auto layout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( uboBindings ) );
				std::vector< renderer::DescriptorSetLayoutPtr > layouts;
				layouts.emplace_back( std::move( layout ) );
				pipeline.setDescriptorSetLayouts( std::move( layouts ) );
				pipeline.initialise( getRenderPass(), renderer::PrimitiveTopology::eTriangleList );
				m_initialised = true;
			};

			if ( getEngine()->getRenderSystem()->hasCurrentDevice() )
			{
				initialise();
			}
			else
			{
				getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, initialise ) );
			}
		}
	}
}

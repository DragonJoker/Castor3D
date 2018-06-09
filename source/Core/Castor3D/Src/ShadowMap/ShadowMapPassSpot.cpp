#include "ShadowMapPassSpot.hpp"

#include "Render/RenderPipeline.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "ShadowMap/ShadowMapSpot.hpp"
#include "Shader/Program.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Technique/RenderTechniquePass.hpp"

#include <RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const ShadowMapPassSpot::UboBindingPoint = 10u;
	uint32_t const ShadowMapPassSpot::TextureSize = 512;
	String const ShadowMapPassSpot::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassSpot::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassSpot::ShadowMapPassSpot( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
	{
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	void ShadowMapPassSpot::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		light.updateShadow( light.getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, index );
		m_camera->attachTo( light.getParent() );
		m_camera->update();
		auto & data = m_shadowConfig->getData();
		data.farPlane = light.getSpotLight()->getFarPlane();
		queues.emplace_back( m_renderQueue );
	}

	void ShadowMapPassSpot::updateDeviceDependent( uint32_t index )
	{
		if ( m_camera && m_initialised )
		{
			m_shadowConfig->upload();
			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doUpdateNodes( m_renderQueue.getCulledRenderNodes(), *m_camera );
		}
	}

	bool ShadowMapPassSpot::doInitialise( Size const & size )
	{
		auto & device = getCurrentDevice( *this );
		Viewport viewport{ *getEngine() };
		m_camera = std::make_shared< Camera >( cuT( "ShadowMapSpot" )
			, m_scene
			, m_scene.getCameraRootNode()
			, std::move( viewport ) );
		m_camera->resize( size );


		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 3u );
		renderPass.attachments[0].format = ShadowMapSpot::RawDepthFormat;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = ShadowMapSpot::LinearDepthFormat;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.attachments[2].format = ShadowMapSpot::VarianceFormat;
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
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		m_shadowConfig = renderer::makeUniformBuffer< Configuration >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent );

		m_renderQueue.initialise( m_scene, *m_camera );
		return true;
	}

	void ShadowMapPassSpot::doCleanup()
	{
		m_renderQueue.cleanup();
		m_shadowConfig.reset();
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassSpot::doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( ShadowMapPassSpot::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassSpot::doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( ShadowMapPassSpot::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassSpot::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	renderer::DescriptorSetLayoutBindingArray ShadowMapPassSpot::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = RenderPass::doCreateUboBindings( flags );
		uboBindings.emplace_back( ShadowMapPassSpot::UboBindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
		m_initialised = true;
		return uboBindings;
	}

	renderer::DepthStencilState ShadowMapPassSpot::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return renderer::DepthStencilState{ 0u, true, true };
	}

	renderer::ColourBlendState ShadowMapPassSpot::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 2u );
	}
}

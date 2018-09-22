#include "ShadowMapPassDirectional.hpp"

#include "Cache/MaterialCache.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Shader/Program.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "ShadowMap/ShadowMapDirectional.hpp"
#include "Technique/RenderTechniquePass.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"
#include "Texture/TextureView.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const ShadowMapPassDirectional::UboBindingPoint = 10u;
	uint32_t const ShadowMapPassDirectional::TextureSize = 2048;
	String const ShadowMapPassDirectional::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassDirectional::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & engine
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, uint32_t cascadeIndex )
		: ShadowMapPass{ engine, matrixUbo, culler, shadowMap }
	{
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
	}

	void ShadowMapPassDirectional::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		auto cameraNode = camera.getParent();
		auto lightNode = light.getParent();
		auto & myCamera = getCuller().getCamera();
		light.getDirectionalLight()->updateShadow( camera
			, myCamera
			, index
			, getCuller().getMinCastersZ() );
		m_farPlane = std::abs( light.getDirectionalLight()->getSplitDepth( index ) );
		doUpdate( queues );
	}

	void ShadowMapPassDirectional::updateDeviceDependent( uint32_t index )
	{
		if ( m_initialised )
		{
			auto & config = m_shadowConfig->getData();

			if ( m_farPlane != config.farPlane )
			{
				config.farPlane = m_farPlane;
				m_shadowConfig->upload();
			}

			auto & myCamera = getCuller().getCamera();
			m_matrixUbo.update( myCamera.getView()
				, myCamera.getProjection() );
			doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
		}
	}

	bool ShadowMapPassDirectional::doInitialise( Size const & size )
	{
		auto & device = getCurrentDevice( *this );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 3u );
		renderPass.attachments[0].format = ShadowMapDirectional::RawDepthFormat;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = ShadowMapDirectional::LinearDepthFormat;
		renderPass.attachments[1].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.attachments[2].format = ShadowMapDirectional::VarianceFormat;
		renderPass.attachments[2].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[2].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[2].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[2].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[2].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[2].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[2].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 1u, ashes::ImageLayout::eColourAttachmentOptimal } );
		renderPass.subpasses[0].colorAttachments.push_back( { 2u, ashes::ImageLayout::eColourAttachmentOptimal } );
		renderPass.subpasses[0].depthStencilAttachment = { 0u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		m_shadowConfig = ashes::makeUniformBuffer< Configuration >( device
			, 1u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible | ashes::MemoryPropertyFlag::eHostCoherent );

		return true;
	}

	void ShadowMapPassDirectional::doCleanup()
	{
		m_renderQueue.cleanup();
		m_shadowConfig.reset();
		getCuller().getCamera().detach();
	}

	void ShadowMapPassDirectional::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( ShadowMapPassDirectional::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassDirectional::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( ShadowMapPassDirectional::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassDirectional::doUpdate( RenderQueueArray & queues )
	{
		getCuller().compute();
		queues.emplace_back( m_renderQueue );
	}

	ashes::DescriptorSetLayoutBindingArray ShadowMapPassDirectional::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = RenderPass::doCreateUboBindings( flags );
		uboBindings.emplace_back( ShadowMapPassDirectional::UboBindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment );
		m_initialised = true;
		return uboBindings;
	}

	ashes::DepthStencilState ShadowMapPassDirectional::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::DepthStencilState{ 0u, true, true };
	}

	ashes::ColourBlendState ShadowMapPassDirectional::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 2u );
	}
}

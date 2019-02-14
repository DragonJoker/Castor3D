#include "ShadowMapPassPoint.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Shader/Program.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "Technique/RenderTechniquePass.hpp"
#include "Texture/TextureView.hpp"

#include <Ashes/Buffer/VertexBuffer.hpp>

#include <Graphics/Image.hpp>

#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>

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

	uint32_t const ShadowMapPassPoint::TextureSize = 1024;
	uint32_t const ShadowMapPassPoint::UboBindingPoint = 10u;
	String const ShadowMapPassPoint::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassPoint::WorldLightPosition = cuT( "c3d_worldLightPosition" );
	String const ShadowMapPassPoint::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & engine
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, matrixUbo, culler, shadowMap }
		, m_viewport{ engine }
	{
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
		light.getPointLight()->updateShadow( m_viewport
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
			doUpdateNodes( m_renderQueue.getCulledRenderNodes() );
		}
	}

	void ShadowMapPassPoint::doUpdateNodes( SceneCulledRenderNodes & nodes )
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
		auto & device = getCurrentDevice( *this );
		real const aspect = real( size.getWidth() ) / size.getHeight();
		real const near = 1.0_r;
		real const far = 2000.0_r;
		m_projection = getEngine()->getRenderSystem()->getPerspective( 90.0_degrees, aspect, near, far );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 3u );
		renderPass.attachments[0].format = ShadowMapPoint::RawDepthFormat;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = ShadowMapPoint::LinearDepthFormat;
		renderPass.attachments[1].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.attachments[2].format = ShadowMapPoint::VarianceFormat;
		renderPass.attachments[2].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[2].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[2].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[2].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[2].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[2].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[2].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

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

		m_viewport.resize( size );
		m_initialised = true;
		return m_initialised;
	}

	void ShadowMapPassPoint::doCleanup()
	{
		m_renderQueue.cleanup();
		m_shadowConfig.reset();
		m_onNodeChanged.disconnect();
	}

	void ShadowMapPassPoint::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( ShadowMapPassPoint::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassPoint::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( ShadowMapPassPoint::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassPoint::doUpdate( RenderQueueArray & queues )
	{
		getCuller().compute();
		queues.emplace_back( m_renderQueue );
	}

	ashes::DescriptorSetLayoutBindingArray ShadowMapPassPoint::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = RenderPass::doCreateUboBindings( flags );
		uboBindings.emplace_back( ShadowMapPassPoint::UboBindingPoint, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment );
		m_initialised = true;
		return uboBindings;
	}

	ashes::DepthStencilState ShadowMapPassPoint::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::DepthStencilState{ 0u, true, true };
	}

	ashes::ColourBlendState ShadowMapPassPoint::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 2u );
	}
}

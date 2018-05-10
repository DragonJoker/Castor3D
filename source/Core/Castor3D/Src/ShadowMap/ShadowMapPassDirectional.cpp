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
	String const ShadowMapPassDirectional::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassDirectional::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
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
		light.updateShadow( light.getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, index );
		m_camera->attachTo( light.getParent() );
		m_camera->update();

		if ( light.getDirectionalLight()->getFarPlane() != m_farPlane )
		{
			m_farPlane = light.getDirectionalLight()->getFarPlane();
		}

		doUpdate( queues );
	}

	void ShadowMapPassDirectional::updateDeviceDependent( uint32_t index )
	{
		if ( m_camera && m_initialised )
		{
			auto & config = m_shadowConfig->getData();

			if ( m_farPlane != config.farPlane )
			{
				config.farPlane = m_farPlane;
				m_shadowConfig->upload();
			}

			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doUpdateNodes( m_renderQueue.getCulledRenderNodes(), *m_camera );
		}
	}

	bool ShadowMapPassDirectional::doInitialise( Size const & size )
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		Viewport viewport{ *getEngine() };
		auto w = float( size.getWidth() );
		auto h = float( size.getHeight() );
		viewport.setOrtho( -w / 2, w / 2, h / 2, -h / 2, -5120.0_r, 5120.0_r );
		viewport.update();
		m_camera = std::make_shared< Camera >( cuT( "ShadowMapDirectional" )
			, m_scene
			, m_scene.getCameraRootNode()
			, std::move( viewport ) );
		m_camera->resize( size );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 3u );
		renderPass.attachments[0].format = ShadowMapDirectional::RawDepthFormat;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = ShadowMapDirectional::LinearDepthFormat;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.attachments[2].format = ShadowMapDirectional::VarianceFormat;
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

	void ShadowMapPassDirectional::doCleanup()
	{
		m_renderQueue.cleanup();
		m_shadowConfig.reset();
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassDirectional::doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( ShadowMapPassDirectional::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassDirectional::doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createBinding( layout.getBinding( ShadowMapPassDirectional::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassDirectional::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void ShadowMapPassDirectional::doPreparePipeline( ShaderProgramSPtr program
		, renderer::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		auto & pipelines = doGetBackPipelines();

		if ( pipelines.find( flags ) == pipelines.end() )
		{
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eNone;
			renderer::DepthStencilState dsState;
			auto bdState = RenderTechniquePass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 2u );
			auto & pipeline = *pipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, std::move( bdState )
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;
			pipeline.setVertexLayouts( layouts );
			pipeline.setViewport( { m_camera->getWidth(), m_camera->getHeight(), 0, 0 } );
			pipeline.setScissor( { 0, 0, m_camera->getWidth(), m_camera->getHeight() } );

			auto initialise = [this, &pipeline, flags]()
			{
				auto uboBindings = doCreateUboBindings( flags );
				uboBindings.emplace_back( ShadowMapPassDirectional::UboBindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
				auto texBindings = doCreateTextureBindings( flags );
				auto uboLayout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( uboBindings ) );
				auto texLayout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( texBindings ) );
				std::vector< renderer::DescriptorSetLayoutPtr > layouts;
				layouts.emplace_back( std::move( uboLayout ) );
				layouts.emplace_back( std::move( texLayout ) );
				pipeline.setDescriptorSetLayouts( std::move( layouts ) );
				pipeline.initialise( getRenderPass() );
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

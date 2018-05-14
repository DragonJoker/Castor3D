#include "MeshLightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		renderer::RenderPassPtr doCreateRenderPass( Engine & engine
			, renderer::TextureView const & depthView
			, renderer::TextureView const & diffuseView
			, renderer::TextureView const & specularView
			, bool first )
		{
			auto & device = *engine.getRenderSystem()->getCurrentDevice();
			renderer::ImageLayout layout = first
				? renderer::ImageLayout::eUndefined
				: renderer::ImageLayout::eColourAttachmentOptimal;
			renderer::AttachmentLoadOp loadOp = first
				? renderer::AttachmentLoadOp::eClear
				: renderer::AttachmentLoadOp::eLoad;
			renderer::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 3u );
			renderPass.attachments[0].format = depthView.getFormat();
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

			renderPass.attachments[1].format = diffuseView.getFormat();
			renderPass.attachments[1].loadOp = loadOp;
			renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[1].initialLayout = layout;
			renderPass.attachments[1].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderPass.attachments[2].format = specularView.getFormat();
			renderPass.attachments[2].loadOp = loadOp;
			renderPass.attachments[2].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[2].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[2].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[2].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[2].initialLayout = layout;
			renderPass.attachments[2].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

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

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };
			renderPass.subpasses[0].colorAttachments =
			{
				{ 1u, renderer::ImageLayout::eColourAttachmentOptimal },
				{ 2u, renderer::ImageLayout::eColourAttachmentOptimal },
			};

			return device.createRenderPass( renderPass );
		}
	}

	//*********************************************************************************************

	MeshLightPass::Program::Program( Engine & engine
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl
		, bool hasShadows )
		: LightPass::Program{ engine, vtx, pxl, hasShadows }
	{
	}

	MeshLightPass::Program::~Program()
	{
	}

	renderer::PipelinePtr MeshLightPass::Program::doCreatePipeline( renderer::VertexLayout const & vertexLayout
		, renderer::RenderPass const & renderPass
		, bool blend )
	{
		renderer::DepthStencilState dsstate;
		dsstate.depthTestEnable = false;
		dsstate.depthWriteEnable = false;
		dsstate.stencilTestEnable = true;
		dsstate.back.compareOp = renderer::CompareOp::eNotEqual;
		dsstate.back.reference = 0u;

		renderer::RasterisationState rsstate;
		rsstate.cullMode = m_engine.isTopDown()
			? renderer::CullModeFlag::eBack
			: renderer::CullModeFlag::eFront;

		renderer::ColourBlendState blstate;

		if ( blend )
		{
			blstate.attachs.push_back( renderer::ColourBlendStateAttachment
			{
				true,
				renderer::BlendFactor::eOne,
				renderer::BlendFactor::eOne,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eOne,
				renderer::BlendFactor::eOne,
				renderer::BlendOp::eAdd,
			} );
		}
		else
		{
			blstate = renderer::ColourBlendState::createDefault();
		}

		blstate.attachs.push_back( blstate.attachs.back() );
		return m_pipelineLayout->createPipeline(
		{
			m_program,
			renderPass,
			renderer::VertexInputState::create( vertexLayout ),
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			std::move( rsstate ),
			renderer::MultisampleState{},
			std::move( blstate ),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			std::move( dsstate ),
		} );
	}

	//*********************************************************************************************

	MeshLightPass::MeshLightPass( Engine & engine
		, renderer::TextureView const & depthView
		, renderer::TextureView const & diffuseView
		, renderer::TextureView const & specularView
		, GpInfoUbo & gpInfoUbo
		, LightType type
		, bool hasShadows )
		: LightPass{ engine
			, doCreateRenderPass( engine, depthView, diffuseView, specularView, true )
			, doCreateRenderPass( engine, depthView, diffuseView, specularView, false )
			, depthView
			, diffuseView
			, specularView
			, gpInfoUbo
			, hasShadows }
		, m_modelMatrixUbo{ engine }
		, m_stencilPass{ engine, depthView, m_matrixUbo, m_modelMatrixUbo }
		, m_type{ type }
	{
	}

	MeshLightPass::~MeshLightPass()
	{
		m_vertexBuffer.reset();
	}

	void MeshLightPass::initialise( Scene const & scene
		, GeometryPassResult const & gp
		, SceneUbo & sceneUbo
		, RenderPassTimer & timer )
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

		auto declaration = renderer::makeLayout< Point3f >( 0u );
		declaration->createAttribute( 0u, renderer::Format::eR32G32B32_SFLOAT, 0u );

		auto data = doGenerateVertices();
		m_count = uint32_t( data.size() );
		m_vertexBuffer = renderer::makeVertexBuffer< float >( device
			, uint32_t( data.size() * 3u )
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );
		if ( auto * buffer = m_vertexBuffer->lock( 0u, m_vertexBuffer->getCount(), renderer::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( buffer, data.data()->constPtr(), m_vertexBuffer->getSize() );
			m_vertexBuffer->flush( 0u, m_vertexBuffer->getCount() );
			m_vertexBuffer->unlock();
		}

		m_stencilPass.initialise( *declaration, *m_vertexBuffer );

		doInitialise( scene
			, gp
			, m_type
			, *m_vertexBuffer
			, *declaration
			, sceneUbo
			, &m_modelMatrixUbo
			, timer );
	}

	void MeshLightPass::cleanup()
	{
		doCleanup();
		m_stencilPass.cleanup();
		m_vertexBuffer.reset();
		m_modelMatrixUbo.cleanup();
		m_matrixUbo.cleanup();
	}

	void MeshLightPass::render( bool first
		, renderer::Semaphore const & toWait
		, TextureUnit * shadowMapOpt )
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_stencilPass.render( toWait );
		LightPass::render( first, m_stencilPass.getSemaphore(), shadowMapOpt );
	}

	uint32_t MeshLightPass::getCount()const
	{
		return m_count;
	}

	void MeshLightPass::doUpdate( Size const & size
		, Light const & light
		, Camera const & camera )
	{
		auto model = doComputeModelMatrix( light, camera );
		m_matrixUbo.update( camera.getView(), camera.getViewport().getProjection() );
		m_modelMatrixUbo.update( model );
		m_pipeline->program->bind( light );
	}

	glsl::Shader MeshLightPass::doGetVertexShaderSource( SceneFlags const & sceneFlags )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto vertex = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );

		// Shader outputs
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_projection * c3d_curView * c3d_mtxModel * vec4( vertex, 1.0_f );
		} );

		return writer.finalise();
	}

	//*********************************************************************************************
}

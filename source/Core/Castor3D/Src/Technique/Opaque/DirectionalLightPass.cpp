#include "DirectionalLightPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Light/DirectionalLight.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static constexpr uint32_t VertexCount = 6u;

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
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
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

	DirectionalLightPass::Program::Program( Engine & engine
		, DirectionalLightPass & pass
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl
		, bool hasShadows )
		: LightPass::Program{ engine, vtx, pxl, hasShadows }
		, m_lightPass{ pass }
	{
	}

	DirectionalLightPass::Program::~Program()
	{
	}

	renderer::PipelinePtr DirectionalLightPass::Program::doCreatePipeline( renderer::VertexLayout const & vertexLayout
		, renderer::RenderPass const & renderPass
		, bool blend )
	{
		renderer::DepthStencilState dsstate
		{
			0u,
			false,
			false
		};
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
			renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eNone },
			renderer::MultisampleState{},
			std::move( blstate ),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			std::move( dsstate ),
		} );
	}

	void DirectionalLightPass::Program::doBind( Light const & light )
	{
		auto & directionalLight = *light.getDirectionalLight();
		auto & data = m_lightPass.m_ubo->getData( 0u );
		data.base.colourIndex = renderer::Vec4{ light.getColour()[0], light.getColour()[1], light.getColour()[2], 0.0f };
		data.base.intensityFarPlane = renderer::Vec4{ light.getIntensity()[0], light.getIntensity()[1], light.getFarPlane(), 0.0f };
		data.direction = renderer::Vec4{ directionalLight.getDirection()[0], directionalLight.getDirection()[1], directionalLight.getDirection()[2], 0.0f };
		data.transform = convert( directionalLight.getLightSpaceTransform() );
		m_lightPass.m_ubo->upload();
	}

	//*********************************************************************************************

	DirectionalLightPass::DirectionalLightPass( Engine & engine
		, renderer::TextureView const & depthView
		, renderer::TextureView const & diffuseView
		, renderer::TextureView const & specularView
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: LightPass{ engine
			, doCreateRenderPass( engine, depthView, diffuseView, specularView, true )
			, doCreateRenderPass( engine, depthView, diffuseView, specularView, false )
			, depthView
			, diffuseView
			, specularView
			, gpInfoUbo
			, hasShadows }
		, m_ubo{ renderer::makeUniformBuffer< Config >( *engine.getRenderSystem()->getCurrentDevice()
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible ) }
		, m_viewport{ engine }
	{
		m_baseUbo = &m_ubo->getUbo();
		m_viewport.setOrtho( -1, 1, -1, 1, -1, 1 );
	}

	void DirectionalLightPass::initialise( Scene const & scene
		, GeometryPassResult const & gp
		, SceneUbo & sceneUbo
		, RenderPassTimer & timer )
	{
		float data[] =
		{
			-1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, -1.0f,
			+1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f,
		};

		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_vertexBuffer = renderer::makeVertexBuffer< float >( device
			, 12u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto buffer = m_vertexBuffer->lock( 0u
			, m_vertexBuffer->getCount()
			, renderer::MemoryMapFlag::eInvalidateRange | renderer::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( buffer, data, sizeof( data ) );
			m_vertexBuffer->flush( 0u, m_vertexBuffer->getCount() );
			m_vertexBuffer->unlock();
		}

		m_vertexLayout = renderer::makeLayout< Point2f >( 0u );
		m_vertexLayout->createAttribute( 0u, renderer::Format::eR32G32_SFLOAT, 0u );
		doInitialise( scene
			, gp
			, LightType::eDirectional
			, *m_vertexBuffer
			, *m_vertexLayout
			, sceneUbo
			, nullptr
			, timer );
		m_viewport.update();
	}

	void DirectionalLightPass::cleanup()
	{
		doCleanup();
	}

	uint32_t DirectionalLightPass::getCount()const
	{
		return VertexCount;
	}

	void DirectionalLightPass::update( Size const & size
		, Light const & light
		, Camera const & camera )
	{
		m_viewport.resize( size );
		m_matrixUbo.update( camera.getView(), m_viewport.getProjection() );
		m_program->bind( light );
	}

	glsl::Shader DirectionalLightPass::doGetVertexShaderSource( SceneFlags const & sceneFlags )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

		// Shader outputs
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_projection * vec4( position, 0.0, 1.0 );
		} );

		return writer.finalise();
	}

	LightPass::ProgramPtr DirectionalLightPass::doCreateProgram( glsl::Shader const & vtx
		, glsl::Shader const & pxl )
	{
		return std::make_unique< Program >( m_engine, *this, vtx, pxl, m_shadows );
	}
}

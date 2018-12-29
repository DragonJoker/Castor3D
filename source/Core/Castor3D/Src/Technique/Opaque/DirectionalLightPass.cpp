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

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

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

		ashes::RenderPassPtr doCreateRenderPass( Engine & engine
			, ashes::TextureView const & depthView
			, ashes::TextureView const & diffuseView
			, ashes::TextureView const & specularView
			, bool first )
		{
			auto & device = getCurrentDevice( engine );
			ashes::ImageLayout layout = first
				? ashes::ImageLayout::eUndefined
				: ashes::ImageLayout::eShaderReadOnlyOptimal;
			ashes::AttachmentLoadOp loadOp = first
				? ashes::AttachmentLoadOp::eClear
				: ashes::AttachmentLoadOp::eLoad;
			ashes::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 3u );
			renderPass.attachments[0].format = depthView.getFormat();
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.attachments[1].format = diffuseView.getFormat();
			renderPass.attachments[1].loadOp = loadOp;
			renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[1].initialLayout = layout;
			renderPass.attachments[1].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.attachments[2].format = specularView.getFormat();
			renderPass.attachments[2].loadOp = loadOp;
			renderPass.attachments[2].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[2].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[2].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[2].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[2].initialLayout = layout;
			renderPass.attachments[2].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

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

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].depthStencilAttachment = { 0u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };
			renderPass.subpasses[0].colorAttachments = 
			{
				{ 1u, ashes::ImageLayout::eColourAttachmentOptimal },
				{ 2u, ashes::ImageLayout::eColourAttachmentOptimal },
			};

			return device.createRenderPass( renderPass );
		}
	}

	//*********************************************************************************************

	DirectionalLightPass::Program::Program( Engine & engine
		, DirectionalLightPass & pass
		, ShaderModule const & vtx
		, ShaderModule const & pxl
		, bool hasShadows )
		: LightPass::Program{ engine, vtx, pxl, hasShadows }
		, m_lightPass{ pass }
	{
	}

	DirectionalLightPass::Program::~Program()
	{
	}

	ashes::PipelinePtr DirectionalLightPass::Program::doCreatePipeline( ashes::VertexLayout const & vertexLayout
		, ashes::RenderPass const & renderPass
		, bool blend )
	{
		ashes::DepthStencilState dsstate
		{
			0u,
			false,
			false
		};
		ashes::ColourBlendState blstate;

		if ( blend )
		{
			blstate.attachs.push_back( ashes::ColourBlendStateAttachment
			{
				true,
				ashes::BlendFactor::eOne,
				ashes::BlendFactor::eOne,
				ashes::BlendOp::eAdd,
				ashes::BlendFactor::eOne,
				ashes::BlendFactor::eOne,
				ashes::BlendOp::eAdd,
			} );
		}
		else
		{
			blstate = ashes::ColourBlendState::createDefault();
		}

		blstate.attachs.push_back( blstate.attachs.back() );
		return m_pipelineLayout->createPipeline(
		{
			m_program,
			renderPass,
			ashes::VertexInputState::create( vertexLayout ),
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{ 0u, false, false, ashes::PolygonMode::eFill, ashes::CullModeFlag::eNone },
			ashes::MultisampleState{},
			std::move( blstate ),
			{ ashes::DynamicState::eViewport, ashes::DynamicState::eScissor },
			std::move( dsstate ),
		} );
	}

	void DirectionalLightPass::Program::doBind( Light const & light )
	{
		auto & directionalLight = *light.getDirectionalLight();
		auto & data = m_lightPass.m_ubo->getData( 0u );
		data.base.colourIndex = castor::Point4f{ light.getColour()[0], light.getColour()[1], light.getColour()[2], 0.0f };
		data.base.intensityFarPlane = castor::Point4f{ light.getIntensity()[0], light.getIntensity()[1], light.getFarPlane(), 0.0f };
		data.base.volumetric = castor::Point4f{ light.getVolumetricSteps(), light.getVolumetricScatteringFactor(), 0.0f, 0.0f };
		data.direction = castor::Point4f{ directionalLight.getDirection()[0]
			, directionalLight.getDirection()[1]
			, directionalLight.getDirection()[2]
			, float( light.getScene()->getDirectionalShadowCascades() ) };

		for ( auto i = 0u; i < light.getScene()->getDirectionalShadowCascades(); ++i )
		{
			data.transform[i] = directionalLight.getLightSpaceTransform( i );
			data.splitDepths[i] = directionalLight.getSplitDepth( i );
		}

		for ( auto i = light.getScene()->getDirectionalShadowCascades(); i < shader::DirectionalMaxCascadesCount; ++i )
		{
			data.transform[i] = Matrix4x4f{ 1.0f };
			data.splitDepths[i] = 0.0f;
		}

		m_lightPass.m_ubo->upload();
	}

	//*********************************************************************************************

	DirectionalLightPass::DirectionalLightPass( Engine & engine
		, ashes::TextureView const & depthView
		, ashes::TextureView const & diffuseView
		, ashes::TextureView const & specularView
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
		, m_ubo{ ashes::makeUniformBuffer< Config >( getCurrentDevice( engine )
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible ) }
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
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		m_vertexBuffer = ashes::makeVertexBuffer< float >( device
			, 12u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );

		if ( auto buffer = m_vertexBuffer->getBuffer().lock( 0u
			, ~( 0ull )
			, ashes::MemoryMapFlag::eInvalidateRange | ashes::MemoryMapFlag::eWrite ) )
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
			std::memcpy( buffer, data, sizeof( data ) );
			m_vertexBuffer->getBuffer().flush( 0u, ~( 0ull ) );
			m_vertexBuffer->getBuffer().unlock();
		}

		m_vertexLayout = ashes::makeLayout< Point2f >( 0u );
		m_vertexLayout->createAttribute( 0u, ashes::Format::eR32G32_SFLOAT, 0u );
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
		m_vertexLayout.reset();
		m_vertexBuffer.reset();
	}

	void DirectionalLightPass::accept( RenderTechniqueVisitor & visitor )
	{
		String name = cuT( "DirectionalLight" );

		if ( m_shadows )
		{
			name += cuT( " Shadow" );
		}

		visitor.visit( name
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
	}

	uint32_t DirectionalLightPass::getCount()const
	{
		return VertexCount;
	}

	void DirectionalLightPass::doUpdate( Size const & size
		, Light const & light
		, Camera const & camera )
	{
		m_viewport.resize( size );
		m_matrixUbo.update( camera.getView(), m_viewport.getProjection() );
		m_pipeline->program->bind( light );
	}

	ShaderPtr DirectionalLightPass::doGetVertexShaderSource( SceneFlags const & sceneFlags )const
	{
		using namespace sdw;
		VertexWriter writer;

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );

		// Shader outputs
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			out.gl_out.gl_Position = c3d_projection * vec4( position, 0.0, 1.0 );
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	LightPass::ProgramPtr DirectionalLightPass::doCreateProgram()
	{
		return std::make_unique< Program >( m_engine
			, *this
			, m_vertexShader
			, m_pixelShader
			, m_shadows );
	}
}

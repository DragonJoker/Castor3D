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

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
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
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;
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

	MeshLightPass::Program::Program( Engine & engine
		, ShaderModule const & vtx
		, ShaderModule const & pxl
		, bool hasShadows )
		: LightPass::Program{ engine, vtx, pxl, hasShadows }
	{
	}

	MeshLightPass::Program::~Program()
	{
	}

	ashes::PipelinePtr MeshLightPass::Program::doCreatePipeline( ashes::VertexLayout const & vertexLayout
		, ashes::RenderPass const & renderPass
		, bool blend )
	{
		ashes::DepthStencilState dsstate;
		dsstate.depthTestEnable = false;
		dsstate.depthWriteEnable = false;
		dsstate.stencilTestEnable = true;
		dsstate.back.compareOp = ashes::CompareOp::eNotEqual;
		dsstate.back.reference = 0u;

		ashes::RasterisationState rsstate;
		rsstate.cullMode = ashes::CullModeFlag::eBack;

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
			std::move( rsstate ),
			ashes::MultisampleState{},
			std::move( blstate ),
			{ ashes::DynamicState::eViewport, ashes::DynamicState::eScissor },
			std::move( dsstate ),
		} );
	}

	//*********************************************************************************************

	MeshLightPass::MeshLightPass( Engine & engine
		, ashes::TextureView const & depthView
		, ashes::TextureView const & diffuseView
		, ashes::TextureView const & specularView
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
		auto & device = getCurrentDevice( renderSystem );

		auto declaration = ashes::makeLayout< Point3f >( 0u );
		declaration->createAttribute( 0u, ashes::Format::eR32G32B32_SFLOAT, 0u );

		auto data = doGenerateVertices();
		m_count = uint32_t( data.size() );
		m_vertexBuffer = ashes::makeVertexBuffer< float >( device
			, uint32_t( data.size() * 3u )
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );
		if ( auto * buffer = m_vertexBuffer->lock( 0u, m_vertexBuffer->getCount(), ashes::MemoryMapFlag::eWrite ) )
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

	ashes::Semaphore const & MeshLightPass::render( uint32_t index
		, ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;
		result = &m_stencilPass.render( *result );
		result = &LightPass::render( index, *result );
		return *result;
	}

	uint32_t MeshLightPass::getCount()const
	{
		return m_count;
	}

	void MeshLightPass::doUpdate( bool first
		, Size const & size
		, Light const & light
		, Camera const & camera
		, ShadowMap const * shadowMap
		, uint32_t shadowMapIndex )
	{
		auto model = doComputeModelMatrix( light, camera );
		m_matrixUbo.update( camera.getView(), camera.getProjection() );
		m_modelMatrixUbo.update( model );
		m_pipeline->program->bind( light );
	}

	ShaderPtr MeshLightPass::doGetVertexShaderSource( SceneFlags const & sceneFlags )const
	{
		using namespace sdw;
		VertexWriter writer;

		// Shader inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0u );
		UBO_GPINFO( writer, GpInfoUbo::BindingPoint, 0u );
		auto vertex = writer.declInput< Vec3 >( cuT( "position" ), 0u );

		// Shader outputs
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			out.gl_out.gl_Position = c3d_projection * c3d_curView * c3d_curMtxModel * vec4( vertex, 1.0_f );
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	//*********************************************************************************************
}

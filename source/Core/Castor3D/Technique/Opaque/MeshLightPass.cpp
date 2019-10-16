#include "Castor3D/Technique/Opaque/MeshLightPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::RenderPassPtr doCreateRenderPass( Engine & engine
			, ashes::ImageView const & depthView
			, ashes::ImageView const & diffuseView
			, ashes::ImageView const & specularView
			, bool first )
		{
			auto & device = getCurrentRenderDevice( engine );
			VkImageLayout layout = first
				? VK_IMAGE_LAYOUT_UNDEFINED
				: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkAttachmentLoadOp loadOp = first
				? VK_ATTACHMENT_LOAD_OP_CLEAR
				: VK_ATTACHMENT_LOAD_OP_LOAD;

			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					depthView.getFormat(),
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				},
				{
					0u,
					diffuseView.getFormat(),
					VK_SAMPLE_COUNT_1_BIT,
					loadOp,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					layout,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				},
				{
					0u,
					specularView.getFormat(),
					VK_SAMPLE_COUNT_1_BIT,
					loadOp,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					layout,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				},
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{
						{ 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
						{ 2u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
					},
					{},
					VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				}
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( std::move( createInfo ) );
			return result;
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

	ashes::GraphicsPipelinePtr MeshLightPass::Program::doCreatePipeline( ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
		, ashes::RenderPass const & renderPass
		, bool blend )
	{
		ashes::PipelineDepthStencilStateCreateInfo dsstate
		{
			0u,
			VK_FALSE,
			VK_FALSE,
			VK_COMPARE_OP_LESS,
			VK_FALSE,
			VK_FALSE,
			{
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_COMPARE_OP_NOT_EQUAL,
				0xFFFFFFFFu,
				0xFFFFFFFFu,
				0x0u,
			},
		};

		ashes::VkPipelineColorBlendAttachmentStateArray blattaches;

		if ( blend )
		{
			blattaches.push_back( VkPipelineColorBlendAttachmentState
			{
				true,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_OP_ADD,
				defaultColorWriteMask,
			} );
		}
		else
		{
			blattaches.push_back( VkPipelineColorBlendAttachmentState
				{
					true,
					VK_BLEND_FACTOR_ONE,
					VK_BLEND_FACTOR_ZERO,
					VK_BLEND_OP_ADD,
					VK_BLEND_FACTOR_ONE,
					VK_BLEND_FACTOR_ZERO,
					VK_BLEND_OP_ADD,
					defaultColorWriteMask,
				} );
		}

		blattaches.push_back( blattaches.back() );
		auto & device = getCurrentRenderDevice( m_engine );
		return device->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				0u,
				m_program,
				vertexLayout,
				ashes::PipelineInputAssemblyStateCreateInfo{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
				std::nullopt,
				ashes::PipelineViewportStateCreateInfo{},
				ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT },
				ashes::PipelineMultisampleStateCreateInfo{},
				std::move( dsstate ),
				ashes::PipelineColorBlendStateCreateInfo{ 0u, VK_FALSE, VK_LOGIC_OP_COPY, std::move( blattaches ) },
				ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
				*m_pipelineLayout,
				renderPass,
			} );
	}

	//*********************************************************************************************

	MeshLightPass::MeshLightPass( Engine & engine
		, ashes::ImageView const & depthView
		, ashes::ImageView const & diffuseView
		, ashes::ImageView const & specularView
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
		auto & device = getCurrentRenderDevice( renderSystem );

		ashes::PipelineVertexInputStateCreateInfo declaration
		{
			0u,
			ashes::VkVertexInputBindingDescriptionArray
			{
				{ 0u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX },
			},
			ashes::VkVertexInputAttributeDescriptionArray
			{
				{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, 0u },
			}
		};

		auto data = doGenerateVertices();
		m_count = uint32_t( data.size() );
		m_vertexBuffer = makeVertexBuffer< float >( device
			, uint32_t( data.size() * 3u )
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "MeshLightPassVbo" );

		if ( auto * buffer = m_vertexBuffer->lock( 0u, m_vertexBuffer->getCount(), 0u ) )
		{
			std::memcpy( buffer, data.data()->constPtr(), m_vertexBuffer->getSize() );
			m_vertexBuffer->flush( 0u, m_vertexBuffer->getCount() );
			m_vertexBuffer->unlock();
		}

		m_stencilPass.initialise( declaration, *m_vertexBuffer );

		doInitialise( scene
			, gp
			, m_type
			, *m_vertexBuffer
			, declaration
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

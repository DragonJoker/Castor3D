#include "Castor3D/Render/ShadowMap/ShadowMapPassPoint.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void doUpdateShadowMatrices( castor::Point3f const & position
			, std::array< castor::Matrix4x4f, size_t( CubeMapFace::eCount ) > & matrices )
		{
			matrices =
			{
				{
					castor::matrix::lookAt( position, position + castor::Point3f{ +1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ),// Positive X
					castor::matrix::lookAt( position, position + castor::Point3f{ -1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ),// Negative X
					castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, +1.0f } ),// Positive Y
					castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, -1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, -1.0f } ),// Negative Y
					castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +0.0f, +1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ),// Positive Z
					castor::matrix::lookAt( position, position + castor::Point3f{ +0.0f, +0.0f, -1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ),// Negative Z
				}
			};
		}
	}

	uint32_t const ShadowMapPassPoint::TextureSize = 512u;
	uint32_t const ShadowMapPassPoint::UboBindingPoint = 10u;
	String const ShadowMapPassPoint::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassPoint::WorldLightPosition = cuT( "c3d_worldLightPosition" );

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ cuT( "ShadowMapPassPoint" ), engine, matrixUbo, culler, shadowMap }
		, m_viewport{ engine }
	{
		log::trace << "Created ShadowMapPassPoint" << std::endl;
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	bool ShadowMapPassPoint::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		getCuller().compute();
		m_outOfDate = m_outOfDate
			|| getCuller().areAllChanged()
			|| getCuller().areCulledChanged();
		auto position = light.getParent()->getDerivedPosition();
		light.getPointLight()->updateShadow( m_viewport
			, index );
		doUpdateShadowMatrices( position, m_matrices );
		auto & config = m_shadowConfig->getData();
		config.worldLightPosition = position;
		config.farPlane = m_viewport.getFar();
		doUpdate( queues );
		return m_outOfDate;
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
		auto & device = getCurrentRenderDevice( *this );
		float const aspect = float( size.getWidth() ) / size.getHeight();
		float const nearZ = 1.0f;
		float const farZ = 2000.0f;
		m_projection = getEngine()->getRenderSystem()->getPerspective( 90.0_degrees, aspect, nearZ, farZ );

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				ShadowMapPoint::RawDepthFormat,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
			{
				0u,
				ShadowMapPoint::LinearDepthFormat,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
			{
				0u,
				ShadowMapPoint::VarianceFormat,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
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
		m_renderPass = device->createRenderPass( std::move( createInfo ) );
		setDebugObjectName( device
			, *m_renderPass
			, "ShadowMapPassPoint_Pass" );

		m_shadowConfig = makeUniformBuffer< Configuration >( *getEngine()->getRenderSystem()
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, "ShadowMapPassPoint_ShadowConfigUbo" );

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
		node.uboDescriptorSet->createSizedBinding( layout.getBinding( ShadowMapPassPoint::UboBindingPoint )
			, m_shadowConfig->getBuffer() );
	}

	void ShadowMapPassPoint::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
		node.uboDescriptorSet->createSizedBinding( layout.getBinding( ShadowMapPassPoint::UboBindingPoint )
			, m_shadowConfig->getBuffer() );
	}

	void ShadowMapPassPoint::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	ashes::VkDescriptorSetLayoutBindingArray ShadowMapPassPoint::doCreateUboBindings( PipelineFlags const & flags )const
	{
		auto uboBindings = RenderPass::doCreateUboBindings( flags );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( ShadowMapPassPoint::UboBindingPoint
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
		return uboBindings;
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassPoint::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassPoint::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 2u );
	}
}

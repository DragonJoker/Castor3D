#include "Castor3D/Render/Opaque/Lighting/LightPipeline.hpp"

#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

namespace castor3d
{
	//*********************************************************************************************

	LightPipelineConfig::LightPipelineConfig( LightingModelID lightingModelId
		, SceneFlags const & sceneFlags
		, Light const & light )
		: lightingModelId{ lightingModelId }
		, sceneFlags{ sceneFlags }
		, lightType{ light.getLightType() }
		, shadowType{ light.getShadowType() }
		, shadows{ shadowType != ShadowType::eNone }
	{
	}

	size_t LightPipelineConfig::makeHash()const
	{
		size_t hash = std::hash< LightingModelID >{}( lightingModelId );
		hash = castor::hashCombine( hash, size_t( sceneFlags ) );
		hash = castor::hashCombine( hash, size_t( lightType ) );
		hash = castor::hashCombine( hash, size_t( shadowType ) );
		hash = castor::hashCombine( hash, shadows );
		return hash;
	}

	//*********************************************************************************************

	LightDescriptors::LightDescriptors( Light const & plight
		, RenderDevice const & device )
		: light{ plight }
		, matrixUbo{ device }
	{
	}

	//*********************************************************************************************

	uint32_t getLightRenderPassIndex( bool blend
		, LightType lightType )
	{
		if ( blend )
		{
			if ( lightType != LightType::eDirectional )
			{
				return 3u;
			}

			return 2u;
		}

		if ( lightType != LightType::eDirectional )
		{
			return 1u;
		}

		return 0u;
	}

	//*********************************************************************************************

	LightPipeline::LightPipeline( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, LightPipelineConfig const & config
		, std::vector< LightRenderPass > const & renderPasses
		, ashes::PipelineShaderStageCreateInfoArray stages
		, VkDescriptorSetLayout descriptorSetLayout )
		: m_holder{ pass
			, context
			, graph
			, crg::pp::Config{ std::vector< crg::VkPipelineShaderStageCreateInfoArray >{ crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages )
					, crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) }
				, crg::defaultV< crg::ProgramCreator >
				, std::vector< VkDescriptorSetLayout >{ descriptorSetLayout } }
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, 2u }
		, m_config{ config }
		, m_renderPasses{ renderPasses }
	{
		m_holder.initialise();
		doCreatePipeline();
	}

	void LightPipeline::doCreatePipeline()
	{
		ashes::PipelineVertexInputStateCreateInfo vertexLayout = doCreateVertexLayout();
		crg::VkVertexInputAttributeDescriptionArray vertexAttribs;
		crg::VkVertexInputBindingDescriptionArray vertexBindings;
		ashes::PipelineDepthStencilStateCreateInfo depthStencil{ 0u
			, VK_FALSE
			, VK_FALSE
			, VK_COMPARE_OP_LESS
			, VK_FALSE
			, ( getLightType() == LightType::eDirectional
				? VK_FALSE
				: VK_TRUE )
			, ( getLightType() == LightType::eDirectional
				? VkStencilOpState{}
				: VkStencilOpState{ VK_STENCIL_OP_KEEP
					, VK_STENCIL_OP_KEEP
					, VK_STENCIL_OP_KEEP
					, VK_COMPARE_OP_NOT_EQUAL
					, 0xFFFFFFFFu
					, 0xFFFFFFFFu
					, 0x0u } )
			, ( getLightType() == LightType::eDirectional
				? VkStencilOpState{}
				: VkStencilOpState{ VK_STENCIL_OP_KEEP
					, VK_STENCIL_OP_KEEP
					, VK_STENCIL_OP_KEEP
					, VK_COMPARE_OP_NOT_EQUAL
					, 0xFFFFFFFFu
					, 0xFFFFFFFFu
					, 0x0u } ) };
		auto iaState = makeVkStruct< VkPipelineInputAssemblyStateCreateInfo >( 0u
			, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			, VK_FALSE );
		auto msState = makeVkStruct< VkPipelineMultisampleStateCreateInfo >( 0u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_FALSE
			, 0.0f
			, nullptr
			, VK_FALSE
			, VK_FALSE );
		auto rsState = makeVkStruct< VkPipelineRasterizationStateCreateInfo >( 0u
			, VK_FALSE
			, VK_FALSE
			, VK_POLYGON_MODE_FILL
			, doGetCullMode()
			, VK_FRONT_FACE_COUNTER_CLOCKWISE
			, VK_FALSE
			, 0.0f
			, 0.0f
			, 0.0f
			, 0.0f );
		VkPipelineVertexInputStateCreateInfo const & vsState = vertexLayout;
		VkPipelineDepthStencilStateCreateInfo const & dsState = depthStencil;
		auto nameBase = m_holder.getPass().getGroupName() + "/Lighting";

		for ( uint32_t index = 0u; index < 2u; ++index )
		{
			auto rpIndex = getLightRenderPassIndex( index != 0u, getLightType() );
			auto viewportState = doCreateViewportState( *m_renderPasses[rpIndex].framebuffer );
			auto colourBlend = doCreateBlendState( index != 0u );
			auto & program = m_holder.getProgram( index );
			VkPipelineColorBlendStateCreateInfo const & cbState = colourBlend;
			VkPipelineViewportStateCreateInfo const & vpState = viewportState;
			auto createInfo = makeVkStruct< VkGraphicsPipelineCreateInfo >( 0u
				, uint32_t( program.size() )
				, program.data()
				, &vsState
				, &iaState
				, nullptr
				, &vpState
				, &rsState
				, &msState
				, &dsState
				, &cbState
				, nullptr
				, m_holder.getPipelineLayout()
				, *m_renderPasses[rpIndex].renderPass
				, 0u
				, VK_NULL_HANDLE
				, 0 );
			auto name = nameBase + ( index ? std::string{ "/Blend" } : std::string{ "/First" } );
			m_holder.createPipeline( index, name, createInfo );
		}
	}

	ashes::PipelineVertexInputStateCreateInfo LightPipeline::doCreateVertexLayout()
	{
		if ( getLightType() == LightType::eDirectional )
		{
			return { 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 0u
					, sizeof( castor::Point2f )
					, VK_VERTEX_INPUT_RATE_VERTEX } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
					, 0u
					, VK_FORMAT_R32G32_SFLOAT
					, 0u } } };
		}

		return { 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( castor::Point3f )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
				, 0u
				, VK_FORMAT_R32G32B32_SFLOAT
				, 0u } } };
	}

	ashes::PipelineViewportStateCreateInfo LightPipeline::doCreateViewportState( ashes::FrameBuffer const & framebuffer )
	{
		auto dim = framebuffer.getDimensions();
		ashes::VkViewportArray viewports{ { 0.0f
			, 0.0f
			, float( dim.width )
			, float( dim.height )
			, 0.0f
			, 1.0f } };
		ashes::VkScissorArray scissors{ { {}, dim } };
		return { 0u
			, 1u
			, viewports
			, 1u
			, scissors };
	}

	ashes::PipelineColorBlendStateCreateInfo LightPipeline::doCreateBlendState( bool blend )
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachs;

		if ( blend )
		{
			attachs.push_back( { VK_TRUE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_OP_ADD
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_OP_ADD
				, defaultColorWriteMask } );
		}
		else
		{
			attachs.push_back( { VK_FALSE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ZERO
				, VK_BLEND_OP_ADD
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ZERO
				, VK_BLEND_OP_ADD
				, defaultColorWriteMask } );
		}

		attachs.push_back( attachs.back() );
		attachs.push_back( attachs.back() );
		attachs.push_back( attachs.back() );
		attachs.push_back( attachs.back() );

		return { 0u
			, VK_FALSE
			, VK_LOGIC_OP_COPY
			, std::move( attachs ) };
	}

	VkCullModeFlags LightPipeline::doGetCullMode()const
	{
		if ( getLightType() == LightType::eDirectional )
		{
			return VK_CULL_MODE_NONE;
		}

		return VK_CULL_MODE_FRONT_BIT;
	}

	//*********************************************************************************************
}

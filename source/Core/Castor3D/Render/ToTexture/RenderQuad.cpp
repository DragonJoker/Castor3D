#include "Castor3D/Render/ToTexture/RenderQuad.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <ShaderWriter/Source.hpp>

#include <algorithm>

using namespace castor;

namespace castor3d
{
	namespace
	{
		uint32_t doGetColourAttachmentCount( ashes::RenderPass const & pass )
		{
			return uint32_t( std::count_if( pass.getAttachments().begin()
				, pass.getAttachments().end()
				, []( VkAttachmentDescription const & lookup )
				{
					return !ashes::isDepthOrStencilFormat( lookup.format );
				} ) );
		}

		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( ashes::RenderPass const & pass
			, BlendMode blendMode )
		{
			return RenderPass::createBlendState( blendMode
				, blendMode
				, doGetColourAttachmentCount( pass ) );
		}
	}

	RenderQuad::RenderQuad( RenderSystem & renderSystem
		, castor::String const & name
		, VkFilter samplerFilter
		, RenderQuadConfig config )
		: castor::Named{ name }
		, m_renderSystem{ renderSystem }
		, m_vertexData
		{
			TexturedQuad::Vertex{ Point2f{ -1.0, -1.0 }, config.texcoordConfig ? Point2f{ ( config.texcoordConfig->invertU ? 1.0 : 0.0 ), ( config.texcoordConfig->invertV ? 1.0 : 0.0 ) } : Point2f{} },
			TexturedQuad::Vertex{ Point2f{ -1.0, +1.0 }, config.texcoordConfig ? Point2f{ ( config.texcoordConfig->invertU ? 1.0 : 0.0 ), ( config.texcoordConfig->invertV ? 0.0 : 1.0 ) } : Point2f{} },
			TexturedQuad::Vertex{ Point2f{ +1.0, -1.0 }, config.texcoordConfig ? Point2f{ ( config.texcoordConfig->invertU ? 0.0 : 1.0 ), ( config.texcoordConfig->invertV ? 1.0 : 0.0 ) } : Point2f{} },
			TexturedQuad::Vertex{ Point2f{ +1.0, +1.0 }, config.texcoordConfig ? Point2f{ ( config.texcoordConfig->invertU ? 0.0 : 1.0 ), ( config.texcoordConfig->invertV ? 0.0 : 1.0 ) } : Point2f{} },
		}
		, m_sampler{ createSampler( *m_renderSystem.getEngine()
			, getName()
			, samplerFilter
			, ( config.range ? &config.range.value() : nullptr) ) }
		, m_useTexCoords{ bool( config.texcoordConfig ) }
		, m_blendMode{ config.blendMode ? *config.blendMode : BlendMode::eNoBlend }
	{
	}

	RenderQuad::RenderQuad( RenderQuad && rhs )noexcept
		: castor::Named{ std::forward< castor::Named && >( rhs ) }
		, m_renderSystem{ rhs.m_renderSystem }
		, m_sampler{ std::move( rhs.m_sampler ) }
		, m_pipeline{ std::move( rhs.m_pipeline ) }
		, m_pipelineLayout{ std::move( rhs.m_pipelineLayout ) }
		, m_commandBuffer{ std::move( rhs.m_commandBuffer ) }
		, m_vertexData{ std::move( rhs.m_vertexData ) }
		, m_vertexBuffer{ std::move( rhs.m_vertexBuffer ) }
		, m_descriptorSetLayout{ std::move( rhs.m_descriptorSetLayout ) }
		, m_descriptorSetPool{ std::move( rhs.m_descriptorSetPool ) }
		, m_descriptorSet{ std::move( rhs.m_descriptorSet ) }
		, m_sourceView{ std::move( rhs.m_sourceView ) }
		, m_useTexCoords{ std::move( rhs.m_useTexCoords ) }
		, m_blendMode{ std::move( rhs.m_blendMode ) }
	{
	}

	RenderQuad::~RenderQuad()
	{
		cleanup();
	}

	void RenderQuad::cleanup()
	{
		m_pipeline.reset();
		m_descriptorSet.reset();
		m_pipelineLayout.reset();
		m_descriptorSetPool.reset();
		m_descriptorSetLayout.reset();
		m_commandBuffer.reset();
		m_vertexBuffer.reset();
	}

	void RenderQuad::createPipeline( VkExtent2D const & size
		, castor::Position const & position
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::ImageView const & view
		, ashes::RenderPass const & renderPass
		, ashes::VkDescriptorSetLayoutBindingArray bindings
		, ashes::VkPushConstantRangeArray const & pushRanges )
	{
		createPipeline( size
			, position
			, program
			, view
			, renderPass
			, bindings
			, pushRanges
			, ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false } );
	}

	void RenderQuad::createPipeline( VkExtent2D const & size
		, castor::Position const & position
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::ImageView const & view
		, ashes::RenderPass const & renderPass
		, ashes::VkDescriptorSetLayoutBindingArray bindings
		, ashes::VkPushConstantRangeArray const & pushRanges
		, ashes::PipelineDepthStencilStateCreateInfo dsState )
	{
		m_sourceView = &view;
		m_sampler->initialise();
		auto & device = getCurrentRenderDevice( m_renderSystem );
		// Initialise the vertex buffer.
		m_vertexBuffer = makeVertexBuffer< TexturedQuad::Vertex >( device
			, 4u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getName() );

		if ( auto buffer = m_vertexBuffer->lock( 0u, 4u, 0u ) )
		{
			std::copy( m_vertexData.begin(), m_vertexData.end(), buffer );
			m_vertexBuffer->flush( 0u, 4u );
			m_vertexBuffer->unlock();
		}

		// Initialise the vertex layout.
		ashes::VkVertexInputAttributeDescriptionArray attributes
		{
			{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, position ) },
		};

		if ( m_useTexCoords )
		{
			attributes.push_back( { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, texture ) } );
		}

		ashes::PipelineVertexInputStateCreateInfo vertexState
		{
			0u,
			ashes::VkVertexInputBindingDescriptionArray
			{
				{ 0u, sizeof( TexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
			},
			std::move( attributes ),
		};

		// Initialise the descriptor set.
		auto textureBindingPoint = uint32_t( bindings.size() );

		if ( !bindings.empty() )
		{
			--textureBindingPoint;

			for ( auto & binding : bindings )
			{
				textureBindingPoint = std::max( textureBindingPoint, binding.binding );
			}

			++textureBindingPoint;
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( textureBindingPoint
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_descriptorSetLayout = device->createDescriptorSetLayout( getName()
			, std::move( bindings ) );
		m_pipelineLayout = device->createPipelineLayout( getName()
			, { *m_descriptorSetLayout }, pushRanges );
		m_descriptorSetPool = m_descriptorSetLayout->createPool( getName()
			, 1u );
		m_descriptorSet = m_descriptorSetPool->createDescriptorSet( getName() );
		doFillDescriptorSet( *m_descriptorSetLayout, *m_descriptorSet );
		m_descriptorSet->createBinding( m_descriptorSetLayout->getBinding( textureBindingPoint )
			, *m_sourceView
			, m_sampler->getSampler() );
		m_descriptorSet->update();

		// Initialise the pipeline.
		VkViewport viewport{ float( position.x() ), float( position.y() ), float( size.width ), float( size.height ) };
		VkRect2D scissor{ position.x(), position.y(), size.width, size.height };
		ashes::PipelineViewportStateCreateInfo vpState
		{
			0u,
			1u,
			ashes::VkViewportArray{ viewport },
			1u,
			ashes::VkScissorArray{ scissor },
		};
		m_pipeline = device->createPipeline( getName()
			, ashes::GraphicsPipelineCreateInfo
			(
				0u,
				program,
				std::move( vertexState ),
				ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP },
				ashes::nullopt,
				std::move( vpState ),
				ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::Optional< ashes::PipelineDepthStencilStateCreateInfo >( std::move( dsState ) ),
				doCreateBlendState( renderPass, m_blendMode ),
				ashes::nullopt,
				*m_pipelineLayout,
				static_cast< VkRenderPass const & >( renderPass )
			) );
	}

	void RenderQuad::prepareFrame( ashes::RenderPass const & renderPass
		, uint32_t subpassIndex )
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( getName()
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkType< VkCommandBufferInheritanceInfo >( renderPass
				, subpassIndex
				, VkFramebuffer( VK_NULL_HANDLE )
				, VkBool32( VK_FALSE )
				, 0u
				, 0u ) );
		registerFrame( *m_commandBuffer );
		m_commandBuffer->end();
	}

	void RenderQuad::registerFrame( ashes::CommandBuffer & commandBuffer )const
	{
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		doRegisterFrame( commandBuffer );
		commandBuffer.draw( 4u );
	}

	void RenderQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
	}

	void RenderQuad::doRegisterFrame( ashes::CommandBuffer & commandBuffer )const
	{
	}
}

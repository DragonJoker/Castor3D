#include "Castor3D/Render/ToTexture/RenderQuad.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
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
#include <ashespp/Image/Image.hpp>
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

		ashes::ImageViewArray createMipViews( ashes::ImageView const & source
			, uint32_t descriptorCount
			, bool arraySource )
		{
			ashes::ImageViewArray result;
			auto createInfo = source.createInfo;

			if ( !arraySource )
			{
				createInfo.subresourceRange.layerCount = 1u;

				if ( createInfo.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY )
				{
					createInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
				}
				else if ( createInfo.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
					|| createInfo.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
					|| createInfo.viewType == VK_IMAGE_VIEW_TYPE_3D )
				{
					createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				}
			}
			else
			{
				descriptorCount = 1u;
			}

			createInfo.subresourceRange.levelCount = 1u;

			for ( uint32_t layer = 0; layer < descriptorCount; ++layer )
			{
				createInfo.subresourceRange.baseMipLevel = source->subresourceRange.baseMipLevel;

				for ( uint32_t level = 0; level < source->subresourceRange.levelCount; ++level )
				{
					auto info = TextureView::convertToSampledView( createInfo );
					result.push_back( source.image->createView( std::move( info ) ) );
					++createInfo.subresourceRange.baseMipLevel;
				}

				++createInfo.subresourceRange.baseArrayLayer;
			}

			return result;
		}
	}

	RenderQuad::RenderQuad( RenderSystem & renderSystem
		, RenderDevice const & device
		, castor::String const & name
		, VkFilter samplerFilter
		, RenderQuadConfig config )
		: castor::Named{ name }
		, m_renderSystem{ renderSystem }
		, m_device{ device }
		, m_sampler{ createSampler( *m_renderSystem.getEngine()
			, getName()
			, samplerFilter
			, ( config.range ? &config.range.value() : nullptr ) ) }
		, m_vertexData
		{
			TexturedQuad::Vertex{ Point2f{ -1.0, -1.0 }, config.texcoordConfig ? Point2f{ ( config.texcoordConfig->invertU ? 1.0 : 0.0 ), ( config.texcoordConfig->invertV ? 1.0 : 0.0 ) } : Point2f{} },
			TexturedQuad::Vertex{ Point2f{ -1.0, +1.0 }, config.texcoordConfig ? Point2f{ ( config.texcoordConfig->invertU ? 1.0 : 0.0 ), ( config.texcoordConfig->invertV ? 0.0 : 1.0 ) } : Point2f{} },
			TexturedQuad::Vertex{ Point2f{ +1.0, -1.0 }, config.texcoordConfig ? Point2f{ ( config.texcoordConfig->invertU ? 0.0 : 1.0 ), ( config.texcoordConfig->invertV ? 1.0 : 0.0 ) } : Point2f{} },
			TexturedQuad::Vertex{ Point2f{ +1.0, +1.0 }, config.texcoordConfig ? Point2f{ ( config.texcoordConfig->invertU ? 0.0 : 1.0 ), ( config.texcoordConfig->invertV ? 0.0 : 1.0 ) } : Point2f{} },
		}
		, m_descriptorSetCount{ config.descriptorSetsCount ? *config.descriptorSetsCount : 1u }
		, m_useTexCoords{ bool( config.texcoordConfig ) }
		, m_arraySource{ config.arraySource ? *config.arraySource : false }
		, m_blendMode{ config.blendMode ? *config.blendMode : BlendMode::eNoBlend }
	{
	}

	RenderQuad::RenderQuad( RenderQuad && rhs )noexcept
		: castor::Named{ std::forward< castor::Named && >( rhs ) }
		, m_renderSystem{ rhs.m_renderSystem }
		, m_device{ rhs.m_device }
		, m_sampler{ std::move( rhs.m_sampler ) }
		, m_pipeline{ std::move( rhs.m_pipeline ) }
		, m_pipelineLayout{ std::move( rhs.m_pipelineLayout ) }
		, m_commandBuffer{ std::move( rhs.m_commandBuffer ) }
		, m_vertexData{ std::move( rhs.m_vertexData ) }
		, m_vertexBuffer{ std::move( rhs.m_vertexBuffer ) }
		, m_descriptorSetLayout{ std::move( rhs.m_descriptorSetLayout ) }
		, m_descriptorSetPool{ std::move( rhs.m_descriptorSetPool ) }
		, m_descriptorSets{ std::move( rhs.m_descriptorSets ) }
		, m_sourceView{ std::move( rhs.m_sourceView ) }
		, m_useTexCoords{ std::move( rhs.m_useTexCoords ) }
		, m_blendMode{ std::move( rhs.m_blendMode ) }
		, m_descriptorSetCount{ std::move( rhs.m_descriptorSetCount ) }
		, m_arraySource{ std::move( rhs.m_arraySource ) }
	{
	}

	RenderQuad::~RenderQuad()
	{
		cleanup();
	}

	void RenderQuad::cleanup()
	{
		m_pipeline.reset();
		m_descriptorSets.clear();
		m_pipelineLayout.reset();
		m_descriptorSetPool.reset();
		m_descriptorSetLayout.reset();
		m_commandBuffer.reset();
		m_vertexBuffer.reset();
	}

	void RenderQuad::createPipeline( VkExtent2D const & size
		, castor::Position const & position
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::ImageView const & inputView
		, ashes::RenderPass const & renderPass
		, ashes::VkDescriptorSetLayoutBindingArray bindings
		, ashes::VkPushConstantRangeArray const & pushRanges )
	{
		createPipeline( size
			, position
			, program
			, inputView
			, renderPass
			, bindings
			, pushRanges
			, ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false } );
	}

	void RenderQuad::createPipeline( VkExtent2D const & size
		, castor::Position const & position
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::ImageView const & inputView
		, ashes::RenderPass const & renderPass
		, ashes::VkDescriptorSetLayoutBindingArray bindings
		, ashes::VkPushConstantRangeArray const & pushRanges
		, ashes::PipelineDepthStencilStateCreateInfo dsState )
	{
		m_sourceView = &inputView;
		m_sourceMipViews = createMipViews( *m_sourceView, m_descriptorSetCount, m_arraySource );
		m_sampler->initialise( m_device );
		// Initialise the vertex buffer.
		m_vertexBuffer = makeVertexBuffer< TexturedQuad::Vertex >( m_device
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
		m_descriptorSetLayout = m_device->createDescriptorSetLayout( getName()
			, std::move( bindings ) );
		m_pipelineLayout = m_device->createPipelineLayout( getName()
			, { *m_descriptorSetLayout }, pushRanges );
		auto descriptorSetCount = uint32_t( m_descriptorSetCount * m_sourceView->createInfo.subresourceRange.levelCount );
		m_descriptorSetPool = m_descriptorSetLayout->createPool( getName()
			, descriptorSetCount );
		m_descriptorSets.reserve( descriptorSetCount );

		for ( uint32_t index = 0u; index < m_descriptorSetCount; ++index )
		{
			auto prefix = getName() + "_" + string::toString( index ) + "x";

			for ( auto levelOff = 0u; levelOff < m_sourceView->createInfo.subresourceRange.levelCount; ++levelOff )
			{
				auto descriptorSet = m_descriptorSetPool->createDescriptorSet( prefix + string::toString( index ) );
				doFillDescriptorSet( *m_descriptorSetLayout
					, *descriptorSet
					, index
					, levelOff + m_sourceView->createInfo.subresourceRange.baseMipLevel );
				descriptorSet->createBinding( m_descriptorSetLayout->getBinding( textureBindingPoint )
					, m_sourceMipViews[getDescriptorSetIndex( index, levelOff )]
					, m_sampler->getSampler() );
				descriptorSet->update();
				m_descriptorSets.emplace_back( std::move( descriptorSet ) );
			}
		}

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
		m_pipeline = m_device->createPipeline( getName()
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
		, uint32_t subpassIndex
		, uint32_t descriptorSetIndex )
	{
		m_commandBuffer = m_device.graphicsCommandPool->createCommandBuffer( getName()
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkType< VkCommandBufferInheritanceInfo >( renderPass
				, subpassIndex
				, VkFramebuffer( VK_NULL_HANDLE )
				, VkBool32( VK_FALSE )
				, 0u
				, 0u ) );
		registerFrame( *m_commandBuffer, descriptorSetIndex );
		m_commandBuffer->end();
	}

	void RenderQuad::registerFrame( ashes::CommandBuffer & commandBuffer
		, uint32_t descriptorSetIndex )const
	{
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		commandBuffer.bindDescriptorSet( *m_descriptorSets[descriptorSetIndex], *m_pipelineLayout );
		doRegisterFrame( commandBuffer );
		commandBuffer.draw( 4u );
	}

	uint32_t RenderQuad::getDescriptorSetIndex( uint32_t descriptorBaseIndex
		, uint32_t level )const
	{
		CU_Require( m_sourceView );
		return ( descriptorBaseIndex * m_sourceView->createInfo.subresourceRange.levelCount ) + level;
	}

	void RenderQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
	}
	
	void RenderQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet
		, uint32_t descriptorSetIndex )
	{
		doFillDescriptorSet( descriptorSetLayout
			, descriptorSet );
	}

	void RenderQuad::doRegisterFrame( ashes::CommandBuffer & commandBuffer )const
	{
	}
}

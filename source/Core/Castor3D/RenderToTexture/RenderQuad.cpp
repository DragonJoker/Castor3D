#include "Castor3D/RenderToTexture/RenderQuad.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Texture/Sampler.hpp"

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

using namespace castor;

namespace castor3d
{
	namespace
	{
		SamplerSPtr doCreateSampler( RenderSystem const & renderSystem, bool nearest )
		{
			String const name = nearest
				? String{ cuT( "RenderQuad_Nearest" ) }
			: String{ cuT( "RenderQuad_Linear" ) };
			VkFilter const filter = nearest
				? VK_FILTER_NEAREST
				: VK_FILTER_LINEAR;
			auto & cache = renderSystem.getEngine()->getSamplerCache();
			SamplerSPtr sampler;

			if ( cache.has( name ) )
			{
				sampler = cache.find( name );
			}
			else
			{
				sampler = cache.add( name );
				sampler->setMinFilter( filter );
				sampler->setMagFilter( filter );
				sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			}

			return sampler;
		}
	}

	RenderQuad::RenderQuad( RenderSystem & renderSystem
		, bool nearest
		, bool invertU
		, bool invertV )
		: m_renderSystem{ renderSystem }
		, m_vertexData
		{
			TexturedQuad::Vertex{ Point2f{ -1.0, -1.0 }, Point2f{ ( invertU ? 1.0 : 0.0 ), ( invertV ? 1.0 : 0.0 ) } },
			TexturedQuad::Vertex{ Point2f{ -1.0, +1.0 }, Point2f{ ( invertU ? 1.0 : 0.0 ), ( invertV ? 0.0 : 1.0 ) } },
			TexturedQuad::Vertex{ Point2f{ +1.0, -1.0 }, Point2f{ ( invertU ? 0.0 : 1.0 ), ( invertV ? 1.0 : 0.0 ) } },
			TexturedQuad::Vertex{ Point2f{ +1.0, +1.0 }, Point2f{ ( invertU ? 0.0 : 1.0 ), ( invertV ? 0.0 : 1.0 ) } },
		}
		, m_sampler{ doCreateSampler( m_renderSystem, nearest ) }
	{
	}

	RenderQuad::RenderQuad( RenderQuad && rhs )
		: m_renderSystem{ rhs.m_renderSystem }
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
			, "RenderQuad" );

		if ( auto buffer = m_vertexBuffer->lock( 0u, 4u, 0u ) )
		{
			std::copy( m_vertexData.begin(), m_vertexData.end(), buffer );
			m_vertexBuffer->flush( 0u, 4u );
			m_vertexBuffer->unlock();
		}

		// Initialise the vertex layout.
		ashes::PipelineVertexInputStateCreateInfo vertexState
		{
			0u,
			ashes::VkVertexInputBindingDescriptionArray
			{
				{ 0u, sizeof( TexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
			},
			ashes::VkVertexInputAttributeDescriptionArray
			{
				{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, position ) },
				{ 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, texture ) },
			},
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
		m_descriptorSetLayout = device->createDescriptorSetLayout( std::move( bindings ) );
		m_pipelineLayout = device->createPipelineLayout( { *m_descriptorSetLayout }, pushRanges );
		m_descriptorSetPool = m_descriptorSetLayout->createPool( 1u );
		m_descriptorSet = m_descriptorSetPool->createDescriptorSet();
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
		m_pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
			(
				0u,
				program,
				std::move( vertexState ),
				ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP },
				std::nullopt,
				std::move( vpState ),
				ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::Optional< ashes::PipelineDepthStencilStateCreateInfo >( std::move( dsState ) ),
				ashes::PipelineColorBlendStateCreateInfo{},
				ashes::nullopt,
				*m_pipelineLayout,
				static_cast< VkRenderPass const & >( renderPass )
			) );
	}

	void RenderQuad::prepareFrame( ashes::RenderPass const & renderPass
		, uint32_t subpassIndex )
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_SECONDARY );
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

#include "Castor3D/Render/Passes/RenderQuad.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>
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

		bool checkWrites( ashes::WriteDescriptorSetArray const & writes
			, rq::BindingDescriptionArray const & bindings )
		{
			if ( writes.size() != bindings.size() )
			{
				return false;
			}

			for ( auto & write : writes )
			{
				if ( write->descriptorType != bindings[write->dstBinding].descriptorType )
				{
					return false;
				}
			}

			return true;
		}

		ashes::VkDescriptorSetLayoutBindingArray createBindings( rq::BindingDescriptionArray const & bindings )
		{
			ashes::VkDescriptorSetLayoutBindingArray result;
			uint32_t index = 0u;

			for ( auto & binding : bindings )
			{
				result.push_back( { index++
					, binding.descriptorType
					, 1u
					, binding.stageFlags
					, nullptr } );
			}

			return result;
		}

		ashes::VkDescriptorSetLayoutBindingArray createBindings( rq::BindingDescriptionArray const & bindings
			, ashes::WriteDescriptorSetArray const & writes )
		{
			ashes::VkDescriptorSetLayoutBindingArray result;
			uint32_t index = 0u;

			for ( auto & write : writes )
			{
				auto & binding = bindings[index++];
				result.push_back( { write->dstBinding
					, binding.descriptorType
					, write->descriptorCount
					, binding.stageFlags
					, nullptr } );
			}

			return result;
		}

		template< typename TypeT >
		struct DefaultValueGetterT;

		template<>
		struct DefaultValueGetterT< VkImageSubresourceRange >
		{
			static VkImageSubresourceRange const & get()
			{
				static VkImageSubresourceRange const result = VkImageSubresourceRange{ 0u, 0u, 1u, 0u, 1u };
				return result;
			}
		};

		template<>
		struct DefaultValueGetterT< BlendMode >
		{
			static BlendMode const & get()
			{
				static BlendMode const result = BlendMode::eNoBlend;
				return result;
			}
		};

		template<>
		struct DefaultValueGetterT< rq::Texcoord >
		{
			static rq::Texcoord const & get()
			{
				static rq::Texcoord const result{ false, false };
				return result;
			}
		};

		template<>
		struct DefaultValueGetterT< rq::BindingDescriptionArray >
		{
			static rq::BindingDescriptionArray const & get()
			{
				static rq::BindingDescriptionArray const result = rq::BindingDescriptionArray{ []()
					{
						rq::BindingDescriptionArray result
						{
							{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
						};
						return result;
				}( ) };
				return result;
			}
		};

		template< typename TypeT >
		static inline TypeT const & defaultV = DefaultValueGetterT< TypeT >::get();

		ashes::WriteDescriptorSet clone( ashes::WriteDescriptorSet const & src )
		{
			ashes::WriteDescriptorSet result{ src->dstBinding
				, src->dstArrayElement
				, src->descriptorCount
				, src->descriptorType };
			result.bufferInfo = src.bufferInfo;
			result.imageInfo = src.imageInfo;
			result.texelBufferView = src.texelBufferView;
			return result;
		}
	}

	RenderQuad::RenderQuad( RenderDevice const & device
		, castor::String const & name
		, VkFilter samplerFilter
		, rq::Config config )
		: castor::Named{ name }
		, m_renderSystem{ device.renderSystem }
		, m_device{ device }
		, m_sampler{ createSampler( *m_renderSystem.getEngine()
			, getName()
			, samplerFilter
			, ( config.range ? &config.range.value() : nullptr ) ) }
		, m_config{ ( config.bindings ? *config.bindings : defaultV< rq::BindingDescriptionArray > )
			, ( config.range ? *config.range : defaultV< VkImageSubresourceRange > )
			, ( config.texcoordConfig ? *config.texcoordConfig : defaultV< rq::Texcoord > )
			, ( config.blendMode ? *config.blendMode : defaultV< BlendMode > ) }
		, m_useTexCoord{ config.texcoordConfig }
	{
		m_sampler->initialise( m_device );
	}

	RenderQuad::RenderQuad( RenderQuad && rhs )noexcept
		: castor::Named{ std::forward< castor::Named && >( rhs ) }
		, m_renderSystem{ rhs.m_renderSystem }
		, m_device{ rhs.m_device }
		, m_sampler{ std::move( rhs.m_sampler ) }
		, m_config{ std::move( rhs.m_config ) }
		, m_useTexCoord{ std::move( rhs.m_useTexCoord ) }
		, m_descriptorSetLayout{ std::move( rhs.m_descriptorSetLayout ) }
		, m_pipelineLayout{ std::move( rhs.m_pipelineLayout ) }
		, m_pipeline{ std::move( rhs.m_pipeline ) }
		, m_descriptorSetPool{ std::move( rhs.m_descriptorSetPool ) }
		, m_passes{ std::move( rhs.m_passes ) }
		, m_descriptorSets{ std::move( rhs.m_descriptorSets ) }
		, m_vertexBuffer{ std::move( rhs.m_vertexBuffer ) }
	{
	}

	RenderQuad::~RenderQuad()
	{
		m_sampler->cleanup();
		cleanup();
	}

	void RenderQuad::cleanup()
	{
		for ( auto & pass : m_passes )
		{
			for ( auto & write : pass )
			{
				write = clone( write );
			}
		}

		m_vertexBuffer.reset();
		m_descriptorSets.clear();
		m_descriptorSetPool.reset();
		m_pipeline.reset();
		m_pipelineLayout.reset();
		m_descriptorSetLayout.reset();
	}

	void RenderQuad::createPipeline( VkExtent2D const & size
		, castor::Position const & position
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::RenderPass const & renderPass
		, ashes::VkPushConstantRangeArray const & pushRanges
		, ashes::PipelineDepthStencilStateCreateInfo dsState )
	{
		// Initialise the vertex buffer.
		m_vertexBuffer = makeVertexBuffer< TexturedQuad::Vertex >( m_device
			, 4u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getName() );

		if ( auto buffer = m_vertexBuffer->lock( 0u, 4u, 0u ) )
		{
			std::array< TexturedQuad::Vertex, 4u > vertexData
			{
				TexturedQuad::Vertex{ Point2f{ -1.0, -1.0 }
				, ( m_useTexCoord
					? Point2f{ ( m_config.texcoordConfig.invertU ? 1.0 : 0.0 ), ( m_config.texcoordConfig.invertV ? 1.0 : 0.0 ) }
					: Point2f{} ) },
				TexturedQuad::Vertex{ Point2f{ -1.0, +1.0 }
				, ( m_useTexCoord ? Point2f{ ( m_config.texcoordConfig.invertU ? 1.0 : 0.0 ), ( m_config.texcoordConfig.invertV ? 0.0 : 1.0 ) }
					: Point2f{} ) },
				TexturedQuad::Vertex{ Point2f{ +1.0, -1.0 }
				, ( m_useTexCoord
					? Point2f{ ( m_config.texcoordConfig.invertU ? 0.0 : 1.0 ), ( m_config.texcoordConfig.invertV ? 1.0 : 0.0 ) }
					: Point2f{} ) },
				TexturedQuad::Vertex{ Point2f{ +1.0, +1.0 }
				, ( m_useTexCoord
					? Point2f{ ( m_config.texcoordConfig.invertU ? 0.0 : 1.0 ), ( m_config.texcoordConfig.invertV ? 0.0 : 1.0 ) }
					: Point2f{} ) },
			};
			std::copy( vertexData.begin(), vertexData.end(), buffer );
			m_vertexBuffer->flush( 0u, 4u );
			m_vertexBuffer->unlock();
		}

		// Initialise the vertex layout.
		ashes::VkVertexInputAttributeDescriptionArray attributes
		{
			{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, position ) },
		};

		if ( m_useTexCoord )
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

		auto bindings = createBindings( m_config.bindings );
		CU_Require( bindings.capacity() < 1000u );
		m_descriptorSetLayout = m_device->createDescriptorSetLayout( getName()
			, std::move( bindings ) );
		m_pipelineLayout = m_device->createPipelineLayout( getName()
			, { *m_descriptorSetLayout }, pushRanges );

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
				doCreateBlendState( renderPass, m_config.blendMode ),
				ashes::nullopt,
				*m_pipelineLayout,
				static_cast< VkRenderPass const & >( renderPass )
			) );
	}

	void RenderQuad::registerPassInputs( ashes::WriteDescriptorSetArray const & writes )
	{
		CU_Require( checkWrites( writes, m_config.bindings ) );

		m_passes.emplace_back( writes );
	}

	void RenderQuad::initialisePasses()
	{
		CU_Require( m_descriptorSetLayout );
		auto descriptorSetCount = uint32_t( uint32_t( m_passes.size() ) );
		m_descriptorSetPool = m_descriptorSetLayout->createPool( getName()
			, descriptorSetCount );
		m_descriptorSets.reserve( descriptorSetCount );
		uint32_t index = 0u;
		auto prefix = getName() + ", Pass ";

		for ( auto & pass : m_passes )
		{
			auto descriptorSet = m_descriptorSetPool->createDescriptorSet( prefix + string::toString( index ) );
			descriptorSet->setBindings( pass );
			descriptorSet->update();
			m_descriptorSets.emplace_back( std::move( descriptorSet ) );
			++index;
		}
	}

	void RenderQuad::createPipelineAndPass( VkExtent2D const & size
		, castor::Position const & position
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::RenderPass const & renderPass
		, ashes::WriteDescriptorSetArray const & writes
		, ashes::VkPushConstantRangeArray const & pushRanges
		, ashes::PipelineDepthStencilStateCreateInfo dsState )
	{
		CU_Require( checkWrites( writes, m_config.bindings ) );
		createPipeline( size
			, position
			, program
			, renderPass
			, pushRanges
			, std::move( dsState ) );
		m_passes.emplace_back( writes );
		initialisePasses();
	}

	void RenderQuad::registerPass( ashes::CommandBuffer & commandBuffer
		, uint32_t descriptorSetIndex )const
	{
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		commandBuffer.bindDescriptorSet( *m_descriptorSets[descriptorSetIndex], *m_pipelineLayout );
		doRegisterPass( commandBuffer );
		commandBuffer.draw( 4u );
	}

	ashes::WriteDescriptorSet RenderQuad::makeDescriptorWrite( ashes::ImageView const & view
		, ashes::Sampler const & sampler
		, uint32_t dstBinding
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };
		result.imageInfo.push_back( { sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );
		return result;
	}

	ashes::WriteDescriptorSet RenderQuad::makeDescriptorWrite( ashes::UniformBuffer const & buffer
		, uint32_t dstBinding
		, uint32_t elemOffset
		, uint32_t elemRange
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		result.bufferInfo.push_back( { buffer.getBuffer()
			, buffer.getAlignedSize() * elemOffset
			, buffer.getAlignedSize() * elemRange } );
		return result;
	}

	ashes::WriteDescriptorSet RenderQuad::makeDescriptorWrite( ashes::BufferBase const & storageBuffer
		, uint32_t dstBinding
		, uint32_t byteOffset
		, uint32_t byteRange
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( { storageBuffer
			, byteOffset
			, byteRange } );
		return result;
	}

	ashes::WriteDescriptorSet RenderQuad::makeDescriptorWrite( ashes::BufferBase const & buffer
		, ashes::BufferView const & view
		, uint32_t dstBinding
		, uint32_t dstArrayElement )
	{
		auto result = ashes::WriteDescriptorSet{ dstBinding
			, dstArrayElement
			, 1u
			, ( ( buffer.getUsage() & VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
				? VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
				: VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER ), };
		result.bufferInfo.push_back( { buffer
			, view.getOffset()
			, view.getRange() } );
		result.texelBufferView.push_back( view );
		return result;
	}

	void RenderQuad::doRegisterPass( ashes::CommandBuffer & commandBuffer )const
	{
	}
}

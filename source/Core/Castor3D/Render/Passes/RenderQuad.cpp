#include "Castor3D/Render/Passes/RenderQuad.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
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

CU_ImplementCUSmartPtr( castor3d, RenderQuad )

namespace castor3d
{
	namespace passrquad
	{
		static uint32_t doGetColourAttachmentCount( ashes::RenderPass const & pass )
		{
			return uint32_t( std::count_if( pass.getAttachments().begin()
				, pass.getAttachments().end()
				, []( VkAttachmentDescription const & lookup )
				{
					return !ashes::isDepthOrStencilFormat( lookup.format );
				} ) );
		}

		static ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( ashes::RenderPass const & pass
			, BlendMode blendMode )
		{
			return RenderNodesPass::createBlendState( blendMode
				, blendMode
				, doGetColourAttachmentCount( pass ) );
		}

#if !defined( NDEBUG )

		static bool checkWrites( ashes::WriteDescriptorSetArray const & writes
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

#endif

		static ashes::VkDescriptorSetLayoutBindingArray createBindings( rq::BindingDescriptionArray const & bindings )
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
						rq::BindingDescriptionArray tmp
						{
							{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
						};
						return tmp;
				}( ) };
				return result;
			}
		};

		template<>
		struct DefaultValueGetterT< IntermediateView >
		{
			static IntermediateView const & get()
			{
				static IntermediateView const result = IntermediateView{ []()
					{
						IntermediateView tmp{};
						return tmp;
				}( ) };
				return result;
			}
		};

		template< typename TypeT >
		static inline TypeT const & defaultV = DefaultValueGetterT< TypeT >::get();

		static ashes::WriteDescriptorSet clone( ashes::WriteDescriptorSet const & src )
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
		, m_config{ ( config.bindings ? *config.bindings : passrquad::defaultV< rq::BindingDescriptionArray > )
			, ( config.range ? *config.range : passrquad::defaultV< VkImageSubresourceRange > )
			, ( config.texcoordConfig ? *config.texcoordConfig : passrquad::defaultV< rq::Texcoord > )
			, ( config.blendMode ? *config.blendMode : passrquad::defaultV< BlendMode > )
			, ( config.tex3DResult ? *config.tex3DResult : passrquad::defaultV< IntermediateView > ) }
		, m_useTexCoord{ config.texcoordConfig }
	{
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
		, m_invertY{ std::move( rhs.m_invertY ) }
		, m_vertexBuffer{ std::move( rhs.m_vertexBuffer ) }
		, m_uvInvVertexBuffer{ std::move( rhs.m_uvInvVertexBuffer ) }
	{
	}

	RenderQuad::~RenderQuad()
	{
		cleanup();
	}

	void RenderQuad::cleanup()
	{
		for ( auto & pass : m_passes )
		{
			for ( auto & write : pass )
			{
				write = passrquad::clone( write );
			}
		}

		m_vertexBuffer.reset();
		m_uvInvVertexBuffer.reset();
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
		log::debug << "Creating pipeline for " << getName() << std::endl;
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
				TexturedQuad::Vertex{ castor::Point2f{ -1.0, -1.0 }
					, ( m_useTexCoord
						? castor::Point2f{ ( m_config.texcoordConfig.invertU ? 1.0 : 0.0 ), ( m_config.texcoordConfig.invertV ? 1.0 : 0.0 ) }
						: castor::Point2f{} ) },
				TexturedQuad::Vertex{ castor::Point2f{ -1.0, +1.0 }
					, ( m_useTexCoord
						? castor::Point2f{ ( m_config.texcoordConfig.invertU ? 1.0 : 0.0 ), ( m_config.texcoordConfig.invertV ? 0.0 : 1.0 ) }
						: castor::Point2f{} ) },
				TexturedQuad::Vertex{ castor::Point2f{ +1.0, -1.0 }
					, ( m_useTexCoord
						? castor::Point2f{ ( m_config.texcoordConfig.invertU ? 0.0 : 1.0 ), ( m_config.texcoordConfig.invertV ? 1.0 : 0.0 ) }
						: castor::Point2f{} ) },
				TexturedQuad::Vertex{ castor::Point2f{ +1.0, +1.0 }
					, ( m_useTexCoord
						? castor::Point2f{ ( m_config.texcoordConfig.invertU ? 0.0 : 1.0 ), ( m_config.texcoordConfig.invertV ? 0.0 : 1.0 ) }
						: castor::Point2f{} ) },
			};
			std::copy( vertexData.begin(), vertexData.end(), buffer );
			m_vertexBuffer->flush( 0u, 4u );
			m_vertexBuffer->unlock();
		}

		// Initialise the V inverted vertex buffer.
		m_uvInvVertexBuffer = makeVertexBuffer< TexturedQuad::Vertex >( m_device
			, 4u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, getName() );

		if ( auto buffer = m_uvInvVertexBuffer->lock( 0u, 4u, 0u ) )
		{
			std::array< TexturedQuad::Vertex, 4u > vertexData
			{
				TexturedQuad::Vertex{ castor::Point2f{ -1.0, -1.0 }
					, ( m_useTexCoord
						? castor::Point2f{ ( m_config.texcoordConfig.invertU ? 1.0 : 0.0 ), ( m_config.texcoordConfig.invertV ? 0.0 : 1.0 ) }
						: castor::Point2f{} ) },
				TexturedQuad::Vertex{ castor::Point2f{ -1.0, +1.0 }
					, ( m_useTexCoord
						? castor::Point2f{ ( m_config.texcoordConfig.invertU ? 1.0 : 0.0 ), ( m_config.texcoordConfig.invertV ? 1.0 : 0.0 ) }
						: castor::Point2f{} ) },
				TexturedQuad::Vertex{ castor::Point2f{ +1.0, -1.0 }
					, ( m_useTexCoord
						? castor::Point2f{ ( m_config.texcoordConfig.invertU ? 0.0 : 1.0 ), ( m_config.texcoordConfig.invertV ? 0.0 : 1.0 ) }
						: castor::Point2f{} ) },
				TexturedQuad::Vertex{ castor::Point2f{ +1.0, +1.0 }
					, ( m_useTexCoord
						? castor::Point2f{ ( m_config.texcoordConfig.invertU ? 0.0 : 1.0 ), ( m_config.texcoordConfig.invertV ? 1.0 : 0.0 ) }
						: castor::Point2f{} ) },
			};
			std::copy( vertexData.begin(), vertexData.end(), buffer );
			m_uvInvVertexBuffer->flush( 0u, 4u );
			m_uvInvVertexBuffer->unlock();
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

		auto bindings = passrquad::createBindings( m_config.bindings );
		CU_Require( bindings.capacity() < 1000u );
		m_descriptorSetLayout = m_device->createDescriptorSetLayout( getName()
			, std::move( bindings ) );
		m_pipelineLayout = m_device->createPipelineLayout( getName()
			, { *m_descriptorSetLayout }, pushRanges );

		// Initialise the pipeline.
		VkViewport viewport{ float( position.x() ), float( position.y() ), float( size.width ), float( size.height ), 0.0f, 1.0f };
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
				passrquad::doCreateBlendState( renderPass, m_config.blendMode ),
				ashes::nullopt,
				*m_pipelineLayout,
				static_cast< VkRenderPass const & >( renderPass )
			) );
	}

	void RenderQuad::registerPassInputs( ashes::WriteDescriptorSetArray const & writes
		, bool invertY )
	{
		CU_Require( passrquad::checkWrites( writes, m_config.bindings ) );

		m_passes.emplace_back( writes );
		m_invertY.emplace_back( invertY );
	}

	void RenderQuad::initialisePasses()
	{
		log::debug << "Initialising passes for " << getName() << std::endl;
		CU_Require( m_descriptorSetLayout );
		auto descriptorSetCount = uint32_t( m_passes.size() );
		m_descriptorSetPool = m_descriptorSetLayout->createPool( getName()
			, descriptorSetCount );
		m_descriptorSets.reserve( descriptorSetCount );
		uint32_t index = 0u;
		auto prefix = getName() + ", Pass ";

		for ( auto & pass : m_passes )
		{
			auto descriptorSet = m_descriptorSetPool->createDescriptorSet( prefix + castor::string::toString( index ) );
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
		CU_Require( passrquad::checkWrites( writes, m_config.bindings ) );
		createPipeline( size
			, position
			, program
			, renderPass
			, pushRanges
			, std::move( dsState ) );
		m_passes.emplace_back( writes );
		m_invertY.emplace_back( false );
		initialisePasses();
	}

	void RenderQuad::registerPass( ashes::CommandBuffer & commandBuffer
		, uint32_t descriptorSetIndex )const
	{
		commandBuffer.bindPipeline( *m_pipeline );

		if ( m_invertY[descriptorSetIndex] )
		{
			commandBuffer.bindVertexBuffer( 0u, m_uvInvVertexBuffer->getBuffer(), 0u );
		}
		else
		{
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		}

		commandBuffer.bindDescriptorSet( *m_descriptorSets[descriptorSetIndex], *m_pipelineLayout );
		doRegisterPass( commandBuffer );
		commandBuffer.draw( 4u );
	}

	void RenderQuad::doRegisterPass( ashes::CommandBuffer & commandBuffer )const
	{
	}
}

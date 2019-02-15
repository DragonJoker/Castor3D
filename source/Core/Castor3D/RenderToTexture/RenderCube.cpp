#include "RenderCube.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/Sampler.hpp"

#include <Ashes/Buffer/StagingBuffer.hpp>
#include <Ashes/Command/CommandBufferInheritanceInfo.hpp>
#include <Ashes/Core/Device.hpp>
#include <Ashes/Core/Renderer.hpp>
#include <Ashes/Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Ashes/Descriptor/WriteDescriptorSet.hpp>
#include <Ashes/Pipeline/DepthStencilState.hpp>
#include <Ashes/Pipeline/InputAssemblyState.hpp>
#include <Ashes/Pipeline/MultisampleState.hpp>
#include <Ashes/Pipeline/RasterisationState.hpp>
#include <Ashes/Pipeline/Scissor.hpp>
#include <Ashes/Pipeline/Viewport.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		SamplerSPtr doCreateSampler( RenderSystem & renderSystem
			, bool nearest )
		{
			String const name = nearest
				? String{ cuT( "RenderCube_Nearest" ) }
			: String{ cuT( "RenderCube_Linear" ) };
			ashes::Filter const filter = nearest
				? ashes::Filter::eNearest
				: ashes::Filter::eLinear;
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
				sampler->setWrapS( ashes::WrapMode::eClampToEdge );
				sampler->setWrapT( ashes::WrapMode::eClampToEdge );
				sampler->setWrapR( ashes::WrapMode::eClampToEdge );
			}

			return sampler;
		}

		ashes::UniformBufferPtr< castor::Matrix4x4f > doCreateMatrixUbo( ashes::Device const & device
			, ashes::CommandBuffer const & commandBuffer
			, bool srcIsCube )
		{
			static castor::Matrix4x4f const projection = convert( device.perspective( float( 90.0_degrees ), 1.0f, 0.1f, 10.0f ) );

			auto result = ashes::makeUniformBuffer< castor::Matrix4x4f >( device
				, 6u
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			static std::array< castor::Matrix4x4f, 6u > const views = [&device]()
			{
				std::array< castor::Matrix4x4f, 6u > result
				{
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ -1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, +1.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, -1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, -1.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, +1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, -1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } )
				};

				if ( device.getRenderer().getClipDirection() == ashes::ClipDirection::eBottomUp )
				{
					std::swap( result[2], result[3] );
				}

				return result;
			}();

			result->getData( 0u ) = projection * views[0];
			result->getData( 1u ) = projection * views[1];
			result->getData( 2u ) = projection * views[2];
			result->getData( 3u ) = projection * views[3];
			result->getData( 4u ) = projection * views[4];
			result->getData( 5u ) = projection * views[5];

			ashes::StagingBuffer stagingBuffer{ device
				, ashes::BufferTarget::eTransferSrc
				, 6u * result->getAlignedSize() };
			stagingBuffer.uploadUniformData( commandBuffer
				, result->getDatas()
				, *result );
			return result;
		}

		ashes::VertexBufferPtr< castor::Point4f > doCreateVertexBuffer( ashes::Device const & device
			, ashes::CommandBuffer const & commandBuffer )
		{
			std::vector< castor::Point4f > vertexData
			{
				castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, +1, -1, +1 },// Back
				castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ -1, +1, +1, +1 },// Left
				castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, -1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, +1, -1, +1 },// Right
				castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, +1, +1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, -1, +1, +1 },// Front
				castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ +1, +1, -1, +1 }, castor::Point4f{ +1, +1, +1, +1 }, castor::Point4f{ -1, +1, -1, +1 }, castor::Point4f{ -1, +1, +1, +1 },// Top
				castor::Point4f{ -1, -1, -1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ -1, -1, +1, +1 }, castor::Point4f{ +1, -1, -1, +1 }, castor::Point4f{ +1, -1, +1, +1 }, castor::Point4f{ -1, -1, +1, +1 },// Bottom
			};
			auto result = ashes::makeVertexBuffer< castor::Point4f >( device
				, uint32_t( vertexData.size() )
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eDeviceLocal );

			ashes::StagingBuffer stagingBuffer{ device
				, ashes::BufferTarget::eTransferSrc
				, result->getBuffer().getSize() };
			stagingBuffer.uploadVertexData( commandBuffer
				, vertexData
				, *result );
			return result;
		}

		ashes::VertexInputState doCreateVertexLayout()
		{
			ashes::VertexInputState result;
			result.vertexBindingDescriptions.push_back( {
				0u,
				uint32_t( sizeof( castor::Point4f ) ),
				ashes::VertexInputRate::eVertex
			} );
			result.vertexAttributeDescriptions.push_back( {
				0u,
				0u,
				ashes::Format::eR32G32B32A32_SFLOAT,
				0u
			} );
			return result;
		}

		ashes::DescriptorSetLayoutPtr doCreateDescriptorSetLayout( ashes::Device const & device )
		{
			ashes::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },
				{ 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}
	}

	RenderCube::RenderCube( RenderSystem & renderSystem
		, bool nearest
		, SamplerSPtr sampler )
		: m_renderSystem{ renderSystem }
		, m_sampler{ sampler ? std::move( sampler ) : doCreateSampler( m_renderSystem, nearest ) }
	{
	}

	RenderCube::~RenderCube()
	{
		cleanup();
	}

	void RenderCube::createPipelines( ashes::Extent2D const & size
		, castor::Position const & position
		, ashes::ShaderStageStateArray const & program
		, ashes::TextureView const & view
		, ashes::RenderPass const & renderPass
		, ashes::PushConstantRangeArray const & pushRanges )
	{
		createPipelines( size
			, position
			, program
			, view
			, renderPass
			, pushRanges
			, ashes::DepthStencilState{ 0u, false, false } );
	}

	void RenderCube::createPipelines( ashes::Extent2D const & size
		, castor::Position const & position
		, ashes::ShaderStageStateArray const & program
		, ashes::TextureView const & view
		, ashes::RenderPass const & renderPass
		, ashes::PushConstantRangeArray const & pushRanges
		, ashes::DepthStencilState const & dsState )
	{
		m_sampler->initialise();
		auto & device = getCurrentDevice( m_renderSystem );
		auto commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		m_matrixUbo = doCreateMatrixUbo( device, *commandBuffer, view.getType() == ashes::TextureViewType::eCube );
		m_vertexBuffer = doCreateVertexBuffer( device, *commandBuffer );
		auto vertexLayout = doCreateVertexLayout();

		// Initialise the descriptor set.
		ashes::DescriptorSetLayoutBindingArray bindings;
		bindings.emplace_back( 0u
			, ashes::DescriptorType::eUniformBuffer
			, ashes::ShaderStageFlag::eVertex );
		bindings.emplace_back( 1u
			, ashes::DescriptorType::eCombinedImageSampler
			, ashes::ShaderStageFlag::eFragment );
		doFillDescriptorLayoutBindings( bindings );
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_pipelineLayout = device.createPipelineLayout( { *m_descriptorLayout }, pushRanges );
		m_descriptorPool = m_descriptorLayout->createPool( 6u );
		uint32_t face = 0u;

		for ( auto & facePipeline : m_faces )
		{
			facePipeline.pipeline = m_pipelineLayout->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				program,
				renderPass,
				vertexLayout,
				ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
				ashes::RasterisationState{ 0u, false, false, ashes::PolygonMode::eFill, ashes::CullModeFlag::eNone },
				ashes::MultisampleState{},
				ashes::ColourBlendState::createDefault(),
				{},
				ashes::DepthStencilState{ 0u, false, false },
				ashes::TessellationState{},
				ashes::Viewport{ size.width, size.height, 0, 0 },
				ashes::Scissor{ 0, 0, size.width, size.height }
			} );

			facePipeline.descriptorSet = m_descriptorPool->createDescriptorSet();
			facePipeline.descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
				, *m_matrixUbo
				, face
				, 1u );
			facePipeline.descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
				, view
				, m_sampler->getSampler() );
			doFillDescriptorSet( *m_descriptorLayout, *facePipeline.descriptorSet, face );
			facePipeline.descriptorSet->update();
			++face;
		}
	}

	void RenderCube::cleanup()
	{
		m_commandBuffer.reset();

		for ( auto & facePipeline : m_faces )
		{
			facePipeline.descriptorSet.reset();
			facePipeline.pipeline.reset();
		}

		m_descriptorPool.reset();
		m_pipelineLayout.reset();
		m_descriptorLayout.reset();
		m_matrixUbo.reset();
		m_vertexBuffer.reset();
	}

	void RenderCube::prepareFrame( ashes::RenderPass const & renderPass
		, uint32_t subpassIndex
		, uint32_t face )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( false );
		m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eRenderPassContinue
			, ashes::CommandBufferInheritanceInfo
			{
				&renderPass,
				subpassIndex,
				nullptr,
				false,
				0u,
				0u
			} );
		registerFrame( *m_commandBuffer, face );
		m_commandBuffer->end();
	}

	void RenderCube::registerFrame( ashes::CommandBuffer & commandBuffer
		, uint32_t face )const
	{
		auto & facePipeline = m_faces[face];
		commandBuffer.bindPipeline( *facePipeline.pipeline );
		commandBuffer.bindDescriptorSet( *facePipeline.descriptorSet, *m_pipelineLayout );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		doRegisterFrame( commandBuffer, face );
		commandBuffer.draw( 36u );
	}

	void RenderCube::doFillDescriptorLayoutBindings( ashes::DescriptorSetLayoutBindingArray & bindings )
	{
	}

	void RenderCube::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet
		, uint32_t face )
	{
	}

	void RenderCube::doRegisterFrame( ashes::CommandBuffer & commandBuffer
		, uint32_t face )const
	{
	}
}

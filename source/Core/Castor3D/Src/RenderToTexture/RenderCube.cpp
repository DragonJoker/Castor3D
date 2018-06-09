#include "RenderCube.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/Sampler.hpp"


#include <Buffer/StagingBuffer.hpp>
#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Descriptor/WriteDescriptorSet.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/Viewport.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>

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
			renderer::Filter const filter = nearest
				? renderer::Filter::eNearest
				: renderer::Filter::eLinear;
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
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			}

			return sampler;
		}

		renderer::UniformBufferPtr< castor::Matrix4x4f > doCreateMatrixUbo( renderer::Device const & device
			, renderer::CommandBuffer const & commandBuffer
			, bool srcIsCube )
		{
			static castor::Matrix4x4f const projection = convert( device.perspective( float( 90.0_degrees ), 1.0f, 0.1f, 10.0f ) );

			auto result = renderer::makeUniformBuffer< castor::Matrix4x4f >( device
				, 6u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eDeviceLocal );

			castor::Matrix4x4f const views[] =
			{
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ -1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, +1.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, -1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, -1.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, +1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, -1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } )
			};

			result->getData( 0u ) = projection * views[0];
			result->getData( 1u ) = projection * views[1];
			result->getData( 2u ) = projection * views[2];
			result->getData( 3u ) = projection * views[3];
			result->getData( 4u ) = projection * views[4];
			result->getData( 5u ) = projection * views[5];

			renderer::StagingBuffer stagingBuffer{ device
				, renderer::BufferTarget::eTransferSrc
				, 6u * result->getAlignedSize() };
			stagingBuffer.uploadUniformData( commandBuffer
				, result->getDatas()
				, *result );
			return result;
		}

		renderer::VertexBufferPtr< castor::Point4f > doCreateVertexBuffer( renderer::Device const & device
			, renderer::CommandBuffer const & commandBuffer )
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
			auto result = renderer::makeVertexBuffer< castor::Point4f >( device
				, uint32_t( vertexData.size() )
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eDeviceLocal );

			renderer::StagingBuffer stagingBuffer{ device
				, renderer::BufferTarget::eTransferSrc
				, result->getBuffer().getSize() };
			stagingBuffer.uploadVertexData( commandBuffer
				, vertexData
				, *result );
			return result;
		}

		renderer::VertexInputState doCreateVertexLayout()
		{
			renderer::VertexInputState result;
			result.vertexBindingDescriptions.push_back( {
				0u,
				uint32_t( sizeof( castor::Point4f ) ),
				renderer::VertexInputRate::eVertex
			} );
			result.vertexAttributeDescriptions.push_back( {
				0u,
				0u,
				renderer::Format::eR32G32B32A32_SFLOAT,
				0u
			} );
			return result;
		}

		renderer::DescriptorSetLayoutPtr doCreateDescriptorSetLayout( renderer::Device const & device )
		{
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
				{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
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

	void RenderCube::createPipelines( renderer::Extent2D const & size
		, castor::Position const & position
		, renderer::ShaderStageStateArray const & program
		, renderer::TextureView const & view
		, renderer::RenderPass const & renderPass
		, renderer::PushConstantRangeCRefArray const & pushRanges )
	{
		createPipelines( size
			, position
			, program
			, view
			, renderPass
			, pushRanges
			, renderer::DepthStencilState{ 0u, false, false } );
	}

	void RenderCube::createPipelines( renderer::Extent2D const & size
		, castor::Position const & position
		, renderer::ShaderStageStateArray const & program
		, renderer::TextureView const & view
		, renderer::RenderPass const & renderPass
		, renderer::PushConstantRangeCRefArray const & pushRanges
		, renderer::DepthStencilState const & dsState )
	{
		m_sampler->initialise();
		auto & device = getCurrentDevice( m_renderSystem );
		auto commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		m_matrixUbo = doCreateMatrixUbo( device, *commandBuffer, view.getType() == renderer::TextureViewType::eCube );
		m_vertexBuffer = doCreateVertexBuffer( device, *commandBuffer );
		auto vertexLayout = doCreateVertexLayout();

		// Initialise the descriptor set.
		renderer::DescriptorSetLayoutBindingArray bindings;
		bindings.emplace_back( 0u
			, renderer::DescriptorType::eUniformBuffer
			, renderer::ShaderStageFlag::eVertex );
		bindings.emplace_back( 1u
			, renderer::DescriptorType::eCombinedImageSampler
			, renderer::ShaderStageFlag::eFragment );
		doFillDescriptorLayoutBindings( bindings );
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_pipelineLayout = device.createPipelineLayout( { *m_descriptorLayout }, pushRanges );
		m_descriptorPool = m_descriptorLayout->createPool( 6u );
		uint32_t face = 0u;

		for ( auto & facePipeline : m_faces )
		{
			facePipeline.pipeline = m_pipelineLayout->createPipeline( renderer::GraphicsPipelineCreateInfo
			{
				program,
				renderPass,
				vertexLayout,
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
				renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eNone },
				renderer::MultisampleState{},
				renderer::ColourBlendState::createDefault(),
				{},
				renderer::DepthStencilState{ 0u, false, false },
				renderer::TessellationState{},
				renderer::Viewport{ size.width, size.height, 0, 0 },
				renderer::Scissor{ 0, 0, size.width, size.height }
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

	void RenderCube::prepareFrame( renderer::RenderPass const & renderPass
		, uint32_t subpassIndex
		, uint32_t face )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( false );
		m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eRenderPassContinue
			, renderer::CommandBufferInheritanceInfo
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

	void RenderCube::registerFrame( renderer::CommandBuffer & commandBuffer
		, uint32_t face )const
	{
		auto & facePipeline = m_faces[face];
		commandBuffer.bindPipeline( *facePipeline.pipeline );
		commandBuffer.bindDescriptorSet( *facePipeline.descriptorSet, *m_pipelineLayout );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		doRegisterFrame( commandBuffer, face );
		commandBuffer.draw( 36u );
	}

	void RenderCube::doFillDescriptorLayoutBindings( renderer::DescriptorSetLayoutBindingArray & bindings )
	{
	}

	void RenderCube::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet
		, uint32_t face )
	{
	}

	void RenderCube::doRegisterFrame( renderer::CommandBuffer & commandBuffer
		, uint32_t face )const
	{
	}
}

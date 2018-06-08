#include "RenderQuad.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/Sampler.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Descriptor/WriteDescriptorSet.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
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
		SamplerSPtr doCreateSampler( RenderSystem & renderSystem, bool nearest )
		{
			String const name = nearest
				? String{ cuT( "RenderQuad_Nearest" ) }
			: String{ cuT( "RenderQuad_Linear" ) };
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

	void RenderQuad::createPipeline( renderer::Extent2D const & size
		, castor::Position const & position
		, renderer::ShaderStageStateArray const & program
		, renderer::TextureView const & view
		, renderer::RenderPass const & renderPass
		, renderer::DescriptorSetLayoutBindingArray bindings
		, renderer::PushConstantRangeCRefArray const & pushRanges )
	{
		createPipeline( size
			, position
			, program
			, view
			, renderPass
			, bindings
			, pushRanges
			, renderer::DepthStencilState{ 0u, false, false } );
	}

	void RenderQuad::createPipeline( renderer::Extent2D const & size
		, castor::Position const & position
		, renderer::ShaderStageStateArray const & program
		, renderer::TextureView const & view
		, renderer::RenderPass const & renderPass
		, renderer::DescriptorSetLayoutBindingArray bindings
		, renderer::PushConstantRangeCRefArray const & pushRanges
		, renderer::DepthStencilState dsState )
	{
		m_sourceView = &view;
		m_sampler->initialise();
		auto & device = getCurrentDevice( m_renderSystem );
		// Initialise the vertex buffer.
		m_vertexBuffer = renderer::makeVertexBuffer< TexturedQuad::Vertex >( device
			, 4u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( device.getClipDirection() == renderer::ClipDirection::eTopDown )
		{
			std::swap( dsState.front, dsState.back );
		}

		if ( auto buffer = m_vertexBuffer->lock( 0u, 1u, renderer::MemoryMapFlag::eWrite ) )
		{
			std::copy( m_vertexData.begin(), m_vertexData.end(), buffer );
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

		// Initialise the vertex layout.
		renderer::VertexInputState vertexState;
		vertexState.vertexBindingDescriptions.push_back( {
			0u,
			sizeof( TexturedQuad::Vertex ),
			renderer::VertexInputRate::eVertex
		} );
		vertexState.vertexAttributeDescriptions.push_back( {
			0u,
			0u,
			renderer::Format::eR32G32_SFLOAT,
			offsetof( TexturedQuad::Vertex, position )
		} );
		vertexState.vertexAttributeDescriptions.push_back( {
			1u,
			0u,
			renderer::Format::eR32G32_SFLOAT,
			offsetof( TexturedQuad::Vertex, texture )
		} );

		// Initialise the descriptor set.
		auto textureBindingPoint = uint32_t( bindings.size() );

		if ( !bindings.empty() )
		{
			--textureBindingPoint;

			for ( auto & binding : bindings )
			{
				textureBindingPoint = std::max( textureBindingPoint, binding.getBindingPoint() );
			}

			++textureBindingPoint;
		}

		bindings.emplace_back( textureBindingPoint
			, renderer::DescriptorType::eCombinedImageSampler
			, renderer::ShaderStageFlag::eFragment );
		m_descriptorSetLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_pipelineLayout = device.createPipelineLayout( { *m_descriptorSetLayout }, pushRanges );
		m_descriptorSetPool = m_descriptorSetLayout->createPool( 1u );
		m_descriptorSet = m_descriptorSetPool->createDescriptorSet();
		doFillDescriptorSet( *m_descriptorSetLayout, *m_descriptorSet );
		m_descriptorSet->createBinding( m_descriptorSetLayout->getBinding( textureBindingPoint )
			, *m_sourceView
			, m_sampler->getSampler() );
		m_descriptorSet->update();

		// Initialise the pipeline.
		auto bdState = renderer::ColourBlendState::createDefault();
		m_pipeline = m_pipelineLayout->createPipeline( 
		{
			program,
			renderPass,
			vertexState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
			renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eNone },
			renderer::MultisampleState{},
			std::move( bdState ),
			{},
			dsState,
			std::nullopt,
			renderer::Viewport{ size.width, size.height, position.x(), position.y() },
			renderer::Scissor{ position.x(), position.y(), size.width, size.height }
		} );
	}

	void RenderQuad::prepareFrame( renderer::RenderPass const & renderPass
		, uint32_t subpassIndex )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( false );
		m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eRenderPassContinue
			, renderer::CommandBufferInheritanceInfo{ &renderPass, subpassIndex, nullptr, false, 0u, 0u } );
		registerFrame( *m_commandBuffer );
		m_commandBuffer->end();
	}

	void RenderQuad::registerFrame( renderer::CommandBuffer & commandBuffer )const
	{
		// Put source image in shader input layout.
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		doRegisterFrame( commandBuffer );
		commandBuffer.draw( 4u );
	}

	void RenderQuad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
	}

	void RenderQuad::doRegisterFrame( renderer::CommandBuffer & commandBuffer )const
	{
	}
}

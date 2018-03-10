#include "RenderQuad.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/Sampler.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
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
		, bool invertU )
		: m_renderSystem{ renderSystem }
		, m_vertexData
		{
			{
				{ Point2f{ -1.0, -1.0 }, Point2f{ ( invertU ? 1.0 : 0.0 ), 0.0 } },
				{ Point2f{ -1.0, +1.0 }, Point2f{ ( invertU ? 1.0 : 0.0 ), 1.0 } },
				{ Point2f{ +1.0, -1.0 }, Point2f{ ( invertU ? 0.0 : 1.0 ), 0.0 } },
				{ Point2f{ +1.0, +1.0 }, Point2f{ ( invertU ? 0.0 : 1.0 ), 1.0 } },
			}
		}
		, m_sampler{ doCreateSampler( m_renderSystem, nearest ) }
	{
	}

	RenderQuad::~RenderQuad()
	{
		if ( m_pipeline )
		{
			m_pipeline->cleanup();
		}
	}

	void RenderQuad::createPipeline( renderer::Extent2D const & size
		, castor::Position const & position
		, renderer::ShaderStageStateArray const & program
		, renderer::TextureView const & view
		, renderer::RenderPass const & renderPass
		, renderer::DescriptorSetLayoutBindingArray bindings
		, renderer::PushConstantRangeCRefArray const & pushRanges )
	{
		auto & device = *m_renderSystem.getCurrentDevice();
		// Initialise the vertex buffer.
		m_vertexBuffer = renderer::makeVertexBuffer< TexturedQuad >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto buffer = m_vertexBuffer->lock( 0u, 1u, renderer::MemoryMapFlag::eWrite ) )
		{
			*buffer = m_vertexData;
			m_vertexBuffer->unlock();
		}

		// Initialise the vertex layout.
		m_vertexLayout = renderer::makeLayout< TexturedQuad >( 0u );
		m_vertexLayout->createAttribute( 0u, renderer::Format::eR32G32_SFLOAT, offsetof( TexturedQuad::Vertex, position ) );
		m_vertexLayout->createAttribute( 0u, renderer::Format::eR32G32_SFLOAT, offsetof( TexturedQuad::Vertex, texture ) );

		// Initialise the descriptor set.
		auto textureBindingPoint = uint32_t( bindings.size() );
		bindings.emplace_back( textureBindingPoint
			, renderer::DescriptorType::eCombinedImageSampler
			, renderer::ShaderStageFlag::eFragment );
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( 1u );
		m_descriptorSet = m_descriptorPool->createDescriptorSet();
		doFillDescriptorSet( *m_descriptorLayout, *m_descriptorSet );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( textureBindingPoint )
			, view
			, m_sampler->getSampler() );
		m_descriptorSet->update();

		// Initialise the pipeline.
		auto bdState = renderer::ColourBlendState::createDefault();
		m_pipeline = std::make_unique< RenderPipeline >( m_renderSystem
			, renderer::DepthStencilState{ 0u, false, false }
			, renderer::RasterisationState{}
			, std::move( bdState )
			, renderer::MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->setPushConstantRanges( pushRanges );
		m_pipeline->setDescriptorSetLayouts( { *m_descriptorLayout } );
		m_pipeline->setVertexLayouts( { *m_vertexLayout } );
		m_pipeline->setViewport( renderer::Viewport
		{
			size.width,
			size.height,
			position.x(),
			position.y(),
		} );
		m_pipeline->setScissor( renderer::Scissor
		{
			position.x(),
			position.y(),
			size.width,
			size.height,
		} );
		m_pipeline->initialise( renderPass, renderer::PrimitiveTopology::eTriangleStrip );
	}

	void RenderQuad::prepareFrame()
	{
		auto & device = *m_renderSystem.getCurrentDevice();
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( false );
		registerFrame( *m_commandBuffer );
	}

	void RenderQuad::registerFrame( renderer::CommandBuffer & commandBuffer )
	{
		commandBuffer.bindPipeline( m_pipeline->getPipeline() );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		commandBuffer.bindDescriptorSet( *m_descriptorSet, m_pipeline->getPipelineLayout() );
		doRegisterFrame( commandBuffer );
		commandBuffer.draw( 4u );
	}

	void RenderQuad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
	}

	void RenderQuad::doRegisterFrame( renderer::CommandBuffer & commandBuffer )
	{
	}
}

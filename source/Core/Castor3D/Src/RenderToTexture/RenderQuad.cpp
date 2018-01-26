#include "RenderQuad.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/Sampler.hpp"

#include <Buffer/GeometryBuffers.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetBinding.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/DepthStencilState.hpp>
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
	{
		// Initialise the sampler.
		String const name = nearest
			? String{ cuT( "RenderQuad_Nearest" ) }
			: String{ cuT( "RenderQuad_Linear" ) };
		renderer::Filter const filter = nearest
			? renderer::Filter::eNearest
			: renderer::Filter::eLinear;
		auto & cache = renderSystem.getEngine()->getSamplerCache();

		if ( cache.has( name ) )
		{
			m_sampler = cache.find( name );
		}
		else
		{
			m_sampler = cache.add( name );
			m_sampler->setMinFilter( filter );
			m_sampler->setMagFilter( filter );
			m_sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			m_sampler->setWrapT( renderer::WrapMode::eClampToEdge );
		}
	}

	void RenderQuad::createPipeline( castor::Size const & size
		, castor::Position const & position
		, renderer::ShaderProgram const & program
		, renderer::TextureView const & view
		, renderer::RenderPass const & renderPass
		, std::vector< renderer::DescriptorSetLayoutBinding > bindings )
	{
		auto & device = *m_renderSystem.getCurrentDevice();
		// Initialise the vertex buffer.
		m_vertexBuffer = renderer::makeVertexBuffer< TexturedQuad >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		// Initialise the vertex layout.
		m_vertexLayout = device.createVertexLayout( 0u, sizeof( TexturedQuad ) );
		m_vertexLayout->createAttribute< Point2f >( 0u, offsetof( TexturedVertex, m_position ) );
		m_vertexLayout->createAttribute< Point2f >( 0u, offsetof( TexturedVertex, m_texture ) );

		// Initialise the geometry buffers.
		m_geometryBuffers = device.createGeometryBuffers( *m_vertexBuffer
			, 0u
			, *m_vertexLayout );
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
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout );
		m_pipeline = device.createPipeline( *m_pipelineLayout
			, program
			, { *m_vertexLayout }
			, renderPass
			, renderer::PrimitiveTopology::eTriangleStrip );
		m_pipeline->depthStencilState( renderer::DepthStencilState{ 0u, false, false } );
		m_pipeline->viewport( renderer::Viewport
		{
			size.getWidth(),
			size.getHeight(),
			position.x(),
			position.y(),
		} );
		m_pipeline->scissor( renderer::Scissor
		{
			position.x(),
			position.y(),
			size.getWidth(),
			size.getHeight(),
		} );
		m_pipeline->finish();
	}

	void RenderQuad::registerFrame( renderer::CommandBuffer & commandBuffer )
	{
		commandBuffer.bindPipeline( *m_pipeline );
		commandBuffer.bindGeometryBuffers( *m_geometryBuffers );
		commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		commandBuffer.draw( 4u, 1u, 0u, 0u );
	}

	void RenderQuad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
	}
}

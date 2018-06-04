#include "SmaaPostEffect.hpp"

#include "ColourEdgeDetection.hpp"
#include "DepthEdgeDetection.hpp"
#include "LumaEdgeDetection.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Texture/Sampler.hpp>

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>

#include <numeric>

using namespace castor;

#define C3D_DebugEdgeDetection 0
#define C3D_DebugBlendingWeightCalculation 1
#define C3D_DebugNeighbourhoodBlending 1

namespace smaa
{
	//*********************************************************************************************

	namespace
	{
		castor3d::SamplerSPtr doCreateSampler( castor3d::Engine & engine
			, bool linear
			, castor::String const & name )
		{
			castor3d::SamplerSPtr result;
			renderer::Filter filter = linear
				? renderer::Filter::eLinear
				: renderer::Filter::eNearest;

			if ( !engine.getSamplerCache().has( name ) )
			{
				result = engine.getSamplerCache().add( name );
				result->setMinFilter( renderer::Filter::eNearest );
				result->setMagFilter( renderer::Filter::eNearest );
				result->setWrapS( renderer::WrapMode::eClampToBorder );
				result->setWrapT( renderer::WrapMode::eClampToBorder );
				result->setWrapR( renderer::WrapMode::eClampToBorder );
			}
			else
			{
				result = engine.getSamplerCache().find( name );
			}

			return result;
		}

		glsl::Shader doGetCopyVertexShader( castor3d::RenderSystem & renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;
				} );
			return writer.finalise();
		}

		glsl::Shader doGetCopyPixelShader( castor3d::RenderSystem & renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto c3d_map = writer.declSampler< Sampler2D >( cuT( "c3d_map" ), 0u, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					pxl_fragColour = texture( c3d_map, vtx_texture );
				} );
			return writer.finalise();
		}

		renderer::RenderPassPtr doCreateRenderPass( renderer::Device const & device
			, renderer::Format format )
		{
			renderer::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, castor3d::TextureLayout const & texture )
		{
			renderer::FrameBufferAttachmentArray attaches
			{
				{ *renderPass.getAttachments().begin(), texture.getDefaultView() }
			};
			return renderPass.createFrameBuffer( { texture.getWidth(), texture.getHeight() }
				, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "smaa" );
	String PostEffect::Name = cuT( "SMAA PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters
			, true }
		, m_config{ parameters }
		, m_pointSampler{ doCreateSampler( *renderSystem.getEngine(), false, cuT( "SMAA_Point" ) ) }
		, m_linearSampler{ doCreateSampler( *renderSystem.getEngine(), false, cuT( "SMAA_Linear" ) ) }
	{
		m_passesCount = 5u;

		if ( m_config.data.mode == Mode::eT2X )
		{
			++m_passesCount;
		}
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, parameters );
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		if ( m_edgeDetection )
		{
			m_edgeDetection->accept( visitor );
		}

		if ( m_blendingWeightCalculation )
		{
			m_blendingWeightCalculation->accept( visitor );
		}

		if ( m_neighbourhoodBlending )
		{
			m_neighbourhoodBlending->accept( visitor );
		}

		for ( auto & reproject : m_reproject )
		{
			reproject->accept( visitor );
		}
	}

	void PostEffect::update( castor::Nanoseconds const & elapsedTime )
	{
		if ( m_config.maxSubsampleIndices > 1u )
		{
			std::swap( m_commands, m_commandBuffers[m_curIndex] );
			m_curIndex = ( m_config.subsampleIndex + 1 ) % m_config.maxSubsampleIndices;
			std::swap( m_commands, m_commandBuffers[m_curIndex] );
		}

		if ( m_blendingWeightCalculation )
		{
			m_blendingWeightCalculation->update( m_config.subsampleIndices[m_config.subsampleIndex] );
			m_config.subsampleIndex = m_curIndex;
			m_renderTarget.setJitter( m_config.jitters[m_config.subsampleIndex] );
		}
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_srgbTextureView = &m_target->getDefaultView();
		m_hdrTextureView = &m_renderTarget.getTechnique()->getResult().getDefaultView();
		m_pointSampler->initialise();
		m_linearSampler->initialise();

		switch ( m_config.data.edgeDetection )
		{
		case EdgeDetectionType::eDepth:
			m_edgeDetection = std::make_unique< DepthEdgeDetection >( m_renderTarget
				, m_renderTarget.getTechnique()->getDepth().getDefaultView()
				, m_linearSampler
				, m_config );
			break;

		case EdgeDetectionType::eColour:
			m_edgeDetection = std::make_unique< ColourEdgeDetection >( m_renderTarget
				, *m_srgbTextureView
				, doGetPredicationTexture()
				, m_linearSampler
				, m_config );
			break;

		case EdgeDetectionType::eLuma:
			m_edgeDetection = std::make_unique< LumaEdgeDetection >( m_renderTarget
				, *m_srgbTextureView
				, doGetPredicationTexture()
				, m_linearSampler
				, m_config );
			break;
		}

#if !C3D_DebugEdgeDetection
		m_blendingWeightCalculation = std::make_unique< BlendingWeightCalculation >( m_renderTarget
			, m_edgeDetection->getSurface()->getDefaultView()
			, m_edgeDetection->getDepth()
			, m_linearSampler
			, m_config );
		auto * velocityView = doGetVelocityView();

#	if !C3D_DebugBlendingWeightCalculation
		m_neighbourhoodBlending = std::make_unique< NeighbourhoodBlending >( m_renderTarget
			, *m_srgbTextureView
			, m_blendingWeightCalculation->getSurface()->getDefaultView()
			, velocityView
			, m_linearSampler
			, m_config );

#		if !C3D_DebugNeighbourhoodBlending
		if ( m_config.mode == Mode::eT2X )
		{
			for ( uint32_t i = 0u; i < m_config.maxSubsampleIndices; ++i )
			{
				auto & previous = i == 0u
					? m_neighbourhoodBlending->getSurface( m_config.maxSubsampleIndices - 1 )->getDefaultView()
					: m_neighbourhoodBlending->getSurface( i - 1 )->getDefaultView();
				auto & current = m_neighbourhoodBlending->getSurface( i )->getDefaultView();
				m_reproject.emplace_back( std::make_unique< Reproject >( m_renderTarget
					, current
					, previous
					, velocityView
					, m_linearSampler
					, m_config ) );
			}
		}
#		endif
#	endif
#endif

		auto & device = getCurrentDevice( *this );
		m_copyProgram.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		m_copyProgram.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		m_copyProgram[0].module->loadShader( doGetCopyVertexShader( *getRenderSystem() ).getSource() );
		m_copyProgram[1].module->loadShader( doGetCopyPixelShader( *getRenderSystem() ).getSource() );
		m_copyRenderPass = doCreateRenderPass( device, m_target->getPixelFormat() );
		m_copyFrameBuffer = doCreateFrameBuffer( *m_copyRenderPass, *m_target );

		doBuildCommandBuffers( timer );
		m_result = m_target;
		return true;
	}

	void PostEffect::doCleanup()
	{
		m_copyQuads.clear();
		m_copyFrameBuffer.reset();
		m_copyRenderPass.reset();
		m_copyProgram.clear();
		m_reproject.clear();
		m_neighbourhoodBlending.reset();
		m_blendingWeightCalculation.reset();
		m_edgeDetection.reset();
	}

	void PostEffect::doBuildCommandBuffers( castor3d::RenderPassTimer const & timer )
	{
		for ( uint32_t i = 0u; i < m_config.maxSubsampleIndices; ++i )
		{
			m_commandBuffers.emplace_back( doBuildCommandBuffer( timer, i ) );
		}

		if ( m_config.maxSubsampleIndices == 1 )
		{
			m_commands = std::move( m_commandBuffers[0] );
		}
		else
		{
			m_curIndex = m_config.maxSubsampleIndices - 1u;
			m_config.subsampleIndex = m_curIndex;
			std::swap( m_commands, m_commandBuffers[m_curIndex] );
		}
	}

	castor3d::CommandsSemaphoreArray PostEffect::doBuildCommandBuffer( castor3d::RenderPassTimer const & timer
		, uint32_t index )
	{
		castor3d::CommandsSemaphoreArray result;
		uint32_t passIndex = 0u;

		if ( m_edgeDetection )
		{
			result.emplace_back( m_edgeDetection->prepareCommands( timer, passIndex++ ) );
			m_smaaResult = m_edgeDetection->getSurface().get();
		}

		if ( m_blendingWeightCalculation )
		{
			result.emplace_back( m_blendingWeightCalculation->prepareCommands( timer, passIndex++ ) );
			m_smaaResult = m_blendingWeightCalculation->getSurface().get();
		}

		if ( m_neighbourhoodBlending )
		{
			result.emplace_back( m_neighbourhoodBlending->prepareCommands( timer, passIndex++, index ) );
			m_smaaResult = m_neighbourhoodBlending->getSurface( index ).get();
		}

		if ( !m_reproject.empty() )
		{
			if ( m_config.data.mode == Mode::eT2X )
			{
				auto & reproject = *m_reproject[index];
				result.emplace_back( reproject.prepareCommands( timer, passIndex++ ) );
				m_smaaResult = reproject.getSurface().get();
			}
		}

		auto & device = getCurrentDevice( m_renderTarget );
		renderer::DescriptorSetLayoutBindingArray bindings;
		auto copyQuad = std::make_shared< castor3d::RenderQuad >( *getRenderSystem(), true );
		copyQuad->createPipeline( { m_renderTarget.getSize().getWidth(), m_renderTarget.getSize().getHeight() }
			, {}
			, m_copyProgram
			, m_smaaResult->getDefaultView()
			, *m_copyRenderPass
			, bindings
			, {} );
		m_copyQuads.push_back( copyQuad );

		castor3d::CommandsSemaphore copyCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & copyCmd = *copyCommands.commandBuffer;

		copyCmd.begin();
		timer.beginPass( copyCmd, passIndex++ );
		copyCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::PipelineStageFlag::eFragmentShader
			, m_smaaResult->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
		copyCmd.beginRenderPass( *m_copyRenderPass
			, *m_copyFrameBuffer
			, { renderer::ClearColorValue{} }
			, renderer::SubpassContents::eInline );
		copyQuad->registerFrame( copyCmd );
		copyCmd.endRenderPass();
		timer.endPass( copyCmd, 4u );
		copyCmd.end();
		result.emplace_back( std::move( copyCommands ) );

		return result;
	}

	bool PostEffect::doWriteInto( TextFile & p_file )
	{
		return true;
	}

	renderer::Texture const * PostEffect::doGetPredicationTexture()
	{
		renderer::Texture const * predication = nullptr;

		switch ( m_config.data.mode )
		{
		case Mode::eT2X:
			predication = &m_renderTarget.getTechnique()->getDepth().getTexture();
			break;
		}

		return predication;
	}

	renderer::TextureView const * PostEffect::doGetVelocityView()
	{
		renderer::TextureView const * velocityView = nullptr;

		switch ( m_config.data.mode )
		{
		case Mode::eT2X:
			velocityView = &m_renderTarget.getVelocity().getTexture()->getDefaultView();
			break;
		}

		return velocityView;
	}
}

#include "SmaaPostEffect.hpp"

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
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

using namespace castor;

namespace smaa
{
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
	{
		String preset;

		if ( !parameters.get( cuT( "preset" ), preset ) )
		{
			preset = "high";
		}

		if ( preset == cuT( "low" ) )
		{
			m_smaaThreshold = 0.15f;
			m_smaaMaxSearchSteps = 4;
			m_smaaMaxSearchStepsDiag = 0;
			m_smaaCornerRounding = 100;
		}
		else if ( preset == cuT( "medium" ) )
		{
			m_smaaThreshold = 0.1f;
			m_smaaMaxSearchSteps = 8;
			m_smaaMaxSearchStepsDiag = 0;
			m_smaaCornerRounding = 100;
		}
		else if ( preset == cuT( "high" ) )
		{
			m_smaaThreshold = 0.1f;
			m_smaaMaxSearchSteps = 16;
			m_smaaMaxSearchStepsDiag = 8;
			m_smaaCornerRounding = 25;
		}
		else if ( preset == cuT( "ultra" ) )
		{
			m_smaaThreshold = 0.05f;
			m_smaaMaxSearchSteps = 32;
			m_smaaMaxSearchStepsDiag = 16;
			m_smaaCornerRounding = 25;
		}
		else if ( preset == cuT( "custom" ) )
		{
			parameters.get( cuT( "threshold" ), m_smaaThreshold );
			parameters.get( cuT( "maxSearchSteps" ), m_smaaMaxSearchSteps );
			parameters.get( cuT( "maxSearchStepsDiag" ), m_smaaMaxSearchStepsDiag );
			parameters.get( cuT( "cornerRounding" ), m_smaaCornerRounding );
		}

		String mode;

		if ( parameters.get( cuT( "mode" ), mode ) )
		{
			if ( mode == cuT( "T2X" ) )
			{
				m_mode = Mode::eT2X;
			}
			else if ( mode == cuT( "S2X" ) )
			{
				m_mode = Mode::eS2X;
			}
			else if ( mode == cuT( "4X" ) )
			{
				m_mode = Mode::e4X;
			}
		}

		m_passesCount = 4u;

		if ( m_mode == Mode::eT2X )
		{
			++m_passesCount;
			parameters.get( cuT( "reprojection" ), m_reprojection );
			parameters.get( cuT( "reprojectionWeightScale" ), m_reprojectionWeightScale );
		}

		doInitialiseSubsampleIndices();
		String name = cuT( "SMAA_Point" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_pointSampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_pointSampler->setMinFilter( renderer::Filter::eNearest );
			m_pointSampler->setMagFilter( renderer::Filter::eNearest );
			m_pointSampler->setWrapS( renderer::WrapMode::eClampToBorder );
			m_pointSampler->setWrapT( renderer::WrapMode::eClampToBorder );
			m_pointSampler->setWrapR( renderer::WrapMode::eClampToBorder );
		}
		else
		{
			m_pointSampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		name = cuT( "SMAA_Linear" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_linearSampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_linearSampler->setMinFilter( renderer::Filter::eLinear );
			m_linearSampler->setMagFilter( renderer::Filter::eLinear );
			m_linearSampler->setWrapS( renderer::WrapMode::eClampToBorder );
			m_linearSampler->setWrapT( renderer::WrapMode::eClampToBorder );
			m_linearSampler->setWrapR( renderer::WrapMode::eClampToBorder );
		}
		else
		{
			m_linearSampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
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
		visitor.visit( cuT( "EdgeDetection" )
			, renderer::ShaderStageFlag::eVertex
			, m_edgeDetection->getVertexShader() );
		visitor.visit( cuT( "EdgeDetection" )
			, renderer::ShaderStageFlag::eFragment
			, m_edgeDetection->getPixelShader() );

		visitor.visit( cuT( "BlendingWeightCalculation" )
			, renderer::ShaderStageFlag::eVertex
			, m_blendingWeightCalculation->getVertexShader() );
		visitor.visit( cuT( "BlendingWeightCalculation" )
			, renderer::ShaderStageFlag::eFragment
			, m_blendingWeightCalculation->getPixelShader() );

		visitor.visit( cuT( "NeighbourhoodBlending" )
			, renderer::ShaderStageFlag::eVertex
			, m_neighbourhoodBlending->getVertexShader() );
		visitor.visit( cuT( "NeighbourhoodBlending" )
			, renderer::ShaderStageFlag::eFragment
			, m_neighbourhoodBlending->getPixelShader() );

		if ( m_mode == Mode::eT2X )
		{
			uint32_t index = 0;

			for ( auto & reproject : m_reproject )
			{
				visitor.visit( cuT( "Reproject " ) + string::toString( index, std::locale{ "C" } )
					, renderer::ShaderStageFlag::eVertex
					, reproject->getVertexShader() );
				visitor.visit( cuT( "Reproject " ) + string::toString( index, std::locale{ "C" } )
					, renderer::ShaderStageFlag::eFragment
					, reproject->getPixelShader() );
				++index;
			}
		}
	}

	void PostEffect::update( castor::Nanoseconds const & elapsedTime )
	{
		if ( m_maxSubsampleIndices > 1u )
		{
			std::swap( m_commands, m_commandBuffers[m_curIndex] );
			m_curIndex = ( m_subsampleIndex + 1 ) % m_maxSubsampleIndices;
			std::swap( m_commands, m_commandBuffers[m_curIndex] );
		}

		m_blendingWeightCalculation->update( m_smaaSubsampleIndices[m_subsampleIndex] );
		m_subsampleIndex = m_curIndex;
		m_renderTarget.setJitter( m_jitters[m_subsampleIndex] );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_srgbTexture = &m_target->getDefaultView();
		m_hdrTexture = &m_renderTarget.getTechnique()->getResult().getDefaultView();

		m_pointSampler->initialise();
		m_linearSampler->initialise();
		doInitialiseEdgeDetection();
		doInitialiseBlendingWeightCalculation();
		doInitialiseNeighbourhoodBlending();

		if ( m_mode == Mode::eT2X )
		{
			doInitialiseReproject();
		}

		doBuildCommandBuffers( timer );
		m_result = m_target;
		return true;
	}

	void PostEffect::doCleanup()
	{
		m_reproject.clear();
		m_neighbourhoodBlending.reset();
		m_blendingWeightCalculation.reset();
		m_edgeDetection.reset();
	}

	void PostEffect::doBuildCommandBuffers( castor3d::RenderPassTimer const & timer )
	{
		for ( uint32_t i = 0u; i < m_maxSubsampleIndices; ++i )
		{
			m_commandBuffers.emplace_back( doBuildCommandBuffer( timer, i ) );
		}

		if ( m_maxSubsampleIndices == 1 )
		{
			m_commands = std::move( m_commandBuffers[0] );
		}
		else
		{
			m_curIndex = m_maxSubsampleIndices - 1u;
			m_subsampleIndex = m_curIndex;
			std::swap( m_commands, m_commandBuffers[m_curIndex] );
		}
	}

	castor3d::CommandsSemaphoreArray PostEffect::doBuildCommandBuffer( castor3d::RenderPassTimer const & timer
		, uint32_t index )
	{
		auto & renderSystem = *m_renderTarget.getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		castor3d::CommandsSemaphoreArray result;

		castor3d::CommandsSemaphore edgeDetectionCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & edgeDetectionCmd = *edgeDetectionCommands.commandBuffer;

		if ( edgeDetectionCmd.begin() )
		{
			timer.beginPass( edgeDetectionCmd, 0u );
			// Put SRGB image in shader input layout.
			edgeDetectionCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_srgbTexture->makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			// Put HDR image in shader input layout.
			edgeDetectionCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_hdrTexture->makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );

			edgeDetectionCmd.beginRenderPass( m_edgeDetection->getRenderPass()
				, m_edgeDetection->getFrameBuffer()
				, { renderer::ClearColorValue{}, renderer::DepthStencilClearValue{ 1.0f, 0 } }
				, renderer::SubpassContents::eInline );
			m_edgeDetection->registerFrame( edgeDetectionCmd );
			edgeDetectionCmd.endRenderPass();
			timer.endPass( edgeDetectionCmd, 0u );
			edgeDetectionCmd.end();
			result.emplace_back( std::move( edgeDetectionCommands ) );
		}

		castor3d::CommandsSemaphore blendingWeightCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & blendingWeightCmd = *blendingWeightCommands.commandBuffer;

		if ( blendingWeightCmd.begin() )
		{
			timer.beginPass( blendingWeightCmd, 1u );
			// Put edge detection image in shader input layout.
			blendingWeightCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_edgeDetection->getSurface()->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			blendingWeightCmd.beginRenderPass( m_blendingWeightCalculation->getRenderPass()
				, m_blendingWeightCalculation->getFrameBuffer()
				, { renderer::ClearColorValue{}, renderer::DepthStencilClearValue{ 1.0f, 0 } }
				, renderer::SubpassContents::eInline );
			m_blendingWeightCalculation->registerFrame( blendingWeightCmd );
			blendingWeightCmd.endRenderPass();
			timer.endPass( blendingWeightCmd, 1u );
			blendingWeightCmd.end();
			result.emplace_back( std::move( blendingWeightCommands ) );
		}

		castor3d::CommandsSemaphore neighbourhoodBlendingCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & neighbourhoodBlendingCmd = *neighbourhoodBlendingCommands.commandBuffer;

		if ( neighbourhoodBlendingCmd.begin() )
		{
			timer.beginPass( neighbourhoodBlendingCmd, 2u );
			// Put blending weights image in shader input layout.
			neighbourhoodBlendingCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_blendingWeightCalculation->getSurface()->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			neighbourhoodBlendingCmd.beginRenderPass( m_neighbourhoodBlending->getRenderPass()
				, m_neighbourhoodBlending->getFrameBuffer( index )
				, { renderer::ClearColorValue{} }
				, renderer::SubpassContents::eInline );
			m_neighbourhoodBlending->registerFrame( neighbourhoodBlendingCmd );
			neighbourhoodBlendingCmd.endRenderPass();
			timer.endPass( neighbourhoodBlendingCmd, 2u );
			neighbourhoodBlendingCmd.end();
			result.emplace_back( std::move( neighbourhoodBlendingCommands ) );
		}

		auto resultSurface = m_neighbourhoodBlending->getSurface( index );

		if ( m_mode == Mode::eT2X )
		{
			castor3d::CommandsSemaphore reprojectCommands
			{
				device.getGraphicsCommandPool().createCommandBuffer(),
				device.createSemaphore()
			};
			auto & reprojectCmd = *reprojectCommands.commandBuffer;

			reprojectCmd.begin();
			timer.beginPass( reprojectCmd, 3u );
			auto & reproject = *m_reproject[index];
			// Put neighbourhood image in shader input layout.
			reprojectCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, resultSurface->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			reprojectCmd.beginRenderPass( reproject.getRenderPass()
				, reproject.getFrameBuffer()
				, { renderer::ClearColorValue{} }
				, renderer::SubpassContents::eInline );
			reproject.registerFrame( reprojectCmd );
			reprojectCmd.endRenderPass();
			timer.endPass( reprojectCmd, 3u );
			reprojectCmd.end();
			result.emplace_back( std::move( reprojectCommands ) );
			resultSurface = reproject.getSurface();
		}

		castor3d::CommandsSemaphore copyCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & copyCmd = *copyCommands.commandBuffer;

		copyCmd.begin();
		timer.beginPass( copyCmd, 4u );
		doCopyResultToTarget( resultSurface->getDefaultView()
			, copyCmd );
		timer.endPass( copyCmd, 4u );
		copyCmd.end();
		result.emplace_back( std::move( copyCommands ) );

		return result;
	}

	bool PostEffect::doWriteInto( TextFile & p_file )
	{
		return true;
	}

	void PostEffect::doInitialiseSubsampleIndices()
	{
		switch ( m_mode )
		{
		case Mode::e1X:
			m_smaaSubsampleIndices[0] = Point4i{ 0, 0, 0, 0 };
			m_maxSubsampleIndices = 1u;
			break;

		case Mode::eT2X:
		case Mode::eS2X:
			/***
			* Sample positions (bottom-to-top y axis):
			*   _______
			*  | S1    |  S0:  0.25    -0.25
			*  |       |  S1: -0.25     0.25
			*  |____S0_|
			*/
			m_smaaSubsampleIndices[0] = Point4i{ 1, 1, 1, 0 }; // S0
			m_smaaSubsampleIndices[1] = Point4i{ 2, 2, 2, 0 }; // S1
			m_maxSubsampleIndices = 2u;
			// (it's 1 for the horizontal slot of S0 because horizontal
			//  blending is reversed: positive numbers point to the right)
			break;

		case Mode::e4X:
			/***
			* Sample positions (bottom-to-top y axis):
			*   ________
			*  |  S1    |  S0:  0.3750   -0.1250
			*  |      S0|  S1: -0.1250    0.3750
			*  |S3      |  S2:  0.1250   -0.3750
			*  |____S2__|  S3: -0.3750    0.1250
			*/
			m_smaaSubsampleIndices[0] = Point4i{ 5, 3, 1, 3 }; // S0
			m_smaaSubsampleIndices[1] = Point4i{ 4, 6, 2, 3 }; // S1
			m_smaaSubsampleIndices[2] = Point4i{ 3, 5, 1, 4 }; // S2
			m_smaaSubsampleIndices[3] = Point4i{ 6, 4, 2, 4 }; // S3
			m_maxSubsampleIndices = 4u;
			break;
		}

		switch ( m_mode )
		{
		case Mode::e1X:
		case Mode::eS2X:
			m_jitters.emplace_back( 0.0, 0.0 );
			break;

		case Mode::eT2X:
			m_jitters.emplace_back( 0.25, -0.25 );
			m_jitters.emplace_back( -0.25, 0.25 );
			break;

		case Mode::e4X:
			m_jitters.emplace_back( 0.125, -0.125 );
			m_jitters.emplace_back( -0.125, 0.125 );
			break;
		}
	}

	void PostEffect::doInitialiseEdgeDetection()
	{
		renderer::Texture const * predication = nullptr;

		switch ( m_mode )
		{
		case Mode::eT2X:
			predication = &m_renderTarget.getTechnique()->getDepth().getTexture();
			break;
		}

		m_edgeDetection = std::make_unique< EdgeDetection >( m_renderTarget
			, *m_hdrTexture
			, predication
			, m_linearSampler
			, m_smaaThreshold
			, m_smaaPredicationThreshold
			, m_smaaPredicationScale
			, m_smaaPredicationStrength );
	}

	void PostEffect::doInitialiseBlendingWeightCalculation()
	{
		 m_blendingWeightCalculation = std::make_unique< BlendingWeightCalculation >( m_renderTarget
			, m_edgeDetection->getSurface()->getDefaultView()
			 , m_edgeDetection->getDepth()
			, m_linearSampler
			, m_smaaMaxSearchSteps
			, m_smaaCornerRounding
			, m_smaaMaxSearchStepsDiag );
	}

	void PostEffect::doInitialiseNeighbourhoodBlending()
	{
		renderer::TextureView const * velocityView = nullptr;

		if ( m_mode == Mode::eT2X )
		{
			velocityView = &m_renderTarget.getVelocity().getTexture()->getDefaultView();
		}

		m_neighbourhoodBlending = std::make_unique< NeighbourhoodBlending >( m_renderTarget
			, m_target->getDefaultView()
			, m_blendingWeightCalculation->getSurface()->getDefaultView()
			, velocityView
			, m_linearSampler
			, m_maxSubsampleIndices );
	}

	void PostEffect::doInitialiseReproject()
	{
		renderer::TextureView const * velocityView = nullptr;

		if ( m_mode == Mode::eT2X )
		{
			velocityView = &m_renderTarget.getVelocity().getTexture()->getDefaultView();
		}

		for ( uint32_t i = 0u; i < m_maxSubsampleIndices; ++i )
		{
			auto & previous = i == 0u
				? m_neighbourhoodBlending->getSurface( m_maxSubsampleIndices - 1 )->getDefaultView()
				: m_neighbourhoodBlending->getSurface( i - 1 )->getDefaultView();
			auto & current = m_neighbourhoodBlending->getSurface( i )->getDefaultView();
			m_reproject.emplace_back( std::make_unique< Reproject >( m_renderTarget
				, current
				, previous
				, velocityView
				, m_linearSampler
				, m_reprojectionWeightScale ) );
		}
	}
}

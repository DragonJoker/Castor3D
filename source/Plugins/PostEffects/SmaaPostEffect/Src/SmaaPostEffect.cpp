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
#include <Shader/GlslToSpv.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

#define C3D_DebugEdgeDetection 0
#define C3D_DebugBlendingWeightCalculation 0
#define C3D_DebugNeighbourhoodBlending 0
#define C3D_DebugVelocity 0

namespace smaa
{
	//*********************************************************************************************

	namespace
	{
		castor3d::ShaderPtr doGetCopyVertexShader()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto texcoord = writer.declInput< Vec2 >( "texcoord", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		castor3d::ShaderPtr doGetCopyPixelShader( SmaaConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto c3d_map = writer.declSampledImage< FImg2DRgba32 >( "c3d_map", 0u, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					if ( config.data.mode == Mode::eT2X
						&& config.data.enableReprojection
						&& C3D_DebugVelocity )
					{
						pxl_fragColour = vec4( texture( c3d_map, vtx_texture ).xy(), 0.0, 1.0 );
					}
					else
					{
						pxl_fragColour = texture( c3d_map, vtx_texture );
					}
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::RenderPassPtr doCreateRenderPass( ashes::Device const & device
			, ashes::Format format )
		{
			ashes::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eMemoryRead;
			renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
			renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eBottomOfPipe;
			renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
			renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eMemoryRead;
			renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eBottomOfPipe;
			renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, castor3d::TextureLayout const & texture )
		{
			ashes::FrameBufferAttachmentArray attaches
			{
				{ *renderPass.getAttachments().begin(), texture.getDefaultView() }
			};
			return renderPass.createFrameBuffer( { texture.getWidth(), texture.getHeight() }
				, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	castor::String getName( Mode mode )
	{
		castor::String result;

		switch ( mode )
		{
		case Mode::e1X:
			result = cuT( "1X" );
			break;

		case Mode::eT2X:
			result = cuT( "T2X" );
			break;

		case Mode::eS2X:
			result = cuT( "S2X" );
			break;

		case Mode::e4X:
			result = cuT( "4X" );
			break;
		}

		return result;
	}

	castor::String getName( Preset preset )
	{
		castor::String result;

		switch ( preset )
		{
		case Preset::eLow:
			result = cuT( "low" );
			break;

		case Preset::eMedium:
			result = cuT( "medium" );
			break;

		case Preset::eHigh:
			result = cuT( "high" );
			break;

		case Preset::eUltra:
			result = cuT( "ultra" );
			break;

		case Preset::eCustom:
			result = cuT( "custom" );
			break;
		}

		return result;
	}

	castor::String getName( EdgeDetectionType detection )
	{
		castor::String result;

		switch ( detection )
		{
		case EdgeDetectionType::eDepth:
			result = cuT( "depth" );
			break;

		case EdgeDetectionType::eColour:
			result = cuT( "colour" );
			break;

		case EdgeDetectionType::eLuma:
			result = cuT( "luma" );
			break;
		}

		return result;
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
	{
		m_passesCount = 5u;

		if ( m_config.data.mode == Mode::eT2X )
		{
			m_passesCount += m_config.maxSubsampleIndices;
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
			std::swap( m_commands, m_commandBuffers[m_frameIndex] );
			m_frameIndex = ( m_config.subsampleIndex + 1 ) % m_config.maxSubsampleIndices;
			std::swap( m_commands, m_commandBuffers[m_frameIndex] );
		}

		if ( m_blendingWeightCalculation )
		{
			m_blendingWeightCalculation->update( m_config.subsampleIndices[m_config.subsampleIndex] );
			m_config.subsampleIndex = m_frameIndex;
			m_renderTarget.setJitter( m_config.jitters[m_config.subsampleIndex] );
		}
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_srgbTextureView = &m_target->getDefaultView();
		m_hdrTextureView = &m_renderTarget.getTechnique()->getResult().getDefaultView();

		switch ( m_config.data.edgeDetection )
		{
		case EdgeDetectionType::eDepth:
			m_edgeDetection = std::make_unique< DepthEdgeDetection >( m_renderTarget
				, m_renderTarget.getTechnique()->getDepth().getDefaultView()
				, m_config );
			break;

		case EdgeDetectionType::eColour:
			m_edgeDetection = std::make_unique< ColourEdgeDetection >( m_renderTarget
				, *m_srgbTextureView
				, doGetPredicationTexture()
				, m_config );
			break;

		case EdgeDetectionType::eLuma:
			m_edgeDetection = std::make_unique< LumaEdgeDetection >( m_renderTarget
				, *m_srgbTextureView
				, doGetPredicationTexture()
				, m_config );
			break;
		}

#if !C3D_DebugEdgeDetection
		m_blendingWeightCalculation = std::make_unique< BlendingWeightCalculation >( m_renderTarget
			, m_edgeDetection->getSurface()->getDefaultView()
			, m_edgeDetection->getDepth()
			, m_config );
		auto * velocityView = doGetVelocityView();

#	if !C3D_DebugBlendingWeightCalculation
		m_neighbourhoodBlending = std::make_unique< NeighbourhoodBlending >( m_renderTarget
			, *m_srgbTextureView
			, m_blendingWeightCalculation->getSurface()->getDefaultView()
			, velocityView
			, m_config );

#		if !C3D_DebugNeighbourhoodBlending
		if ( m_config.data.mode == Mode::eT2X )
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
					, m_config ) );
			}
		}
#		endif
#	endif
#endif

		auto & device = getCurrentDevice( *this );
		m_copyProgram.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		m_copyProgram.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		m_copyProgram[0].module->loadShader( getRenderSystem()->compileShader( { ashes::ShaderStageFlag::eVertex, "SmaaCopy", doGetCopyVertexShader() } ) );
		m_copyProgram[1].module->loadShader( getRenderSystem()->compileShader( { ashes::ShaderStageFlag::eFragment, "SmaaCopy", doGetCopyPixelShader( m_config ) } ) );
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
			m_frameIndex = m_config.maxSubsampleIndices - 1u;
			m_config.subsampleIndex = m_frameIndex;
			std::swap( m_commands, m_commandBuffers[m_frameIndex] );
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
			auto & reproject = *m_reproject[index];
			result.emplace_back( reproject.prepareCommands( timer, passIndex++ ) );
			m_smaaResult = reproject.getSurface().get();
		}

		if ( m_config.data.enableReprojection
			&& m_config.data.mode == Mode::eT2X
			&& C3D_DebugVelocity )
		{
			m_smaaResult = m_renderTarget.getVelocity().getTexture().get();
		}

		auto & device = getCurrentDevice( m_renderTarget );
		ashes::DescriptorSetLayoutBindingArray bindings;
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
		timer.beginPass( copyCmd, passIndex );
		copyCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_smaaResult->getDefaultView().makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
		copyCmd.beginRenderPass( *m_copyRenderPass
			, *m_copyFrameBuffer
			, { ashes::ClearColorValue{} }
			, ashes::SubpassContents::eInline );
		copyQuad->registerFrame( copyCmd );
		copyCmd.endRenderPass();
		timer.endPass( copyCmd, passIndex );
		copyCmd.end();
		result.emplace_back( std::move( copyCommands ) );
		++passIndex;

		return result;
	}

	bool PostEffect::doWriteInto( TextFile & p_file, String const & tabs )
	{
		static SmaaConfig::Data const ref;
		auto result = p_file.writeText( cuT( "\n" ) + tabs + Type + cuT( "\n" ) ) > 0
			&& p_file.writeText( tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = p_file.writeText( tabs + cuT( "\tmode " ) + smaa::getName( m_config.data.mode ) + cuT( "\n" ) ) > 0;
		}

		if ( result )
		{
			result = p_file.writeText( tabs + cuT( "\tpreset " ) + smaa::getName( m_config.data.preset ) + cuT( "\n" ) ) > 0;

			if ( result && m_config.data.preset == Preset::eCustom )
			{
				result = p_file.writeText( tabs + cuT( "\tthreshold" ) + string::toString( m_config.data.threshold, std::locale{ "C" } ) + cuT( "\n" ) ) > 0
					&& p_file.writeText( tabs + cuT( "\tmaxSearchSteps " ) + string::toString( m_config.data.maxSearchSteps, std::locale{ "C" } ) + cuT( "\n" ) ) > 0
					&& p_file.writeText( tabs + cuT( "\tmaxSearchStepsDiag " ) + string::toString( m_config.data.maxSearchStepsDiag, std::locale{ "C" } ) + cuT( "\n" ) ) > 0
					&& p_file.writeText( tabs + cuT( "\tcornerRounding " ) + string::toString( m_config.data.cornerRounding, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			}
		}

		if ( result )
		{
			result = p_file.writeText( tabs + cuT( "\tedgeDetection " ) + smaa::getName( m_config.data.edgeDetection ) + cuT( "\n" ) ) > 0;
		}

		if ( result && m_config.data.disableDiagonalDetection != ref.disableDiagonalDetection )
		{
			result = p_file.writeText( tabs + cuT( "\tdisableDiagonalDetection true\n" ) ) > 0;
		}

		if ( result && m_config.data.disableCornerDetection != ref.disableCornerDetection )
		{
			result = p_file.writeText( tabs + cuT( "\tdisableCornerDetection true\n" ) ) > 0;
		}

		if ( result && m_config.data.enablePredication != ref.enablePredication )
		{
			result = p_file.writeText( tabs + cuT( "\tpredication true\n" ) ) > 0;

			if ( result && m_config.data.predicationScale != ref.predicationScale )
			{
				result = p_file.writeText( tabs + cuT( "\tpredicationScale " ) + string::toString( m_config.data.predicationScale, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			}

			if ( result && m_config.data.predicationStrength != ref.predicationStrength )
			{
				result = p_file.writeText( tabs + cuT( "\tpredicationStrength " ) + string::toString( m_config.data.predicationStrength, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			}

			if ( result && m_config.data.predicationThreshold != ref.predicationThreshold )
			{
				result = p_file.writeText( tabs + cuT( "\tpredicationThreshold " ) + string::toString( m_config.data.predicationThreshold, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			}
		}

		if ( result && m_config.data.enableReprojection != ref.enableReprojection )
		{
			result = p_file.writeText( tabs + cuT( "\treprojection true\n" ) ) > 0;

			if ( result && m_config.data.reprojectionWeightScale != ref.reprojectionWeightScale )
			{
				result = p_file.writeText( tabs + cuT( "\treprojectionWeightScale " ) + string::toString( m_config.data.reprojectionWeightScale, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			}
		}

		if ( result && m_config.data.localContrastAdaptationFactor != ref.localContrastAdaptationFactor )
		{
			result = p_file.writeText( tabs + cuT( "\tlocalContrastAdaptationFactor " ) + string::toString( m_config.data.localContrastAdaptationFactor, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
		}

		if ( result )
		{
			result = p_file.writeText( tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	ashes::Texture const * PostEffect::doGetPredicationTexture()
	{
		ashes::Texture const * predication = nullptr;

		if ( m_config.data.enablePredication )
		{
			predication = &m_renderTarget.getTechnique()->getDepth().getTexture();
		}

		return predication;
	}

	ashes::TextureView const * PostEffect::doGetVelocityView()
	{
		ashes::TextureView const * velocityView = nullptr;

		switch ( m_config.data.mode )
		{
		case Mode::eT2X:
			if ( m_config.data.enableReprojection )
			{
				velocityView = &m_renderTarget.getVelocity().getTexture()->getDefaultView();
			}
			break;
		}

		return velocityView;
	}
}

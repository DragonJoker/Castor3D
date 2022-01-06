#include "SmaaPostEffect/SmaaPostEffect.hpp"

#include "SmaaPostEffect/ColourEdgeDetection.hpp"
#include "SmaaPostEffect/DepthEdgeDetection.hpp"
#include "SmaaPostEffect/LumaEdgeDetection.hpp"
#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Shader/GlslToSpv.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

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
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		castor3d::ShaderPtr doGetCopyPixelShader( SmaaConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			UBO_SMAA( writer, SmaaUboIdx, 0u );
			auto c3d_map = writer.declSampledImage< FImg2DRgba32 >( "c3d_map", SmaaUboIdx + 1, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					if ( config.data.mode == Mode::eT2X
						&& C3D_DebugVelocity )
					{
						IF( writer, c3d_smaaData.enableReprojection != 0 )
						{
							pxl_fragColour = vec4( c3d_map.sample( vtx_texture ).xy(), 0.0_f, 1.0_f );
						}
						ELSE
						{
							pxl_fragColour = c3d_map.sample( vtx_texture );
						}
						FI;
					}
					else
					{
						pxl_fragColour = c3d_map.sample( vtx_texture );
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
			, "SMAA"
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters
			, 5u
			, Kind::eSRGB }
		, m_config{ parameters }
		, m_ubo{ renderSystem.getRenderDevice() }
		, m_stages{ castor3d::makeShaderState( renderSystem.getRenderDevice(), { VK_SHADER_STAGE_VERTEX_BIT, "SmaaCopy", doGetCopyVertexShader() } )
			, castor3d::makeShaderState( renderSystem.getRenderDevice(), { VK_SHADER_STAGE_FRAGMENT_BIT, "SmaaCopy", doGetCopyPixelShader( m_config ) } ) }
	{
		if ( m_config.data.mode == Mode::eT2X )
		{
			m_passesCount += m_config.maxSubsampleIndices;
		}
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

		if ( m_reproject )
		{
			m_reproject->accept( visitor );
		}

		visitor.visit( cuT( "Threshold" )
			, m_config.data.threshold );
		visitor.visit( cuT( "Max. search steps" )
			, m_config.data.maxSearchSteps );
		visitor.visit( cuT( "Max. diag. search steps" )
			, m_config.data.maxSearchStepsDiag );
		visitor.visit( cuT( "Corner rounding" )
			, m_config.data.cornerRounding );
		visitor.visit( cuT( "Disable diagonal detection" )
			, m_config.data.disableDiagonalDetection );
		visitor.visit( cuT( "Disable corner detection" )
			, m_config.data.disableCornerDetection );
		visitor.visit( cuT( "Local contrast adaptation factor" )
			, m_config.data.localContrastAdaptationFactor );
		visitor.visit( cuT( "Enable predication" )
			, m_config.data.enablePredication );
		visitor.visit( cuT( "Predication scale" )
			, m_config.data.predicationScale );
		visitor.visit( cuT( "Predication strength" )
			, m_config.data.predicationStrength );
		visitor.visit( cuT( "Predication threshold" )
			, m_config.data.predicationThreshold );
	}

	crg::ImageViewId const * PostEffect::doInitialise( castor3d::RenderDevice const & device
		, crg::FramePass const & previousPass )
	{
		m_srgbTextureView = m_target;
		m_hdrTextureView = &m_renderTarget.getTechnique().getResultImgView();
		auto previous = &previousPass;
		crg::ImageViewIdArray smaaResult;

		switch ( m_config.data.edgeDetection )
		{
		case EdgeDetectionType::eDepth:
			m_edgeDetection = std::make_unique< DepthEdgeDetection >( m_graph
				, *previous
				, m_renderTarget
				, device
				, m_ubo
				, m_renderTarget.getTechnique().getDepthSampledView()
				, m_config
				, &m_enabled );
			break;

		case EdgeDetectionType::eColour:
			m_edgeDetection = std::make_unique< ColourEdgeDetection >( m_graph
				, *previous
				, m_renderTarget
				, device
				, m_ubo
				, *m_srgbTextureView
				, doGetPredicationTexture()
				, m_config
				, &m_enabled );
			break;

		case EdgeDetectionType::eLuma:
			m_edgeDetection = std::make_unique< LumaEdgeDetection >( m_graph
				, *previous
				, m_renderTarget
				, device
				, m_ubo
				, *m_srgbTextureView
				, doGetPredicationTexture()
				, m_config
				, &m_enabled );
			break;
		}

		previous = &m_edgeDetection->getPass();
		smaaResult = { m_edgeDetection->getColourResult() };

#if !C3D_DebugEdgeDetection
		m_blendingWeightCalculation = std::make_unique< BlendingWeightCalculation >( m_graph
			, *previous
			, m_renderTarget
			, device
			, m_ubo
			, m_edgeDetection->getColourResult()
			, m_edgeDetection->getDepthResult()
			, m_config
			, &m_enabled );
		previous = &m_blendingWeightCalculation->getPass();
		smaaResult = { m_blendingWeightCalculation->getResult() };

#	if !C3D_DebugBlendingWeightCalculation
		auto * velocityView = doGetVelocityView();
		m_neighbourhoodBlending = std::make_unique< NeighbourhoodBlending >( m_graph
			, *previous
			, m_renderTarget
			, device
			, m_ubo
			, *m_srgbTextureView
			, m_blendingWeightCalculation->getResult()
			, velocityView
			, m_config
			, &m_enabled );
		previous = &m_neighbourhoodBlending->getPass();
		smaaResult = m_neighbourhoodBlending->getResult();

#		if !C3D_DebugNeighbourhoodBlending
		if ( m_config.data.mode == Mode::eT2X )
		{
			crg::ImageViewIdArray currentViews = m_neighbourhoodBlending->getResult();
			crg::ImageViewIdArray previousViews;

			for ( size_t i = 0; i < currentViews.size(); ++i )
			{
				previousViews.push_back( i == 0u
					? currentViews[m_config.maxSubsampleIndices - 1u]
					: currentViews[i - 1u] );
			}

			m_reproject = std::make_unique< Reproject >( m_graph
				, *previous
				, m_renderTarget
				, device
				, m_ubo
				, currentViews
				, previousViews
				, velocityView
				, m_config
				, &m_enabled );
			previous = &m_reproject->getPass();
			smaaResult = { m_reproject->getResult() };
		}
#		endif
#	endif
#endif

		auto & pass = m_graph.createPass( "Copy"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( castor3d::getSafeBandedSize( m_renderTarget.getSize() ) ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.passIndex( &m_config.subsampleIndex )
					.enabled( &m_enabled )
					.build( framePass, context, graph, { m_config.maxSubsampleIndices } );
				getOwner()->getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		pass.addDependency( *previous );
		m_ubo.createPassBinding( pass
			, SmaaUboIdx );
		pass.addSampledView( smaaResult
			, SmaaUboIdx + 1
			, {}
			, linearSampler );
		pass.addOutputColourView( *m_target );
		previous = &pass;

		m_pass = previous;
		return m_target;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_reproject.reset();
		m_neighbourhoodBlending.reset();
		m_blendingWeightCalculation.reset();
		m_edgeDetection.reset();
	}

	void PostEffect::doCpuUpdate( castor3d::CpuUpdater & updater )
	{
		if ( m_enabled )
		{
			if ( m_config.maxSubsampleIndices > 1u )
			{
				m_frameIndex = ( m_config.subsampleIndex + 1 ) % m_config.maxSubsampleIndices;
			}

			if ( m_blendingWeightCalculation )
			{
				m_renderTarget.setJitter( m_config.jitters[m_frameIndex] );
			}

			m_ubo.cpuUpdate( castor3d::getSafeBandedSize( m_renderTarget.getSize() )
				, m_config );
			m_config.subsampleIndex = m_frameIndex;
		}
		else
		{
			m_renderTarget.setJitter( { 0.0f, 0.0f } );
		}
	}

	bool PostEffect::doWriteInto( StringStream & p_file, String const & tabs )
	{
		static SmaaConfig::Data const ref;
		p_file << ( cuT( "\n" ) + tabs + Type + cuT( "\n" ) );
		p_file << ( tabs + cuT( "{\n" ) );
		p_file << ( tabs + cuT( "\tmode " ) + smaa::getName( m_config.data.mode ) + cuT( "\n" ) );
		p_file << ( tabs + cuT( "\tpreset " ) + smaa::getName( m_config.data.preset ) + cuT( "\n" ) );

		if ( m_config.data.preset == Preset::eCustom )
		{
			p_file << ( tabs + cuT( "\tthreshold" ) + string::toString( m_config.data.threshold, std::locale{ "C" } ) + cuT( "\n" ) );
			p_file << ( tabs + cuT( "\tmaxSearchSteps " ) + string::toString( m_config.data.maxSearchSteps, std::locale{ "C" } ) + cuT( "\n" ) );
			p_file << ( tabs + cuT( "\tmaxSearchStepsDiag " ) + string::toString( m_config.data.maxSearchStepsDiag, std::locale{ "C" } ) + cuT( "\n" ) );
			p_file << ( tabs + cuT( "\tcornerRounding " ) + string::toString( m_config.data.cornerRounding, std::locale{ "C" } ) + cuT( "\n" ) );
		}

		p_file << ( tabs + cuT( "\tedgeDetection " ) + smaa::getName( m_config.data.edgeDetection ) + cuT( "\n" ) );

		if ( m_config.data.disableDiagonalDetection != ref.disableDiagonalDetection )
		{
			p_file << ( tabs + cuT( "\tdisableDiagonalDetection true\n" ) );
		}

		if ( m_config.data.disableCornerDetection != ref.disableCornerDetection )
		{
			p_file << ( tabs + cuT( "\tdisableCornerDetection true\n" ) );
		}

		if ( m_config.data.enablePredication != ref.enablePredication )
		{
			p_file << ( tabs + cuT( "\tpredication true\n" ) );

			if ( m_config.data.predicationScale != ref.predicationScale )
			{
				p_file << ( tabs + cuT( "\tpredicationScale " ) + string::toString( m_config.data.predicationScale, std::locale{ "C" } ) + cuT( "\n" ) );
			}

			if ( m_config.data.predicationStrength != ref.predicationStrength )
			{
				p_file << ( tabs + cuT( "\tpredicationStrength " ) + string::toString( m_config.data.predicationStrength, std::locale{ "C" } ) + cuT( "\n" ) );
			}

			if ( m_config.data.predicationThreshold != ref.predicationThreshold )
			{
				p_file << ( tabs + cuT( "\tpredicationThreshold " ) + string::toString( m_config.data.predicationThreshold, std::locale{ "C" } ) + cuT( "\n" ) );
			}
		}

		if ( m_config.data.enableReprojection != ref.enableReprojection )
		{
			p_file << ( tabs + cuT( "\treprojection true\n" ) );

			if ( m_config.data.reprojectionWeightScale != ref.reprojectionWeightScale )
			{
				p_file << ( tabs + cuT( "\treprojectionWeightScale " ) + string::toString( m_config.data.reprojectionWeightScale, std::locale{ "C" } ) + cuT( "\n" ) );
			}
		}

		if ( m_config.data.localContrastAdaptationFactor != ref.localContrastAdaptationFactor )
		{
			p_file << ( tabs + cuT( "\tlocalContrastAdaptationFactor " ) + string::toString( m_config.data.localContrastAdaptationFactor, std::locale{ "C" } ) + cuT( "\n" ) );
		}

		p_file << ( tabs + cuT( "}\n" ) );
		return true;
	}

	crg::ImageViewId const * PostEffect::doGetPredicationTexture()
	{
		crg::ImageViewId const * predication = nullptr;

		if ( m_config.data.enablePredication )
		{
			predication = &m_renderTarget.getTechnique().getDepthSampledView();
		}

		return predication;
	}

	crg::ImageViewId const * PostEffect::doGetVelocityView()
	{
		crg::ImageViewId const * velocityView = nullptr;

		switch ( m_config.data.mode )
		{
		case Mode::eT2X:
			if ( m_config.data.enableReprojection )
			{
				velocityView = &m_renderTarget.getVelocity()->sampledViewId;
			}
			break;

		default:
			break;
		}

		return velocityView;
	}
}

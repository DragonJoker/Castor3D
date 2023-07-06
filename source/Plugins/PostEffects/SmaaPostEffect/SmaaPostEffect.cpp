#include "SmaaPostEffect/SmaaPostEffect.hpp"

#include "SmaaPostEffect/ColourEdgeDetection.hpp"
#include "SmaaPostEffect/DepthEdgeDetection.hpp"
#include "SmaaPostEffect/LumaEdgeDetection.hpp"
#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Shader/GlslToSpv.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace smaa
{
	static bool constexpr C3D_DebugEdgeDetection = false;
	static bool constexpr C3D_DebugBlendingWeightCalculation = false;
	static bool constexpr C3D_DebugNeighbourhoodBlending = false;
	static bool constexpr C3D_DebugVelocity = false;

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
			C3D_Smaa( writer, SmaaUboIdx, 0u );
			auto c3d_map = writer.declCombinedImg< FImg2DRgba32 >( "c3d_map", SmaaUboIdx + 1, 0u );

			// Shader outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					if ( config.data.mode == Mode::eT2X
						&& C3D_DebugVelocity )
					{
						IF( writer, c3d_smaaData.enableReprojection != 0 )
						{
							outColour = vec4( c3d_map.sample( vtx_texture ).xy(), 0.0_f, 1.0_f );
						}
						ELSE
						{
							outColour = c3d_map.sample( vtx_texture );
						}
						FI;
					}
					else
					{
						outColour = c3d_map.sample( vtx_texture );
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

	castor::String PostEffect::Type = cuT( "smaa" );
	castor::String PostEffect::Name = cuT( "SMAA PostEffect" );

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
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SmaaCopy", doGetCopyVertexShader() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SmaaCopy", doGetCopyPixelShader( m_config ) }
		, m_stages{ castor3d::makeShaderState( renderSystem.getRenderDevice(), m_vertexShader )
			, castor3d::makeShaderState( renderSystem.getRenderDevice(), m_pixelShader ) }
	{
		if ( m_config.data.mode == Mode::eT2X )
		{
			m_passesCount += m_config.maxSubsampleIndices;
		}
	}

	castor3d::PostEffectUPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
	{
		return castor::makeUniqueDerived< castor3d::PostEffect, PostEffect >( renderTarget
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

		m_presetIndex = int32_t( m_config.data.preset );
		visitor.visit( cuT( "Preset" )
			, m_presetIndex
			, castor::StringArray{ "Low", "Medium", "High", "Ultra", "Custom" }
			, [this]( int32_t oldV, int32_t newV )
			{
				m_config.data.preset = Preset( m_presetIndex );
				m_config.updatePreset();
			} );

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

	void PostEffect::setParameters( castor3d::Parameters parameters )
	{
		m_config = SmaaConfig{ parameters };
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::Texture const & source
		, castor3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
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
				, m_renderTarget.getTechnique().getDepthObj().sampledViewId
				, m_config
				, &m_enabled );
			break;

		case EdgeDetectionType::eColour:
			m_edgeDetection = std::make_unique< ColourEdgeDetection >( m_graph
				, *previous
				, m_renderTarget
				, device
				, m_ubo
				, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
				, doGetPredicationTexture()
				, m_config
				, &m_enabled
				, &m_passIndex );
			break;

		case EdgeDetectionType::eLuma:
			m_edgeDetection = std::make_unique< LumaEdgeDetection >( m_graph
				, *previous
				, m_renderTarget
				, device
				, m_ubo
				, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
				, doGetPredicationTexture()
				, m_config
				, &m_enabled
				, &m_passIndex );
			break;
		}

		previous = &m_edgeDetection->getPass();
		smaaResult = { m_edgeDetection->getColourResult() };

		if constexpr ( !C3D_DebugEdgeDetection )
		{
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

			if constexpr ( !C3D_DebugBlendingWeightCalculation )
			{
				auto * velocityView = doGetVelocityView();
				m_neighbourhoodBlending = std::make_unique< NeighbourhoodBlending >( m_graph
					, *previous
					, m_renderTarget
					, device
					, m_ubo
					, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
					, m_blendingWeightCalculation->getResult()
					, velocityView
					, m_config
					, &m_enabled
					, &m_subsamplePassIndex );
				previous = &m_neighbourhoodBlending->getPass();
				smaaResult = m_neighbourhoodBlending->getResult();

				if constexpr ( !C3D_DebugNeighbourhoodBlending )
				{
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
				}
			}
		}

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
					.passIndex( &m_subsamplePassIndex )
					.enabled( &m_enabled )
					.build( framePass, context, graph, { m_config.maxSubsampleIndices * 2u } );
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
			, linearSampler );
		crg::ImageViewIdArray outputs;
		crg::ImageViewIdArray addOutputs;

		for ( auto index = 0u; index < m_config.maxSubsampleIndices; ++index )
		{
			outputs.push_back( target.targetViewId );
			addOutputs.push_back( source.targetViewId );
		}

		outputs.insert( outputs.end(), addOutputs.begin(), addOutputs.end() );
		pass.addOutputColourView( outputs );
		previous = &pass;

		m_pass = previous;
		return true;
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
			m_subsamplePassIndex = m_config.subsampleIndex + m_passIndex * m_config.maxSubsampleIndices;
		}
		else
		{
			m_renderTarget.setJitter( { 0.0f, 0.0f } );
		}
	}

	bool PostEffect::doWriteInto( castor::StringStream & file, castor::String const & tabs )
	{
		static SmaaConfig::Data const ref;
		file << ( cuT( "\n" ) + tabs + Type + cuT( "\n" ) );
		file << ( tabs + cuT( "{\n" ) );
		file << ( tabs + cuT( "\tmode " ) + smaa::getName( m_config.data.mode ) + cuT( "\n" ) );
		file << ( tabs + cuT( "\tpreset " ) + smaa::getName( m_config.data.preset ) + cuT( "\n" ) );

		if ( m_config.data.preset == Preset::eCustom )
		{
			file << ( tabs + cuT( "\tthreshold" ) + castor::string::toString( m_config.data.threshold, std::locale{ "C" } ) + cuT( "\n" ) );
			file << ( tabs + cuT( "\tmaxSearchSteps " ) + castor::string::toString( m_config.data.maxSearchSteps, std::locale{ "C" } ) + cuT( "\n" ) );
			file << ( tabs + cuT( "\tmaxSearchStepsDiag " ) + castor::string::toString( m_config.data.maxSearchStepsDiag, std::locale{ "C" } ) + cuT( "\n" ) );
			file << ( tabs + cuT( "\tcornerRounding " ) + castor::string::toString( m_config.data.cornerRounding, std::locale{ "C" } ) + cuT( "\n" ) );
		}

		file << ( tabs + cuT( "\tedgeDetection " ) + smaa::getName( m_config.data.edgeDetection ) + cuT( "\n" ) );

		if ( m_config.data.disableDiagonalDetection != ref.disableDiagonalDetection )
		{
			file << ( tabs + cuT( "\tdisableDiagonalDetection true\n" ) );
		}

		if ( m_config.data.disableCornerDetection != ref.disableCornerDetection )
		{
			file << ( tabs + cuT( "\tdisableCornerDetection true\n" ) );
		}

		if ( m_config.data.enablePredication != ref.enablePredication )
		{
			file << ( tabs + cuT( "\tpredication true\n" ) );

			if ( m_config.data.predicationScale != ref.predicationScale )
			{
				file << ( tabs + cuT( "\tpredicationScale " ) + castor::string::toString( m_config.data.predicationScale, std::locale{ "C" } ) + cuT( "\n" ) );
			}

			if ( m_config.data.predicationStrength != ref.predicationStrength )
			{
				file << ( tabs + cuT( "\tpredicationStrength " ) + castor::string::toString( m_config.data.predicationStrength, std::locale{ "C" } ) + cuT( "\n" ) );
			}

			if ( m_config.data.predicationThreshold != ref.predicationThreshold )
			{
				file << ( tabs + cuT( "\tpredicationThreshold " ) + castor::string::toString( m_config.data.predicationThreshold, std::locale{ "C" } ) + cuT( "\n" ) );
			}
		}

		if ( m_config.data.enableReprojection != ref.enableReprojection )
		{
			file << ( tabs + cuT( "\treprojection true\n" ) );

			if ( m_config.data.reprojectionWeightScale != ref.reprojectionWeightScale )
			{
				file << ( tabs + cuT( "\treprojectionWeightScale " ) + castor::string::toString( m_config.data.reprojectionWeightScale, std::locale{ "C" } ) + cuT( "\n" ) );
			}
		}

		if ( m_config.data.localContrastAdaptationFactor != ref.localContrastAdaptationFactor )
		{
			file << ( tabs + cuT( "\tlocalContrastAdaptationFactor " ) + castor::string::toString( m_config.data.localContrastAdaptationFactor, std::locale{ "C" } ) + cuT( "\n" ) );
		}

		file << ( tabs + cuT( "}\n" ) );
		return true;
	}

	crg::ImageViewId const * PostEffect::doGetPredicationTexture()
	{
		crg::ImageViewId const * predication = nullptr;

		if ( m_config.data.enablePredication )
		{
			predication = &m_renderTarget.getTechnique().getDepthObj().sampledViewId;
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

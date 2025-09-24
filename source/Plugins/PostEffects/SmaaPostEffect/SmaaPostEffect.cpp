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
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace smaa
{
	static bool constexpr C3D_DebugEdgeDetection = false;
	static bool constexpr C3D_DebugBlendingWeightCalculation = false;
	static bool constexpr C3D_DebugNeighbourhoodBlending = false;
	static bool constexpr C3D_DebugVelocity = false;

	//*********************************************************************************************

	namespace copy
	{
		namespace c3ds = c3d::shader;

		enum class Bindings : uint32_t
		{
			Texture = uint32_t( smaa::Bindings::SmaaUboIdx ) + 1u,
		};

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device
			, SmaaConfig const & config )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_Smaa( writer, smaa::Bindings::SmaaUboIdx, 0u );
			auto c3d_map = writer.declCombinedImg< FImg2DRgba32 >( "c3d_map", Bindings::Texture, 0u );

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::PosUv2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &config, &c3d_map, &c3d_smaaData]( sdw::FragmentInT< c3ds::Uv2FT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
				{
					if ( config.data.mode == Mode::eT2X
						&& C3D_DebugVelocity )
					{
						sdwIF( writer, c3d_smaaData.enableReprojection != 0 )
						{
							out.colour() = vec4( c3d_map.sample( in.uv() ).xy(), 0.0_f, 1.0_f );
						}
						sdwELSE
						{
							out.colour() = c3d_map.sample( in.uv() );
						}
						sdwFI
					}
					else
					{
						out.colour() = c3d_map.sample( in.uv() );
					}
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	c3d::StringView getName( Mode mode )
	{
		switch ( mode )
		{
		case Mode::e1X:
			return cuT( "1X" );
		case Mode::eT2X:
			return cuT( "T2X" );
		case Mode::eS2X:
			return cuT( "S2X" );
		case Mode::e4X:
			return cuT( "4X" );
		default:
			return cuT( "Unsupported" );
		}
	}

	c3d::StringView getName( Preset preset )
	{
		switch ( preset )
		{
		case Preset::eLow:
			return cuT( "low" );
		case Preset::eMedium:
			return cuT( "medium" );
		case Preset::eHigh:
			return cuT( "high" );
		case Preset::eUltra:
			return cuT( "ultra" );
		case Preset::eCustom:
			return cuT( "custom" );
		default:
			return cuT( "Unsupported" );
		}
	}

	c3d::StringView getName( EdgeDetectionType detection )
	{
		switch ( detection )
		{
		case EdgeDetectionType::eDepth:
			return cuT( "depth" );
		case EdgeDetectionType::eColour:
			return cuT( "colour" );
		case EdgeDetectionType::eLuma:
			return cuT( "luma" );
		default:
			return cuT( "Unsupported" );
		}
	}

	//*********************************************************************************************

	c3d::String PostEffect::Type = cuT( "smaa" );
	c3d::MbString PostEffect::Name = "SMAA PostEffect";

	PostEffect::PostEffect( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & parameters )
		: c3d::PostEffect{ PostEffect::Type
			, cuT( "SMAA" )
			, c3d::makeString( PostEffect::Name )
			, renderTarget
			, renderSystem
			, parameters
			, 5u
			, Kind::eSRGB }
		, m_config{ parameters }
		, m_ubo{ renderSystem.getRenderDevice() }
		, m_shader{ cuT( "SmaaCopy" ), copy::getProgram( renderSystem.getRenderDevice(), m_config ) }
		, m_stages{ makeProgramStates( renderSystem.getRenderDevice(), m_shader ) }
	{
		if ( m_config.data.mode == Mode::eT2X )
			m_passesCount += m_config.maxSubsampleIndices;
	}

	c3d::PostEffectUPtr PostEffect::create( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & parameters )
	{
		return c3d::makeUniqueDerived< c3d::PostEffect, PostEffect >( renderTarget
			, renderSystem
			, parameters );
	}

	void PostEffect::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		if ( m_edgeDetection )
			m_edgeDetection->accept( visitor );
		if ( m_blendingWeightCalculation )
			m_blendingWeightCalculation->accept( visitor );
		if ( m_neighbourhoodBlending )
			m_neighbourhoodBlending->accept( visitor );
		if ( m_reproject )
			m_reproject->accept( visitor );

		visitor.visit( cuT( "Preset" )
			, m_config.data.preset
			, c3d::StringArray{ cuT( "Low" ), cuT( "Medium" ), cuT( "High" ), cuT( "Ultra" ), cuT( "Custom" ) }
			, c3d::ConfigurationVisitorBase::OnEnumValueChangeT< Preset >( [this]( Preset, Preset )
			{
				m_config.updatePreset();
			} ) );

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

	void PostEffect::setParameters( c3d::Parameters parameters )
	{
		m_config = SmaaConfig{ parameters };
	}

	bool PostEffect::doInitialise( c3d::RenderDevice const & device
		, c3d::Texture const & source
		, c3d::Texture & target )
	{
		switch ( m_config.data.edgeDetection )
		{
		case EdgeDetectionType::eDepth:
			m_edgeDetection = c3d::makeRawUnique< DepthEdgeDetection >( m_graph
				, m_renderTarget
				, device
				, m_ubo
				, m_renderTarget.getTechnique().getDepthObj()
				, m_config
				, &m_enabled );
			break;

		case EdgeDetectionType::eColour:
			m_edgeDetection = c3d::makeRawUnique< ColourEdgeDetection >( m_graph
				, m_renderTarget
				, device
				, m_ubo
				, source
				, doGetPredicationTexture()
				, m_config
				, &m_enabled
				, &m_passIndex );
			break;

		case EdgeDetectionType::eLuma:
			m_edgeDetection = c3d::makeRawUnique< LumaEdgeDetection >( m_graph
				, m_renderTarget
				, device
				, m_ubo
				, source
				, doGetPredicationTexture()
				, m_config
				, &m_enabled
				, &m_passIndex );
			break;
		}

		auto smaaResult = m_edgeDetection->getColourResult().getLastAttach();

		if constexpr ( !C3D_DebugEdgeDetection )
		{
			m_blendingWeightCalculation = c3d::makeRawUnique< BlendingWeightCalculation >( m_graph
				, m_renderTarget
				, device
				, m_ubo
				, m_edgeDetection->getColourResult()
				, m_edgeDetection->getDepthResult()
				, &m_enabled );
			smaaResult = m_blendingWeightCalculation->getResult().getLastAttach();

			if constexpr ( !C3D_DebugBlendingWeightCalculation )
			{
				auto * velocityView = doGetVelocityView();
				m_neighbourhoodBlending = c3d::makeRawUnique< NeighbourhoodBlending >( m_graph
					, m_renderTarget
					, device
					, m_ubo
					, source
					, m_blendingWeightCalculation->getResult()
					, velocityView
					, m_config
					, &m_enabled
					, &m_subsamplePassIndex );
				smaaResult = m_neighbourhoodBlending->getResult();

				if constexpr ( !C3D_DebugNeighbourhoodBlending )
				{
					if ( m_config.data.mode == Mode::eT2X )
					{
						crg::ImageViewIdArray currentViews = m_neighbourhoodBlending->getViews();
						crg::ImageViewIdArray previousViews;

						for ( size_t i = 0; i < currentViews.size(); ++i )
						{
							previousViews.push_back( i == 0u
								? currentViews[m_config.maxSubsampleIndices - 1u]
								: currentViews[i - 1u] );
						}

						m_reproject = c3d::makeRawUnique< Reproject >( m_graph
							, m_renderTarget
							, device
							, m_ubo
							, *smaaResult
							, currentViews
							, previousViews
							, velocityView
							, m_config
							, &m_enabled );
						smaaResult = m_reproject->getResult().getLastAttach();
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
					.renderSize( c3d::makeExtent2D( c3d::getSafeBandedSize( m_renderTarget.getDisplaySize() ) ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.passIndex( &m_subsamplePassIndex )
					.enabled( &m_enabled )
					.build( framePass, context, graph, { m_config.maxSubsampleIndices * 2u } );
				getOwner()->getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		crg::SamplerDesc linearSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest };
		m_ubo.createPassBinding( pass, smaa::Bindings::SmaaUboIdx );
		pass.addInputSampledT( *smaaResult, copy::Bindings::Texture, linearSampler );
		crg::ImageViewIdArray outputs;
		crg::ImageViewIdArray addOutputs;

		for ( auto index = 0u; index < m_config.maxSubsampleIndices; ++index )
		{
			outputs.push_back( target.getTargetViewId() );
			addOutputs.push_back( source.getTargetViewId() );
		}

		outputs.insert( outputs.end(), addOutputs.begin(), addOutputs.end() );
		target.setLastAttach( pass.addOutputColourTarget( outputs ) );

		return true;
	}

	void PostEffect::doCleanup( c3d::RenderDevice const & device )
	{
		m_reproject.reset();
		m_neighbourhoodBlending.reset();
		m_blendingWeightCalculation.reset();
		m_edgeDetection.reset();
	}

	void PostEffect::doCpuUpdate( c3d::CpuUpdater & updater )
	{
		if ( m_enabled )
		{
			if ( m_config.maxSubsampleIndices > 1u )
				m_frameIndex = ( m_config.subsampleIndex + 1 ) % m_config.maxSubsampleIndices;

			if ( m_blendingWeightCalculation )
			{
				auto jitter = m_config.jitters[m_frameIndex] * 2.0f;
				jitter[0] /= float( m_renderTarget.getRenderSize().getWidth() );
				jitter[1] /= float( m_renderTarget.getRenderSize().getHeight() );
				m_renderTarget.setJitter( jitter );
			}

			m_ubo.cpuUpdate( c3d::getSafeBandedSize( m_renderTarget.getDisplaySize() )
				, m_config );
			m_config.subsampleIndex = m_frameIndex;
			m_subsamplePassIndex = m_config.subsampleIndex + m_passIndex * m_config.maxSubsampleIndices;
		}
		else
		{
			m_renderTarget.setJitter( { 0.0f, 0.0f } );
		}
	}

	bool PostEffect::doWriteInto( c3d::StringStream & file, c3d::String const & tabs )
	{
		static SmaaConfig::Data const ref;
		file << ( cuT( "\n" ) + tabs + Type + cuT( "\n" ) );
		file << ( tabs + cuT( "{\n" ) );
		file << ( tabs + cuT( "\tmode " ) + c3d::String{ smaa::getName( m_config.data.mode ) } + cuT( "\n" ) );
		file << ( tabs + cuT( "\tpreset " ) + c3d::String{ smaa::getName( m_config.data.preset ) } + cuT( "\n" ) );

		if ( m_config.data.preset == Preset::eCustom )
		{
			file << ( tabs + cuT( "\tthreshold" ) + c3d::string::toString( m_config.data.threshold, std::locale{ "C" } ) + cuT( "\n" ) );
			file << ( tabs + cuT( "\tmaxSearchSteps " ) + c3d::string::toString( m_config.data.maxSearchSteps, std::locale{ "C" } ) + cuT( "\n" ) );
			file << ( tabs + cuT( "\tmaxSearchStepsDiag " ) + c3d::string::toString( m_config.data.maxSearchStepsDiag, std::locale{ "C" } ) + cuT( "\n" ) );
			file << ( tabs + cuT( "\tcornerRounding " ) + c3d::string::toString( m_config.data.cornerRounding, std::locale{ "C" } ) + cuT( "\n" ) );
		}

		file << ( tabs + cuT( "\tedgeDetection " ) + c3d::String{ smaa::getName( m_config.data.edgeDetection ) } + cuT( "\n" ) );
		if ( m_config.data.disableDiagonalDetection != ref.disableDiagonalDetection )
			file << ( tabs + cuT( "\tdisableDiagonalDetection true\n" ) );
		if ( m_config.data.disableCornerDetection != ref.disableCornerDetection )
			file << ( tabs + cuT( "\tdisableCornerDetection true\n" ) );

		if ( m_config.data.enablePredication != ref.enablePredication )
		{
			file << ( tabs + cuT( "\tenablePredication true\n" ) );
			if ( m_config.data.predicationScale != ref.predicationScale )
				file << ( tabs + cuT( "\tpredicationScale " ) + c3d::string::toString( m_config.data.predicationScale, std::locale{ "C" } ) + cuT( "\n" ) );
			if ( m_config.data.predicationStrength != ref.predicationStrength )
				file << ( tabs + cuT( "\tpredicationStrength " ) + c3d::string::toString( m_config.data.predicationStrength, std::locale{ "C" } ) + cuT( "\n" ) );
			if ( m_config.data.predicationThreshold != ref.predicationThreshold )
				file << ( tabs + cuT( "\tpredicationThreshold " ) + c3d::string::toString( m_config.data.predicationThreshold, std::locale{ "C" } ) + cuT( "\n" ) );
		}

		if ( m_config.data.enableReprojection != ref.enableReprojection )
		{
			file << ( tabs + cuT( "\treprojection true\n" ) );
			if ( m_config.data.reprojectionWeightScale != ref.reprojectionWeightScale )
				file << ( tabs + cuT( "\treprojectionWeightScale " ) + c3d::string::toString( m_config.data.reprojectionWeightScale, std::locale{ "C" } ) + cuT( "\n" ) );
		}

		if ( m_config.data.localContrastAdaptationFactor != ref.localContrastAdaptationFactor )
			file << ( tabs + cuT( "\tlocalContrastAdaptationFactor " ) + c3d::string::toString( m_config.data.localContrastAdaptationFactor, std::locale{ "C" } ) + cuT( "\n" ) );

		file << ( tabs + cuT( "}\n" ) );
		return true;
	}

	c3d::Texture const * PostEffect::doGetPredicationTexture()const
	{
		c3d::Texture const * predication = nullptr;
		if ( m_config.data.enablePredication )
			predication = &m_renderTarget.getTechnique().getDepthObj();
		return predication;
	}

	c3d::Texture const * PostEffect::doGetVelocityView()const
	{
		c3d::Texture const * velocityView = nullptr;
		if ( m_config.data.mode == Mode::eT2X && m_config.data.enableReprojection )
			velocityView = &m_renderTarget.getVelocity();
		return velocityView;
	}
}

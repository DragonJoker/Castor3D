#include "Castor3D/Render/Clustered/ClustersConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, ClustersConfig )

namespace castor
{
	template<>
	struct ParserEnumTraits< castor3d::ClusterSplitScheme >
	{
		static inline xchar const * const Name = cuT( "ClusterSplitScheme" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = castor3d::getEnumMapT< castor3d::ClusterSplitScheme >();
			return result;
		}();
	};
}

namespace castor3d
{
	namespace clscfg
	{
		static CU_ImplementAttributeParser( parserRenderTargetClusters )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.renderTarget )
			{
				CU_ParsingError( cuT( "Render target not initialised." ) );
			}
			else
			{
				parsingContext.clustersConfig = castor::makeUnique< ClustersConfig >();
			}
		}
		CU_EndAttributePush( CSCNSection::eClusters )

		static CU_ImplementAttributeParser( parserClustersEnabled )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->enabled );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersUseBVH )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->useLightsBVH );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersSortLights )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->sortLights );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersLimitClusters )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->limitClustersToLightsAABB );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersParseDepth )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->parseDepthBuffer );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersSpotCone )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->useSpotBoundingCone );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersSpotTightAABB )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->useSpotTightBoundingBox );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersReduceWarpOptimisation )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->enableReduceWarpOptimisation );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersBVHWarpOptimisation )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->enableBVHWarpOptimisation );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersSplitScheme )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				parsingContext.clustersConfig->splitScheme = ClusterSplitScheme( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersBias )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( parsingContext.clustersConfig->bias );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserClustersEnd )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				parsingContext.renderTarget->setClustersConfig( *parsingContext.clustersConfig );
				parsingContext.clustersConfig.reset();
			}
		}
		CU_EndAttributePop()
	}

	ClustersConfig::ClustersConfig()
		: enabled{ true }
		, useLightsBVH{ true }
		, sortLights{ true }
		, parseDepthBuffer{ false }
		, limitClustersToLightsAABB{ true }
		, useSpotBoundingCone{ true }
		, useSpotTightBoundingBox{ true }
		, enableReduceWarpOptimisation{ false }
		, enableBVHWarpOptimisation{ true }
		, splitScheme{ ClusterSplitScheme::eExponentialBase }
		, bias{ 1.0f }
	{
	}

	void ClustersConfig::accept( ConfigurationVisitorBase & visitor )
	{
		if ( enabled )
		{
			static castor::StringArray names{ cuT( "Exponential" )
				, cuT( "Biased Exponential" )
				, cuT( "Linear" )
				, cuT( "Hybrid" ) };

			visitor.visit( cuT( "Clusters" ) );
			visitor.visit( cuT( "Use BVH" ), useLightsBVH );
			visitor.visit( cuT( "Sort Lights" ), sortLights );
			visitor.visit( cuT( "Use Depth Buffer" ), parseDepthBuffer );
			visitor.visit( cuT( "Limit Clusters To Lights AABB" ), limitClustersToLightsAABB );
			visitor.visit( cuT( "Use Spot Bounding Cone" ), useSpotBoundingCone );
			visitor.visit( cuT( "Use Spot Tight Bounding Box" ), useSpotTightBoundingBox );
			visitor.visit( cuT( "Enable Reduce Warp Optimisation" ), enableReduceWarpOptimisation );
			visitor.visit( cuT( "Enable BVH Warp Optimisation" ), enableBVHWarpOptimisation );
			visitor.visit( cuT( "Split Scheme" )
				, splitScheme
				, names
				, ConfigurationVisitorBase::OnEnumValueChangeT< ClusterSplitScheme >( [this]( ClusterSplitScheme oldV, ClusterSplitScheme newV )
					{
						splitScheme = newV;
					} ) );
			visitor.visit( cuT( "Biased Exponential Bias" ), bias );
		}
	}

	void ClustersConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		addParser( result, uint32_t( CSCNSection::eRenderTarget ), cuT( "clusters" ), clscfg::parserRenderTargetClusters );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "enabled" ), clscfg::parserClustersEnabled, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "use_lights_bvh" ), clscfg::parserClustersUseBVH, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "sort_lights" ), clscfg::parserClustersSortLights, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "limit_clusters_to_lights_aabb" ), clscfg::parserClustersLimitClusters, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "parse_depth_buffer" ), clscfg::parserClustersParseDepth, { makeDefaultedParameter< ParameterType::eBool >( false ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "use_spot_bounding_cone" ), clscfg::parserClustersSpotCone, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "use_spot_tight_aabb" ), clscfg::parserClustersSpotTightAABB, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "enable_reduce_warp_optimisation" ), clscfg::parserClustersReduceWarpOptimisation, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "enable_bvh_warp_optimisation" ), clscfg::parserClustersBVHWarpOptimisation, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "split_scheme" ), clscfg::parserClustersSplitScheme, { makeParameter< ParameterType::eCheckedText, ClusterSplitScheme >() } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "bias" ), clscfg::parserClustersBias, { makeDefaultedParameter< ParameterType::eFloat >( 1.0f ) } );
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "}" ), clscfg::parserClustersEnd );
	}

	bool operator==( ClustersConfig const & lhs, ClustersConfig const & rhs )
	{
		return lhs.enabled == rhs.enabled
			&& lhs.useLightsBVH == rhs.useLightsBVH
			&& lhs.sortLights == rhs.sortLights
			&& lhs.parseDepthBuffer == rhs.parseDepthBuffer
			&& lhs.limitClustersToLightsAABB == rhs.limitClustersToLightsAABB
			&& lhs.useSpotBoundingCone == rhs.useSpotBoundingCone
			&& lhs.useSpotTightBoundingBox == rhs.useSpotTightBoundingBox
			&& lhs.enableReduceWarpOptimisation == rhs.enableReduceWarpOptimisation
			&& lhs.enableBVHWarpOptimisation == rhs.enableBVHWarpOptimisation
			&& lhs.splitScheme == rhs.splitScheme
			&& lhs.bias == rhs.bias;
	}
}

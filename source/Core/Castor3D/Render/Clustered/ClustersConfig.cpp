#include "Castor3D/Render/Clustered/ClustersConfig.hpp"

#include "Castor3D/Config.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

CU_ImplementSmartPtr( castor3d, ClustersConfig )

namespace castor3d
{
	namespace clscfg
	{
		CU_ImplementAttributeParser( parserRenderTargetClusters )
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

		CU_ImplementAttributeParser( parserClustersEnabled )
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

		CU_ImplementAttributeParser( parserClustersUseBVH )
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

		CU_ImplementAttributeParser( parserClustersSortLights )
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

		CU_ImplementAttributeParser( parserClustersLimitClusters )
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

		CU_ImplementAttributeParser( parserClustersParseDepth )
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

		CU_ImplementAttributeParser( parserClustersSpotCone )
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

		CU_ImplementAttributeParser( parserClustersSpotTightAABB )
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

		CU_ImplementAttributeParser( parserClustersReduceWarpOptimisation )
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

		CU_ImplementAttributeParser( parserClustersBVHWarpOptimisation )
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

		CU_ImplementAttributeParser( parserClustersEnd )
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
	{
	}

	void ClustersConfig::accept( ConfigurationVisitorBase & visitor )
	{
		if ( enabled )
		{
			visitor.visit( cuT( "Clusters" ) );
			visitor.visit( cuT( "Use BVH" ), useLightsBVH );
			visitor.visit( cuT( "Sort Lights" ), sortLights );
			visitor.visit( cuT( "Use Depth Buffer" ), parseDepthBuffer );
			visitor.visit( cuT( "Limit Clusters To Lights AABB" ), limitClustersToLightsAABB );
			visitor.visit( cuT( "Use Spot Bounding Cone" ), useSpotBoundingCone );
			visitor.visit( cuT( "Use Spot Tight Bounding Box" ), useSpotTightBoundingBox );
			visitor.visit( cuT( "Enable Reduce Warp Optimisation" ), enableReduceWarpOptimisation );
			visitor.visit( cuT( "Enable BVH Warp Optimisation" ), enableBVHWarpOptimisation );
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
		addParser( result, uint32_t( CSCNSection::eClusters ), cuT( "}" ), clscfg::parserClustersEnd );
	}
}

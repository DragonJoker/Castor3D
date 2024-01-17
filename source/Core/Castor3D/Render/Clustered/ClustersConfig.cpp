#include "Castor3D/Render/Clustered/ClustersConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

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
		struct ClustersContext
		{
			ClustersConfigUPtr clustersConfig{};
			RenderTargetRPtr renderTarget{};
		};

		static CU_ImplementAttributeParserNewBlock( parserRenderTargetClusters, TargetContext, ClustersContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "Render target not initialised." ) );
			}
			else
			{
				newBlockContext->renderTarget = blockContext->renderTarget;
				newBlockContext->clustersConfig = castor::makeUnique< ClustersConfig >();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eClusters )

		static CU_ImplementAttributeParserBlock( parserClustersEnabled, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->enabled );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersUseBVH, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->useLightsBVH );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersSortLights, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->sortLights );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersLimitClusters, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->limitClustersToLightsAABB );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersParseDepth, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->parseDepthBuffer );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersSpotCone, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->useSpotBoundingCone );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersSpotTightAABB, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->useSpotTightBoundingBox );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersReduceWarpOptimisation, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->enableReduceWarpOptimisation );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersBVHWarpOptimisation, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->enableBVHWarpOptimisation );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersSplitScheme, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				blockContext->clustersConfig->splitScheme = ClusterSplitScheme( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersBias, ClustersContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				params[0]->get( blockContext->clustersConfig->bias );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClustersEnd, ClustersContext )
		{
			if ( !blockContext->clustersConfig )
			{
				CU_ParsingError( cuT( "Clusters configuration not initialised." ) );
			}
			else
			{
				blockContext->renderTarget->setClustersConfig( *blockContext->clustersConfig );
				blockContext->clustersConfig.reset();
			}
		}
		CU_EndAttributePop()
	}

	ClustersConfig::ClustersConfig()
		: enabled{ true }
		, dirty{ true }
		, useLightsBVH{ dirty, true }
		, sortLights{ dirty, true }
		, parseDepthBuffer{ dirty, false }
		, limitClustersToLightsAABB{ dirty, true }
		, useSpotBoundingCone{ dirty, true }
		, useSpotTightBoundingBox{ dirty, true }
		, enableReduceWarpOptimisation{ dirty, false }
		, enableBVHWarpOptimisation{ dirty, true }
		, enablePostAssignSort{ dirty, false }
		, splitScheme{ dirty, ClusterSplitScheme::eExponentialLinearHybrid }
		, bias{ dirty, 1.0f }
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
			visitor.visit( cuT( "Enable Post Assignment Sort" ), enablePostAssignSort );
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
		BlockParserContextT< TargetContext > targetContext{ result, CSCNSection::eRenderTarget };
		BlockParserContextT< clscfg::ClustersContext > clustersContext{ result, CSCNSection::eClusters, CSCNSection::eRenderTarget };

		targetContext.addPushParser( cuT( "clusters" ), CSCNSection::eClusters, clscfg::parserRenderTargetClusters );
		clustersContext.addParser( cuT( "enabled" ), clscfg::parserClustersEnabled, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "use_lights_bvh" ), clscfg::parserClustersUseBVH, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "sort_lights" ), clscfg::parserClustersSortLights, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "limit_clusters_to_lights_aabb" ), clscfg::parserClustersLimitClusters, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "parse_depth_buffer" ), clscfg::parserClustersParseDepth, { makeDefaultedParameter< ParameterType::eBool >( false ) } );
		clustersContext.addParser( cuT( "use_spot_bounding_cone" ), clscfg::parserClustersSpotCone, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "use_spot_tight_aabb" ), clscfg::parserClustersSpotTightAABB, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "enable_reduce_warp_optimisation" ), clscfg::parserClustersReduceWarpOptimisation, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "enable_bvh_warp_optimisation" ), clscfg::parserClustersBVHWarpOptimisation, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "split_scheme" ), clscfg::parserClustersSplitScheme, { makeParameter< ParameterType::eCheckedText, ClusterSplitScheme >() } );
		clustersContext.addParser( cuT( "bias" ), clscfg::parserClustersBias, { makeDefaultedParameter< ParameterType::eFloat >( 1.0f ) } );
		clustersContext.addPopParser( cuT( "}" ), clscfg::parserClustersEnd );
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

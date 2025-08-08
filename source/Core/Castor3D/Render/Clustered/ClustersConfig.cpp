#include "Castor3D/Render/Clustered/ClustersConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, ClustersConfig )

namespace c3d
{
	template<>
	struct ParserEnumTraits< ClusterSplitScheme >
	{
		static inline xchar const * const Name = cuT( "ClusterSplitScheme" );
		static inline UInt32StrMap const Values = []()
		{
			UInt32StrMap result;
			result = getEnumMapT< ClusterSplitScheme >();
			return result;
		}();
	};
}

namespace c3d
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
				newBlockContext->clustersConfig = makeUnique< ClustersConfig >();
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

		static CU_ImplementAttributeParserBlock( parserClustersMinDistance, ClustersContext )
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
				params[0]->get( blockContext->clustersConfig->minDistance );
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
		: enableReduceWarpOptimisation{ dirty, false }
		, enableWaveIntrinsics{ dirty, false }
		, lockClustersFrustum{ dirty, false }
		, debugDisplay{ dirty, ClusterDebugDisplay::eNone }
		, splitScheme{ dirty, ClusterSplitScheme::eExponentialLinearHybrid }
		, minDistance{ dirty, 1.0f }
	{
	}

	void ClustersConfig::accept( ConfigurationVisitorBase & visitor )
	{
		if ( enabled )
		{
			static StringArray splitSchemeNames{ cuT( "Exponential" )
				, cuT( "Linear" )
				, cuT( "Hybrid" ) };
			static StringArray debugDisplayNames{ cuT( "None" )
				, cuT( "Clusters AABB" )
				, cuT( "Lights AABB" )
				, cuT( "Lights BVH" ) };

			visitor.visit( cuT( "Clusters" ) );
			visitor.visit( cuT( "Enable Reduce Warp Optimisation" ), enableReduceWarpOptimisation );
			visitor.visit( cuT( "Enable Wave Intrinsics" ), enableWaveIntrinsics );
			visitor.visit( cuT( "Split Scheme" )
				, splitScheme
				, splitSchemeNames
				, ConfigurationVisitorBase::OnEnumValueChangeT< ClusterSplitScheme >( [this]( ClusterSplitScheme, ClusterSplitScheme newV ){ splitScheme = newV; } ) );
			visitor.visit( cuT( "Minimal Threshold Distance" ), minDistance );
			visitor.visit( cuT( "[Debug] Lock Clusters Frustum" ), lockClustersFrustum );
			visitor.visit( cuT( "[Debug] Display AABBs" )
				, debugDisplay
				, debugDisplayNames
				, ConfigurationVisitorBase::OnEnumValueChangeT< ClusterDebugDisplay >( [this]( ClusterDebugDisplay, ClusterDebugDisplay newV )
					{
						debugDisplay = newV;
					} ) );
		}
	}

	void ClustersConfig::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< TargetContext > targetContext{ result, CSCNSection::eRenderTarget };
		BlockParserContextT< clscfg::ClustersContext > clustersContext{ result, CSCNSection::eClusters, CSCNSection::eRenderTarget };

		targetContext.addPushParser( cuT( "clusters" ), CSCNSection::eClusters, clscfg::parserRenderTargetClusters );
		clustersContext.addParser( cuT( "enabled" ), clscfg::parserClustersEnabled, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		clustersContext.addParser( cuT( "split_scheme" ), clscfg::parserClustersSplitScheme, { makeParameter< ParameterType::eCheckedText, ClusterSplitScheme >() } );
		clustersContext.addParser( cuT( "min_distance" ), clscfg::parserClustersMinDistance, { makeDefaultedParameter< ParameterType::eFloat >( 1.0f ) } );
		clustersContext.addPopParser( cuT( "}" ), clscfg::parserClustersEnd );
	}

	bool operator==( ClustersConfig const & lhs, ClustersConfig const & rhs )
	{
		return lhs.enabled == rhs.enabled
			&& lhs.enableReduceWarpOptimisation == rhs.enableReduceWarpOptimisation
			&& lhs.splitScheme == rhs.splitScheme
			&& lhs.minDistance == rhs.minDistance;
	}
}

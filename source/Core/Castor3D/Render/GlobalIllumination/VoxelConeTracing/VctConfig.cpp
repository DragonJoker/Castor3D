#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	namespace vctcfg
	{
		static CU_ImplementAttributeParser( parserEnabled )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enabled );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserGridSize )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				uint32_t value;
				params[0]->get( value );
				vctConfig.gridSize = value;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserNumCones )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				uint32_t value;
				params[0]->get( value );
				vctConfig.numCones = value;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserMaxDistance )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.maxDistance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserRayStepSize )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.rayStepSize );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserVoxelSize )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.voxelSizeFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserConservativeRasterization )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enableConservativeRasterization );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTemporalSmoothing )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enableTemporalSmoothing );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserOcclusion )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enableOcclusion );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSecondaryBounce )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enableSecondaryBounce );
			}
		}
		CU_EndAttribute()
	}

	void VctConfig::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "Voxel Cone Tracing" ) );
		visitor.visit( "Enable VCT", enabled );
		visitor.visit( "Conservative Rasterization", enableConservativeRasterization );
		visitor.visit( "Occlusion", enableOcclusion );
		visitor.visit( "Temporal Smoothing", enableTemporalSmoothing );
		visitor.visit( "Secondary Bounce", enableSecondaryBounce );
		visitor.visit( "Num. Cones", numCones );
		visitor.visit( "Max. Distance", maxDistance );
		visitor.visit( "Ray Step Size", rayStepSize );
		visitor.visit( "Voxel Size", voxelSizeFactor );
	}

	void VctConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "enabled" ), vctcfg::parserEnabled, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "grid_size" ), vctcfg::parserGridSize, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 2u, VctMaxTextureSize ) ) } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "num_cones" ), vctcfg::parserNumCones, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 1u, VctMaxDiffuseCones ) ) } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "max_distance" ), vctcfg::parserMaxDistance, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "ray_step_size" ), vctcfg::parserRayStepSize, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "voxel_size" ), vctcfg::parserVoxelSize, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "conservative_rasterization" ), vctcfg::parserConservativeRasterization, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "temporal_smoothing" ), vctcfg::parserTemporalSmoothing, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "occlusion" ), vctcfg::parserOcclusion, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "secondary_bounce" ), vctcfg::parserSecondaryBounce, { makeParameter< ParameterType::eBool >() } );
	}
}

#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace castor3d
{
	namespace vctcfg
	{
		static CU_ImplementAttributeParserBlock( parserVoxelConeTracing, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eVoxelConeTracing, blockContext )

		static CU_ImplementAttributeParserBlock( parserEnabled, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enabled );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserGridSize, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				uint32_t value;
				params[0]->get( value );
				vctConfig.gridSize = value;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNumCones, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				uint32_t value;
				params[0]->get( value );
				vctConfig.numCones = value;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaxDistance, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.maxDistance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRayStepSize, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.rayStepSize );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVoxelSize, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.voxelSizeFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserConservativeRasterization, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enableConservativeRasterization );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTemporalSmoothing, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enableTemporalSmoothing );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserOcclusion, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enableOcclusion );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSecondaryBounce, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & vctConfig = blockContext->scene->getVoxelConeTracingConfig();
				params[0]->get( vctConfig.enableSecondaryBounce );
			}
		}
		CU_EndAttribute()
	}

	void VctConfig::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "Voxel Cone Tracing" ) );
		visitor.visit( cuT( "Enable VCT" ), enabled );
		visitor.visit( cuT( "Conservative Rasterization" ), enableConservativeRasterization );
		visitor.visit( cuT( "Occlusion" ), enableOcclusion );
		visitor.visit( cuT( "Temporal Smoothing" ), enableTemporalSmoothing );
		visitor.visit( cuT( "Secondary Bounce" ), enableSecondaryBounce );
		visitor.visit( cuT( "Num. Cones" ), numCones );
		visitor.visit( cuT( "Max. Distance" ), maxDistance );
		visitor.visit( cuT( "Ray Step Size" ), rayStepSize );
		visitor.visit( cuT( "Voxel Size" ), voxelSizeFactor );
	}

	void VctConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< SceneContext > sceneContext{ result, CSCNSection::eScene, CSCNSection::eRoot };
		BlockParserContextT< SceneContext > vctContext{ result, CSCNSection::eVoxelConeTracing, CSCNSection::eScene };

		sceneContext.addPushParser( cuT( "voxel_cone_tracing" ), CSCNSection::eVoxelConeTracing, vctcfg::parserVoxelConeTracing );
		vctContext.addParser( cuT( "enabled" ), vctcfg::parserEnabled, { makeParameter< ParameterType::eBool >() } );
		vctContext.addParser( cuT( "grid_size" ), vctcfg::parserGridSize, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 2u, VctMaxTextureSize ) ) } );
		vctContext.addParser( cuT( "num_cones" ), vctcfg::parserNumCones, { makeParameter< ParameterType::eUInt32 >( castor::Range< uint32_t >( 1u, VctMaxDiffuseCones ) ) } );
		vctContext.addParser( cuT( "max_distance" ), vctcfg::parserMaxDistance, { makeParameter< ParameterType::eFloat >() } );
		vctContext.addParser( cuT( "ray_step_size" ), vctcfg::parserRayStepSize, { makeParameter< ParameterType::eFloat >() } );
		vctContext.addParser( cuT( "voxel_size" ), vctcfg::parserVoxelSize, { makeParameter< ParameterType::eFloat >() } );
		vctContext.addParser( cuT( "conservative_rasterization" ), vctcfg::parserConservativeRasterization, { makeParameter< ParameterType::eBool >() } );
		vctContext.addParser( cuT( "temporal_smoothing" ), vctcfg::parserTemporalSmoothing, { makeParameter< ParameterType::eBool >() } );
		vctContext.addParser( cuT( "occlusion" ), vctcfg::parserOcclusion, { makeParameter< ParameterType::eBool >() } );
		vctContext.addParser( cuT( "secondary_bounce" ), vctcfg::parserSecondaryBounce, { makeParameter< ParameterType::eBool >() } );
		vctContext.addDefaultPopParser();
	}
}

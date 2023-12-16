#include "DiamondSquareTerrain/DiamondSquare_Parsers.hpp"

#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace diamond_square_terrain
{
	CU_ImplementAttributeParserNewBlock( parserDiamondSquareTerrain, castor3d::MeshContext, TerrainContext )
	{
		newBlockContext->scene = blockContext->scene;
		newBlockContext->mesh = blockContext->mesh;
	}
	CU_EndAttributePushNewBlock( DiamondSquareSection::eRoot )

	CU_ImplementAttributeParserBlock( parserDiamondSquareTerrainEnd, TerrainContext )
	{
		auto & factory = blockContext->scene->getEngine()->getMeshFactory();
		auto generator = factory.create( Generator::Type );
		auto & dsgen = static_cast< Generator & >( *generator );
		std::sort( blockContext->biomes.begin()
			, blockContext->biomes.end()
			, []( Biome const & lhs, Biome const & rhs )
			{
				return lhs.heightRange.getMin() < rhs.heightRange.getMin();
			} );
		dsgen.setBiomes( blockContext->biomes );
		generator->generate( *blockContext->mesh
			, blockContext->parameters );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParserBlock( parserRandomSeed, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			bool value;
			params[0]->get( value );

			if ( value )
			{
				blockContext->parameters.add( Generator::ParamRandomSeed, castor::string::toString( value ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserIsland, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			bool value;
			params[0]->get( value );

			if ( value )
			{
				blockContext->parameters.add( Generator::ParamIsland, castor::string::toString( value ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserXzScale, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			blockContext->parameters.add( Generator::ParamXScale, castor::string::toString( value->x ) );
			blockContext->parameters.add( Generator::ParamZScale, castor::string::toString( value->y ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserUvScale, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			blockContext->parameters.add( Generator::ParamUScale, castor::string::toString( value->x ) );
			blockContext->parameters.add( Generator::ParamVScale, castor::string::toString( value->y ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserHeightRange, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			blockContext->parameters.add( Generator::ParamYMin, castor::string::toString( value->x ) );
			blockContext->parameters.add( Generator::ParamYMax, castor::string::toString( value->y ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDetail, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			blockContext->parameters.add( Generator::ParamDetail, castor::string::toString( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserGradient, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Path folder;
			castor::Path relative;
			params[0]->get( relative );

			if ( castor::File::fileExists( context.file.getPath() / relative ) )
			{
				folder = context.file.getPath();
			}
			else if ( !castor::File::fileExists( relative ) )
			{
				CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
				relative.clear();
			}

			if ( !relative.empty() )
			{
				blockContext->parameters.add( Generator::ParamGradientFolder, folder );
				blockContext->parameters.add( Generator::ParamGradientRelative, relative );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserHeatOffset, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			float value;
			params[0]->get( value );
			blockContext->parameters.add( Generator::ParamHeatOffset, castor::string::toString( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserBiome, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->biome.name );
		}
	}
	CU_EndAttributePushBlock( DiamondSquareSection::eBiome, blockContext )

	CU_ImplementAttributeParserBlock( parserBiomeEnd, TerrainContext )
	{
		std::sort( blockContext->biome.steepnessBiomes.begin()
			, blockContext->biome.steepnessBiomes.end()
			, []( SlopeBiome const & lhs, SlopeBiome const & rhs )
			{
				return lhs.steepnessRange.getMin() < rhs.steepnessRange.getMin();
			} );
		blockContext->biomes.push_back( blockContext->biome );
		blockContext->biome = {};
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParserBlock( parserBiomeRange, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			blockContext->biome.heightRange = { value->x, value->y };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserBiomeLowSteepness, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t pass;
			params[0]->get( pass );
			castor::Point2f range;
			params[1]->get( range );
			blockContext->biome.steepnessBiomes[0].passIndex = pass;
			blockContext->biome.steepnessBiomes[0].steepnessRange = { range->x, range->y };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserBiomeMedSteepness, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t pass;
			params[0]->get( pass );
			castor::Point2f range;
			params[1]->get( range );
			blockContext->biome.steepnessBiomes[1].passIndex = pass;
			blockContext->biome.steepnessBiomes[1].steepnessRange = { range->x, range->y };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserBiomeHigSteepness, TerrainContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t pass;
			params[0]->get( pass );
			castor::Point2f range;
			params[1]->get( range );
			blockContext->biome.steepnessBiomes[2].passIndex = pass;
			blockContext->biome.steepnessBiomes[2].steepnessRange = { range->x, range->y };
		}
	}
	CU_EndAttribute()
}

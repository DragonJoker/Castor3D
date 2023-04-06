#include "DiamondSquareTerrain/DiamondSquare_Parsers.hpp"

#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace diamond_square_terrain
{
	namespace parser
	{
		static ParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( Generator::Type ) );
		}
	}

	CU_ImplementAttributeParser( parserDiamondSquareTerrain )
	{
	}
	CU_EndAttributePush( DiamondSquareSection::eRoot )

	CU_ImplementAttributeParser( parserDiamondSquareTerrainEnd )
	{
		auto & parsingContext = castor3d::getSceneParserContext( context );
		auto & pluginContext = parser::getParserContext( context );
		auto & factory = parsingContext.scene->getEngine()->getMeshFactory();
		auto generator = factory.create( Generator::Type );
		auto & dsgen = static_cast< Generator & >( *generator );
		std::sort( pluginContext.biomes.begin()
			, pluginContext.biomes.end()
			, []( Biome const & lhs, Biome const & rhs )
			{
				return lhs.heightRange.getMin() < rhs.heightRange.getMin();
			} );
		dsgen.setBiomes( pluginContext.biomes );
		generator->generate( *parsingContext.mesh.lock()
			, pluginContext.parameters );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserRandomSeed )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			bool value;
			params[0]->get( value );
			auto & pluginContext = parser::getParserContext( context );

			if ( value )
			{
				pluginContext.parameters.add( Generator::ParamRandomSeed, castor::string::toString( value ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserIsland )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			bool value;
			params[0]->get( value );
			auto & pluginContext = parser::getParserContext( context );

			if ( value )
			{
				pluginContext.parameters.add( Generator::ParamIsland, castor::string::toString( value ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserXzScale )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.parameters.add( Generator::ParamXScale, castor::string::toString( value->x ) );
			pluginContext.parameters.add( Generator::ParamZScale, castor::string::toString( value->y ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUvScale )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.parameters.add( Generator::ParamUScale, castor::string::toString( value->x ) );
			pluginContext.parameters.add( Generator::ParamVScale, castor::string::toString( value->y ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserHeightRange )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.parameters.add( Generator::ParamYMin, castor::string::toString( value->x ) );
			pluginContext.parameters.add( Generator::ParamYMax, castor::string::toString( value->y ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDetail )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.parameters.add( Generator::ParamDetail, castor::string::toString( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserGradient )
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
			auto & pluginContext = parser::getParserContext( context );

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
				pluginContext.parameters.add( Generator::ParamGradientFolder, folder );
				pluginContext.parameters.add( Generator::ParamGradientRelative, relative );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserHeatOffset )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			float value;
			params[0]->get( value );
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.parameters.add( Generator::ParamHeatOffset, castor::string::toString( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBiome )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & pluginContext = parser::getParserContext( context );
			params[0]->get( pluginContext.biome.name );
		}
	}
	CU_EndAttributePush( DiamondSquareSection::eBiome )

	CU_ImplementAttributeParser( parserBiomeEnd )
	{
		auto & pluginContext = parser::getParserContext( context );
		std::sort( pluginContext.biome.steepnessBiomes.begin()
			, pluginContext.biome.steepnessBiomes.end()
			, []( SlopeBiome const & lhs, SlopeBiome const & rhs )
			{
				return lhs.steepnessRange.getMin() < rhs.steepnessRange.getMin();
			} );
		pluginContext.biomes.push_back( pluginContext.biome );
		pluginContext.biome = {};
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserBiomeRange )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.biome.heightRange = { value->x, value->y };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBiomeLowSteepness )
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
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.biome.steepnessBiomes[0].passIndex = pass;
			pluginContext.biome.steepnessBiomes[0].steepnessRange = { range->x, range->y };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBiomeMedSteepness )
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
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.biome.steepnessBiomes[1].passIndex = pass;
			pluginContext.biome.steepnessBiomes[1].steepnessRange = { range->x, range->y };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBiomeHigSteepness )
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
			auto & pluginContext = parser::getParserContext( context );
			pluginContext.biome.steepnessBiomes[2].passIndex = pass;
			pluginContext.biome.steepnessBiomes[2].steepnessRange = { range->x, range->y };
		}
	}
	CU_EndAttribute()
}

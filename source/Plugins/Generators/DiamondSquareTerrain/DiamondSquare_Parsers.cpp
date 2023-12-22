#include "DiamondSquareTerrain/DiamondSquare_Parsers.hpp"

#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace diamond_square_terrain
{
	namespace parse
	{
		struct TerrainContext
		{
			castor3d::MeshContext * mesh{};
			castor3d::Parameters parameters{};
			Biomes biomes{};
			Biome biome{};
		};

		enum class DiamondSquareSection
			: uint32_t
		{
			eRoot = CU_MakeSectionName( 'D', 'M', 'S', 'Q' ),
			eBiome = CU_MakeSectionName( 'D', 'S', 'B', 'M' ),
		};

		static CU_ImplementAttributeParserNewBlock( parserDiamondSquareTerrain, castor3d::MeshContext, TerrainContext )
		{
			newBlockContext->mesh = blockContext;
		}
		CU_EndAttributePushNewBlock( DiamondSquareSection::eRoot )

		static CU_ImplementAttributeParserBlock( parserDiamondSquareTerrainEnd, TerrainContext )
		{
			auto & factory = getEngine( *blockContext->mesh )->getMeshFactory();
			auto generator = factory.create( Generator::Type );
			auto & dsgen = static_cast< Generator & >( *generator );
			std::sort( blockContext->biomes.begin()
				, blockContext->biomes.end()
				, []( Biome const & lhs, Biome const & rhs )
				{
					return lhs.heightRange.getMin() < rhs.heightRange.getMin();
				} );
			dsgen.setBiomes( blockContext->biomes );
			generator->generate( *blockContext->mesh->mesh
				, blockContext->parameters );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserRandomSeed, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserIsland, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserXzScale, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserUvScale, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserHeightRange, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserDetail, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserGradient, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserHeatOffset, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserBiome, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserBiomeEnd, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserBiomeRange, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserBiomeLowSteepness, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserBiomeMedSteepness, TerrainContext )
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

		static CU_ImplementAttributeParserBlock( parserBiomeHigSteepness, TerrainContext )
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

	castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;

		addParserT( result
			, castor3d::CSCNSection::eMesh
			, parse::DiamondSquareSection::eRoot
			, Generator::Type
			, &parse::parserDiamondSquareTerrain );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, castor3d::CSCNSection::eMesh
			, cuT( "}" )
			, &parse::parserDiamondSquareTerrainEnd );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamRandomSeed
			, &parse::parserRandomSeed
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamIsland
			, &parse::parserIsland
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamXzScale
			, &parse::parserXzScale
			, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamUvScale
			, &parse::parserUvScale
			, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamHeightRange
			, &parse::parserHeightRange
			, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamDetail
			, &parse::parserDetail
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamHeatOffset
			, &parse::parserHeatOffset
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, parse::DiamondSquareSection::eBiome
			, Generator::Biome
			, &parse::parserBiome
			, { castor::makeParameter< castor::ParameterType::eName >() } );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, parse::DiamondSquareSection::eRoot
			, cuT( "}" )
			, &parse::parserBiomeEnd );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, Generator::BiomeRange
			, &parse::parserBiomeRange
			, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, Generator::BiomeLowSteepness
			, &parse::parserBiomeLowSteepness
			, { castor::makeParameter< castor::ParameterType::eUInt32 >()
				, castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, Generator::BiomeMediumSteepness
			, &parse::parserBiomeMedSteepness
			, { castor::makeParameter< castor::ParameterType::eUInt32 >()
				, castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, Generator::BiomeHighSteepness
			, &parse::parserBiomeHigSteepness
			, { castor::makeParameter< castor::ParameterType::eUInt32 >()
				, castor::makeParameter< castor::ParameterType::ePoint2F >() } );

		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( parse::DiamondSquareSection::eRoot ), Generator::Type },
			{ uint32_t( parse::DiamondSquareSection::eBiome ), Generator::Biome },
		};
	}
}

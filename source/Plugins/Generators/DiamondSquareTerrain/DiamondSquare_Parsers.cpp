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
			c3d::MeshContext * mesh{};
			c3d::Parameters parameters{};
			Biomes biomes{};
			Biome biome{};
		};

		enum class DiamondSquareSection
			: c3d::SectionId
		{
			eRoot = c3d::makeSectionName( 'D', 'M', 'S', 'Q' ),
			eBiome = c3d::makeSectionName( 'D', 'S', 'B', 'M' ),
		};

		static CU_ImplementAttributeParserNewBlock( parserDiamondSquareTerrain, c3d::MeshContext, TerrainContext )
		{
			newBlockContext->mesh = blockContext;
		}
		CU_EndAttributePushNewBlock( DiamondSquareSection::eRoot )

		static CU_ImplementAttributeParserBlock( parserDiamondSquareTerrainEnd, TerrainContext )
		{
			auto const & factory = getEngine( *blockContext->mesh )->getMeshFactory();
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
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( auto value = params[0]->get< bool >() )
			{
				blockContext->parameters.add( Generator::ParamRandomSeed, c3d::string::toString( value ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserIsland, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( auto value = params[0]->get< bool >() )
			{
				blockContext->parameters.add( Generator::ParamIsland, c3d::string::toString( value ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserXzScale, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto value = params[0]->get< c3d::Point2f >();
				blockContext->parameters.add( Generator::ParamXScale, c3d::string::toString( value->x ) );
				blockContext->parameters.add( Generator::ParamZScale, c3d::string::toString( value->y ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUvScale, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto value = params[0]->get< c3d::Point2f >();
				blockContext->parameters.add( Generator::ParamUScale, c3d::string::toString( value->x ) );
				blockContext->parameters.add( Generator::ParamVScale, c3d::string::toString( value->y ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserHeightRange, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto value = params[0]->get< c3d::Point2f >();
				blockContext->parameters.add( Generator::ParamYMin, c3d::string::toString( value->x ) );
				blockContext->parameters.add( Generator::ParamYMax, c3d::string::toString( value->y ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDetail, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto value = params[0]->get< uint32_t >();
				blockContext->parameters.add( Generator::ParamDetail, c3d::string::toString( value ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserHeatOffset, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto value = params[0]->get< float >();
				blockContext->parameters.add( Generator::ParamHeatOffset, c3d::string::toString( value ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBiome, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
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
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto value = params[0]->get< c3d::Point2f >();
				blockContext->biome.heightRange = { value->x, value->y };
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBiomeLowSteepness, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto pass = params[0]->get< uint32_t >();
				auto range= params[1]->get< c3d::Point2f >();
				blockContext->biome.steepnessBiomes[0].passIndex = pass;
				blockContext->biome.steepnessBiomes[0].steepnessRange = { range->x, range->y };
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBiomeMedSteepness, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto pass = params[0]->get< uint32_t >();
				auto range = params[1]->get< c3d::Point2f >();
				blockContext->biome.steepnessBiomes[1].passIndex = pass;
				blockContext->biome.steepnessBiomes[1].steepnessRange = { range->x, range->y };
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBiomeHigSteepness, TerrainContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				auto pass = params[0]->get< uint32_t >();
				auto range = params[1]->get< c3d::Point2f >();
				blockContext->biome.steepnessBiomes[2].passIndex = pass;
				blockContext->biome.steepnessBiomes[2].steepnessRange = { range->x, range->y };
			}
		}
		CU_EndAttribute()
	}

	c3d::AttributeParsers createParsers()
	{
		c3d::AttributeParsers result;

		addParserT( result
			, c3d::CSCNSection::eMesh
			, parse::DiamondSquareSection::eRoot
			, Generator::Type
			, &parse::parserDiamondSquareTerrain );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, c3d::CSCNSection::eMesh
			, cuT( "}" )
			, &parse::parserDiamondSquareTerrainEnd );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamRandomSeed
			, &parse::parserRandomSeed
			, { c3d::makeParameter< c3d::ParameterType::eBool >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamIsland
			, &parse::parserIsland
			, { c3d::makeParameter< c3d::ParameterType::eBool >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamXzScale
			, &parse::parserXzScale
			, { c3d::makeParameter< c3d::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamUvScale
			, &parse::parserUvScale
			, { c3d::makeParameter< c3d::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamHeightRange
			, &parse::parserHeightRange
			, { c3d::makeParameter< c3d::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamDetail
			, &parse::parserDetail
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, Generator::ParamHeatOffset
			, &parse::parserHeatOffset
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::DiamondSquareSection::eRoot
			, parse::DiamondSquareSection::eBiome
			, Generator::Biome
			, &parse::parserBiome
			, { c3d::makeParameter< c3d::ParameterType::eName >() } );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, parse::DiamondSquareSection::eRoot
			, cuT( "}" )
			, &parse::parserBiomeEnd );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, Generator::BiomeRange
			, &parse::parserBiomeRange
			, { c3d::makeParameter< c3d::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, Generator::BiomeLowSteepness
			, &parse::parserBiomeLowSteepness
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >()
				, c3d::makeParameter< c3d::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, Generator::BiomeMediumSteepness
			, &parse::parserBiomeMedSteepness
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >()
				, c3d::makeParameter< c3d::ParameterType::ePoint2F >() } );
		addParserT( result
			, parse::DiamondSquareSection::eBiome
			, Generator::BiomeHighSteepness
			, &parse::parserBiomeHigSteepness
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >()
				, c3d::makeParameter< c3d::ParameterType::ePoint2F >() } );

		return result;
	}

	c3d::StrSectionIdMap createSections()
	{
		return
		{
			{ c3d::SectionId( parse::DiamondSquareSection::eRoot ), Generator::Type },
			{ c3d::SectionId( parse::DiamondSquareSection::eBiome ), Generator::Biome },
		};
	}
}

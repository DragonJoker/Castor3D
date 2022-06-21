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
		factory.create( Generator::Type )->generate( *parsingContext.mesh.lock()
			, pluginContext.parameters );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserRoughness )
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
			pluginContext.parameters.add( Generator::ParamRoughness, castor::string::toString( value ) );
		}
	}
	CU_EndAttribute()

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
			pluginContext.parameters.add( Generator::ParamRandomSeed, castor::string::toString( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserScale )
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
			pluginContext.parameters.add( Generator::ParamScale, castor::string::toString( value ) );
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

	CU_ImplementAttributeParser( parserMinY )
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
			pluginContext.parameters.add( Generator::ParamMinY, castor::string::toString( value ) );
		}
	}
	CU_EndAttribute()
}

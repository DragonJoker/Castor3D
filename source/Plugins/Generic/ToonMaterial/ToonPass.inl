/*
See LICENSE file in root folder
*/
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace toon
{
	//*********************************************************************************************

	namespace
	{
#define ImplementTemplateAttributeParser( funcname )\
		template< typename TypeT >\
		CU_ImplementAttributeParser( funcname )

		ImplementTemplateAttributeParser( parserPassEdgeColour )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonPassT< TypeT > & >( *parsingContext.pass );
				castor::RgbaColour value;
				params[0]->get( value );
				toonPass.setEdgeColour( value );
			}
		}
		CU_EndAttribute()

		ImplementTemplateAttributeParser( parserPassEdgeWidth )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonPassT< TypeT > & >( *parsingContext.pass );
				float value;
				params[0]->get( value );
				toonPass.setEdgeWidth( value );
			}
		}
		CU_EndAttribute()

		ImplementTemplateAttributeParser( parserPassDepthFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonPassT< TypeT > & >( *parsingContext.pass );
				float value;
				params[0]->get( value );
				toonPass.setDepthFactor( value );
			}
		}
		CU_EndAttribute()

		ImplementTemplateAttributeParser( parserPassNormalFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonPassT< TypeT > & >( *parsingContext.pass );
				float value;
				params[0]->get( value );
				toonPass.setNormalFactor( value );
			}
		}
		CU_EndAttribute()

		ImplementTemplateAttributeParser( parserPassObjectFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonPassT< TypeT > & >( *parsingContext.pass );
				float value;
				params[0]->get( value );
				toonPass.setObjectFactor( value );
			}
		}
		CU_EndAttribute()

		ImplementTemplateAttributeParser( parserPassSmoothBandWidth )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & toonPass = static_cast< ToonPassT< TypeT > & >( *parsingContext.pass );
				float value;
				params[0]->get( value );
				toonPass.setSmoothBandWidth( value );
			}
		}
		CU_EndAttribute()

#undef ImplementTemplateAttributeParser
	}

	//*********************************************************************************************

	template< typename TypeT >
	castor::String const ToonPassT< TypeT >::Name = cuT( "Toon Materials" );

	template< typename TypeT >
	ToonPassT< TypeT >::ToonPassT( castor3d::Material & parent
		, castor3d::PassTypeID typeID
		, castor3d::PassFlags initialFlags )
		: TypeT{ parent
			, typeID
			, initialFlags }
		, m_edgeColour{ this->m_dirty, castor::RgbaColour::fromComponents( 0.0f, 0.0f, 0.0f, 1.0f ) }
		, m_edgeWidth{ this->m_dirty, { 1.0f, castor::makeRange( MinMaterialEdgeWidth, MaxMaterialEdgeWidth ) } }
		, m_depthFactor{ this->m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
		, m_normalFactor{ this->m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
		, m_objectFactor{ this->m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
		, m_smoothBandWidth{ this->m_dirty, 1.0f }
	{
	}


	template< typename TypeT >
	castor::AttributeParsers ToonPassT< TypeT >::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID
		, uint32_t remapSectionID
		, castor::String const & remapSectionName
		, uint32_t remapChannelSectionID )
	{
		auto result = TypeT::createParsers( mtlSectionID
			, texSectionID
			, remapSectionID
			, remapSectionName
			, remapChannelSectionID );
		castor3d::Pass::addParser( result, mtlSectionID
			, cuT( "smooth_band_width" )
			, parserPassSmoothBandWidth< TypeT >
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor3d::Pass::addParser( result, mtlSectionID, cuT( "edge_colour" )
			, parserPassEdgeColour< TypeT >
			, { castor::makeParameter< castor::ParameterType::eRgbaColour >() } );
		castor3d::Pass::addParser( result, mtlSectionID, cuT( "edge_width" )
			, parserPassEdgeWidth< TypeT >
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( MinMaterialEdgeWidth, MaxMaterialEdgeWidth ) ) } );
		castor3d::Pass::addParser( result, mtlSectionID, cuT( "edge_depth_factor" )
			, parserPassDepthFactor< TypeT >
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 1.0f ) ) } );
		castor3d::Pass::addParser( result, mtlSectionID, cuT( "edge_normal_factor" )
			, parserPassNormalFactor< TypeT >
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 1.0f ) ) } );
		castor3d::Pass::addParser( result, mtlSectionID, cuT( "edge_object_factor" )
			, parserPassObjectFactor< TypeT >
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 1.0f ) ) } );
		return result;
	}

	template< typename TypeT >
	void ToonPassT< TypeT >::fillData( ToonProfileData & data )const
	{
		data.edgeWidth = getEdgeWidth();
		data.depthFactor = getDepthFactor();
		data.normalFactor = getNormalFactor();
		data.objectFactor = getObjectFactor();
		data.edgeColour->x = powf( getEdgeColour().red(), 2.2f );
		data.edgeColour->y = powf( getEdgeColour().green(), 2.2f );
		data.edgeColour->z = powf( getEdgeColour().blue(), 2.2f );
		data.edgeColour->w = getEdgeColour().alpha();
		data.smoothBand = getSmoothBandWidth();
	}

	template< typename TypeT >
	void ToonPassT< TypeT >::cloneData( ToonPassT & result )const
	{
		result.setEdgeColour( getEdgeColour() );
		result.setEdgeWidth( getEdgeWidth() );
		result.setDepthFactor( getDepthFactor() );
		result.setNormalFactor( getNormalFactor() );
		result.setObjectFactor( getObjectFactor() );
		result.setSmoothBandWidth( getSmoothBandWidth() );
	}

	template< typename TypeT >
	void ToonPassT< TypeT >::doAccept( castor3d::PassVisitorBase & vis )
	{
		TypeT::doAccept( vis );
		vis.visit( cuT( "Edge colour" )
			, m_edgeColour );
		vis.visit( cuT( "Edge width" )
			, m_edgeWidth );
		vis.visit( cuT( "Depth factor" )
			, m_depthFactor );
		vis.visit( cuT( "Normal factor" )
			, m_normalFactor );
		vis.visit( cuT( "Object factor" )
			, m_objectFactor );
		vis.visit( cuT( "Smooth band width" )
			, m_smoothBandWidth );
	}

	template< typename TypeT >
	void ToonPassT< TypeT >::doAccept( castor3d::TextureConfiguration & configuration
		, castor3d::PassVisitorBase & vis )
	{
		TypeT::doAccept( configuration, vis );
	}

	//*********************************************************************************************
}

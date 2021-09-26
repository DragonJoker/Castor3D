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

		ImplementTemplateAttributeParser( parserPassSmoothBandWidth )
		{
			auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

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
		: TypeT{ parent, typeID, initialFlags | castor3d::PassFlag::eDrawEdge }
		, m_smoothBandWidth{ this->m_dirty, 1.0f }
	{
	}


	template< typename TypeT >
	castor::AttributeParsers ToonPassT< TypeT >::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID )
	{
		auto result = TypeT::createParsers( mtlSectionID, texSectionID );
		castor3d::Pass::addParser( result, mtlSectionID
			, cuT( "smooth_band_width" )
			, parserPassSmoothBandWidth< TypeT >
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		return result;
	}

	template< typename TypeT >
	void ToonPassT< TypeT >::fillData( castor3d::PassBuffer::PassDataPtr & data )const
	{
		data.specific->r = getSmoothBandWidth();
	}

	template< typename TypeT >
	void ToonPassT< TypeT >::doAccept( castor3d::PassVisitorBase & vis )
	{
		TypeT::doAccept( vis );
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

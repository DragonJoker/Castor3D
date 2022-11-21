#include "EdgesComponent.hpp"

#include "Shaders/GlslToonProfile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Material/Texture/TextureConfiguration.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>
#include <CastorUtils/Data/Text/TextRgbaColour.hpp>

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace toon
{
	//*********************************************************************************************

	template< typename TypeT >
	class TextWriter
		: public castor::TextWriterT< EdgesComponent >
	{
	public:
		explicit TextWriter( castor::String const & tabs )
			: castor::TextWriterT< EdgesComponent >{ tabs }
		{
		}

		bool operator()( EdgesComponent const & pass
			, castor::StringStream & file )override
		{
			castor3d::log::info << this->tabs() << cuT( "Writing Toon data " ) << std::endl;
			return this->writeOpt( file, "smooth_band_width", pass.getSmoothBandWidth(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_width" ), pass.getEdgeWidth(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_depth_factor" ), pass.getDepthFactor(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_normal_factor" ), pass.getNormalFactor(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_object_factor" ), pass.getObjectFactor(), 1.0f )
				&& this->writeNamedSubOpt( file, cuT( "edge_colour" ), pass.getEdgeColour(), castor::HdrRgbaColour::fromPredefined( castor::PredefinedRgbaColour::eOpaqueBlack ) );
		}
	};

	//*********************************************************************************************

	namespace toonpass
	{
		static CU_ImplementAttributeParser( parserPassEdgeColour )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				castor::HdrRgbaColour value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< EdgesComponent >( parsingContext );
				component.setEdgeColour( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassEdgeWidth )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< EdgesComponent >( parsingContext );
				component.setEdgeWidth( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassDepthFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< EdgesComponent >( parsingContext );
				component.setDepthFactor( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassNormalFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< EdgesComponent >( parsingContext );
				component.setNormalFactor( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassObjectFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< EdgesComponent >( parsingContext );
				component.setObjectFactor( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassSmoothBandWidth )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< EdgesComponent >( parsingContext );
				component.setSmoothBandWidth( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void EdgesComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, castor3d::shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eSpecifics )
				&& !checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eSpecularLighting ) )
			|| !materials.hasSpecificsBuffer< shader::ToonProfile >() )
		{
			return;
		}

		if ( !components.hasMember( "edgeColour" ) )
		{
			components.declMember( "edgeColour", sdw::type::Kind::eVec4F );
			components.declMember( "edgeWidth", sdw::type::Kind::eFloat );
			components.declMember( "depthFactor", sdw::type::Kind::eFloat );
			components.declMember( "normalFactor", sdw::type::Kind::eFloat );
			components.declMember( "objectFactor", sdw::type::Kind::eFloat );
			components.declMember( "smoothBand", sdw::type::Kind::eFloat );
		}
	}

	void EdgesComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, castor3d::shader::Materials const & materials
		, castor3d::shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "edgeColour" ) )
		{
			return;
		}

		if ( material )
		{
			auto & writer = *material->getWriter();
			auto & toonProfiles = materials.getSpecificsBuffer< shader::ToonProfile >();
			auto toonProfile = writer.declLocale( "toonProfile"
				, toonProfiles.getData( material->passId - 1u ) );
			inits.emplace_back( sdw::makeExpr( toonProfile.edgeColour() ) );
			inits.emplace_back( sdw::makeExpr( toonProfile.edgeWidth() ) );
			inits.emplace_back( sdw::makeExpr( toonProfile.depthFactor() ) );
			inits.emplace_back( sdw::makeExpr( toonProfile.normalFactor() ) );
			inits.emplace_back( sdw::makeExpr( toonProfile.objectFactor() ) );
			inits.emplace_back( sdw::makeExpr( toonProfile.smoothBand() ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec4( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void EdgesComponent::ComponentsShader::blendComponents( castor3d::shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, castor3d::shader::BlendComponents & res
		, castor3d::shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "edgeColour" ) )
		{
			res.getMember< sdw::Vec4 >( "edgeColour" ) += src.getMember< sdw::Vec4 >( "edgeColour" ) * passMultiplier;
			res.getMember< sdw::Float >( "edgeWidth" ) += src.getMember< sdw::Float >( "edgeWidth" ) * passMultiplier;
			res.getMember< sdw::Float >( "depthFactor" ) += src.getMember< sdw::Float >( "depthFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "normalFactor" ) += src.getMember< sdw::Float >( "normalFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "objectFactor" ) += src.getMember< sdw::Float >( "objectFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "smoothBand" ) += src.getMember< sdw::Float >( "smoothBand" ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	void EdgesComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, castor3d::ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, castor3d::CSCNSection::ePass
			, cuT( "smooth_band_width" )
			, toonpass::parserPassSmoothBandWidth
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, castor3d::CSCNSection::ePass
			, cuT( "edge_width" )
			, toonpass::parserPassEdgeWidth
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, castor3d::CSCNSection::ePass
			, cuT( "edge_depth_factor" )
			, toonpass::parserPassDepthFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, castor3d::CSCNSection::ePass
			, cuT( "edge_normal_factor" )
			, toonpass::parserPassNormalFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, castor3d::CSCNSection::ePass
			, cuT( "edge_object_factor" )
			, toonpass::parserPassObjectFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, castor3d::CSCNSection::ePass
			, cuT( "edge_colour" )
			, toonpass::parserPassEdgeColour
			, { castor::makeParameter< castor::ParameterType::eHdrRgbaColour >() } );
	}

	bool EdgesComponent::Plugin::isComponentNeeded( castor3d::TextureCombine const & textures
		, castor3d::ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, castor3d::ComponentModeFlag::eColour );
	}

	//*********************************************************************************************

	castor::String const EdgesComponent::TypeName = C3D_PluginMakePassOtherComponentName( "toon", "edges" );

	EdgesComponent::EdgesComponent( castor3d::Pass & pass )
		: BaseDataPassComponentT< EdgesData >{ pass, TypeName }
	{
	}

	void EdgesComponent::accept( castor3d::PassVisitorBase & vis )
	{
		vis.visit( cuT( "Edge colour" )
			, *m_value.edgeColour
			, &m_value.edgeColour.control() );
		vis.visit( cuT( "Edge width" )
			, *m_value.edgeWidth
			, &m_value.edgeWidth.control() );
		vis.visit( cuT( "Depth factor" )
			, *m_value.depthFactor
			, &m_value.depthFactor.control() );
		vis.visit( cuT( "Normal factor" )
			, *m_value.normalFactor
			, &m_value.normalFactor.control() );
		vis.visit( cuT( "Object factor" )
			, *m_value.objectFactor
			, &m_value.objectFactor.control() );
		vis.visit( cuT( "Smooth band width" )
			, *m_value.smoothBand
			, &m_value.smoothBand.control() );
	}

	void EdgesComponent::fillProfileBuffer( ToonProfileData & data )const
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

	castor3d::PassComponentUPtr EdgesComponent::doClone( castor3d::Pass & pass )const
	{
		auto result = std::make_unique< EdgesComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool EdgesComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return TextWriter< EdgesComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}

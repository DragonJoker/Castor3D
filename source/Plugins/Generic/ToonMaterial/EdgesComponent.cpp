#include "EdgesComponent.hpp"

#include "Shaders/GlslToonProfile.hpp"

#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>
#include <CastorUtils/Data/Text/TextRgbaColour.hpp>

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

CU_ImplementSmartPtr( toon, EdgesComponent )

namespace toon
{
	//*********************************************************************************************

	template< typename TypeT >
	class TextWriter
		: public c3d::TextWriterT< EdgesComponent >
	{
	public:
		explicit TextWriter( c3d::String const & tabs )
			: c3d::TextWriterT< EdgesComponent >{ tabs }
		{
		}

		bool operator()( EdgesComponent const & pass
			, c3d::StringStream & file )override
		{
			c3d::log::info << this->tabs() << cuT( "Writing Toon data " ) << std::endl;
			return this->writeOpt( file, cuT( "smooth_band_width" ), pass.getSmoothBandWidth(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_width" ), pass.getEdgeWidth(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_depth_factor" ), pass.getDepthFactor(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_normal_factor" ), pass.getNormalFactor(), 1.0f )
				&& this->writeOpt( file, cuT( "edge_object_factor" ), pass.getObjectFactor(), 1.0f )
				&& this->writeNamedSubOpt( file, cuT( "edge_colour" ), pass.getEdgeColour(), c3d::HdrRgbaColour::fromPredefined( c3d::PredefinedRgbaColour::eOpaqueBlack ) );
		}
	};

	//*********************************************************************************************

	namespace toonpass
	{
		static CU_ImplementAttributeParserBlock( parserPassEdgeColour, c3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = c3d::getPassComponent< EdgesComponent >( *blockContext );
				component.setEdgeColour( params[0]->get< c3d::HdrRgbaColour >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassEdgeWidth, c3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = c3d::getPassComponent< EdgesComponent >( *blockContext );
				component.setEdgeWidth( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassDepthFactor, c3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = c3d::getPassComponent< EdgesComponent >( *blockContext );
				component.setDepthFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassNormalFactor, c3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = c3d::getPassComponent< EdgesComponent >( *blockContext );
				component.setNormalFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassObjectFactor, c3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = c3d::getPassComponent< EdgesComponent >( *blockContext );
				component.setObjectFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassSmoothBandWidth, c3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = c3d::getPassComponent< EdgesComponent >( *blockContext );
				component.setSmoothBandWidth( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void EdgesComponent::ComponentsShader::fillComponents( c3d::ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, c3d::shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, c3d::ComponentModeFlag::eSpecifics )
				&& !checkFlag( componentsMask, c3d::ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( componentsMask, c3d::ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), c3d::ComponentModeFlag::eSpecifics )
				&& !checkFlag( materials.getFilter(), c3d::ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), c3d::ComponentModeFlag::eSpecularLighting ) )
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
		, c3d::shader::Materials const & materials
		, c3d::shader::Material const * material
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

	void EdgesComponent::ComponentsShader::blendComponents( c3d::shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, c3d::shader::BlendComponents & res
		, c3d::shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "edgeColour" ) )
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

	void EdgesComponent::Plugin::createParsers( c3d::AttributeParsers & parsers
		, c3d::ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, c3d::CSCNSection::ePass
			, cuT( "smooth_band_width" )
			, toonpass::parserPassSmoothBandWidth
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, c3d::CSCNSection::ePass
			, cuT( "edge_width" )
			, toonpass::parserPassEdgeWidth
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, c3d::CSCNSection::ePass
			, cuT( "edge_depth_factor" )
			, toonpass::parserPassDepthFactor
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, c3d::CSCNSection::ePass
			, cuT( "edge_normal_factor" )
			, toonpass::parserPassNormalFactor
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, c3d::CSCNSection::ePass
			, cuT( "edge_object_factor" )
			, toonpass::parserPassObjectFactor
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		c3d::addParserT( parsers
			, c3d::CSCNSection::ePass
			, cuT( "edge_colour" )
			, toonpass::parserPassEdgeColour
			, { c3d::makeParameter< c3d::ParameterType::eHdrRgbaColour >() } );
	}

	bool EdgesComponent::Plugin::isComponentNeeded( c3d::TextureCombine const & textures
		, c3d::ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, c3d::ComponentModeFlag::eColour );
	}

	//*********************************************************************************************

	c3d::String const EdgesComponent::TypeName = C3D_PluginMakePassOtherComponentName( "toon", "edges" );

	EdgesComponent::EdgesComponent( c3d::Pass & pass )
		: BaseDataPassComponentT< EdgesData >{ pass, TypeName }
	{
	}

	void EdgesComponent::accept( c3d::ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Toon" ) );
		vis.visit( cuT( "Edge colour" )
			, m_value.edgeColour );
		vis.visit( cuT( "Edge width" )
			, m_value.edgeWidth );
		vis.visit( cuT( "Depth factor" )
			, m_value.depthFactor );
		vis.visit( cuT( "Normal factor" )
			, m_value.normalFactor );
		vis.visit( cuT( "Object factor" )
			, m_value.objectFactor );
		vis.visit( cuT( "Smooth band width" )
			, m_value.smoothBand );
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

	c3d::PassComponentUPtr EdgesComponent::doClone( c3d::Pass & pass )const
	{
		auto result = c3d::makeRawUnique< EdgesComponent >( pass );
		result->setData( getData() );
		return c3d::passComponentCast( result );
	}

	bool EdgesComponent::doWriteText( c3d::String const & tabs
		, c3d::Path const & folder
		, c3d::String const & subfolder
		, c3d::StringStream & file )const
	{
		return TextWriter< EdgesComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}

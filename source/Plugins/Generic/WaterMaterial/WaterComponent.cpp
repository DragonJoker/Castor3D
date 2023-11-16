#include "WaterComponent.hpp"

#include "Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Material/Pass/Component/Base/BlendComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

CU_ImplementSmartPtr( water, WaterComponent )

namespace water
{
	//*********************************************************************************************

	template< typename TypeT >
	class TextWriter
		: public castor::TextWriterT< WaterComponent >
	{
	public:
		explicit TextWriter( castor::String const & tabs )
			: castor::TextWriterT< WaterComponent >{ tabs }
		{
		}

		bool operator()( WaterComponent const & pass
			, castor::StringStream & file )override
		{
			castor3d::log::info << this->tabs() << cuT( "Writing Water data " ) << std::endl;
			return this->writeOpt( file, "dampeningFactor", pass.getDampeningFactor(), 5.0f )
				&& this->writeOpt( file, cuT( "refractionDistortionFactor" ), pass.getRefractionDistortionFactor(), 0.04f )
				&& this->writeOpt( file, cuT( "refractionHeightFactor" ), pass.getRefractionHeightFactor(), 2.5f )
				&& this->writeOpt( file, cuT( "refractionDistanceFactor" ), pass.getRefractionDistanceFactor(), 15.0f )
				&& this->writeOpt( file, cuT( "depthSofteningDistance" ), pass.getDepthSofteningDistance(), 0.5f )
				&& this->writeOpt( file, cuT( "ssrStepSize" ), pass.getSsrStepSize(), 0.5f )
				&& this->writeOpt( file, cuT( "ssrForwardStepsCount" ), pass.getSsrForwardStepsCount(), 20u )
				&& this->writeOpt( file, cuT( "ssrBackwardStepsCount" ), pass.getSsrBackwardStepsCount(), 10u )
				&& this->writeOpt( file, cuT( "ssrDepthMult" ), pass.getSsrDepthMult(), 20.0f );
		}
	};

	//*********************************************************************************************

	namespace waterpass
	{
		static CU_ImplementAttributeParser( parserDampeningFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setDampeningFactor( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserDepthSofteningDistance )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setDepthSofteningDistance( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserRefrDistortionFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setRefractionDistortionFactor( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserRefrHeightFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setRefractionHeightFactor( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserRefrDistanceFactor )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setRefractionDistanceFactor( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSsrStepSize )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setSsrStepSize( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSsrFwdStepCount )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setSsrForwardStepsCount( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSsrBckStepCount )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setSsrBackwardStepsCount( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserSsrDepthMult )
		{
			auto & parsingContext = castor3d::getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				float value;
				params[0]->get( value );
				auto & component = castor3d::getPassComponent< WaterComponent >( parsingContext );
				component.setSsrDepthMult( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void WaterComponent::ComponentsShader::fillComponents( castor3d::ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, castor3d::shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, castor3d::ComponentModeFlag::eSpecifics )
				&& !checkFlag( componentsMask, castor3d::ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( componentsMask, castor3d::ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eSpecifics )
				&& !checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eSpecularLighting ) )
			|| !materials.hasSpecificsBuffer< shader::WaterProfile >() )
		{
			return;
		}

		if ( !components.hasMember( "dampeningFactor" ) )
		{
			components.declMember( "dampeningFactor", sdw::type::Kind::eFloat );
			components.declMember( "depthSofteningDistance", sdw::type::Kind::eFloat );
			components.declMember( "refractionDistortionFactor", sdw::type::Kind::eFloat );
			components.declMember( "refractionHeightFactor", sdw::type::Kind::eFloat );
			components.declMember( "refractionDistanceFactor", sdw::type::Kind::eFloat );
			components.declMember( "ssrStepSize", sdw::type::Kind::eFloat );
			components.declMember( "ssrForwardStepsCount", sdw::type::Kind::eUInt32 );
			components.declMember( "ssrBackwardStepsCount", sdw::type::Kind::eUInt32 );
			components.declMember( "ssrDepthMult", sdw::type::Kind::eFloat );
		}
	}

	void WaterComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, castor3d::shader::Materials const & materials
		, castor3d::shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "dampeningFactor" ) )
		{
			return;
		}

		if ( material )
		{
			auto & writer = *material->getWriter();
			auto & waterProfiles = materials.getSpecificsBuffer< shader::WaterProfile >();
			auto waterProfile = writer.declLocale( "waterProfile"
				, waterProfiles.getData( material->passId - 1u ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.dampeningFactor() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.depthSofteningDistance() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.refractionDistortionFactor() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.refractionHeightFactor() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.refractionDistanceFactor() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.ssrStepSize() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.ssrForwardStepsCount() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.ssrBackwardStepsCount() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.ssrDepthMult() ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void WaterComponent::ComponentsShader::blendComponents( castor3d::shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, castor3d::shader::BlendComponents & res
		, castor3d::shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "dampeningFactor" ) )
		{
			res.getMember< sdw::Float >( "dampeningFactor" ) += src.getMember< sdw::Float >( "dampeningFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "depthSofteningDistance" ) += src.getMember< sdw::Float >( "depthSofteningDistance" ) * passMultiplier;
			res.getMember< sdw::Float >( "refractionDistortionFactor" ) += src.getMember< sdw::Float >( "refractionDistortionFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "refractionHeightFactor" ) += src.getMember< sdw::Float >( "refractionHeightFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "refractionDistanceFactor" ) += src.getMember< sdw::Float >( "refractionDistanceFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "ssrStepSize" ) += src.getMember< sdw::Float >( "ssrStepSize" ) * passMultiplier;
			res.getMember< sdw::UInt32 >( "ssrForwardStepsCount" ) = max( res.getMember< sdw::UInt32 >( "ssrForwardStepsCount" ), src.getMember< sdw::UInt32 >( "ssrForwardStepsCount" ) );
			res.getMember< sdw::UInt32 >( "ssrBackwardStepsCount" ) = max( res.getMember< sdw::UInt32 >( "ssrBackwardStepsCount" ), src.getMember< sdw::UInt32 >( "ssrBackwardStepsCount" ) );
			res.getMember< sdw::Float >( "ssrDepthMult" ) += src.getMember< sdw::Float >( "ssrDepthMult" ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	void WaterComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, castor3d::ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "dampeningFactor" )
			, &waterpass::parserDampeningFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "refractionDistortionFactor" )
			, &waterpass::parserRefrDistortionFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "refractionHeightFactor" )
			, &waterpass::parserRefrHeightFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "refractionDistanceFactor" )
			, &waterpass::parserRefrDistanceFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "depthSofteningDistance" )
			, &waterpass::parserDepthSofteningDistance, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "ssrStepSize" )
			, &waterpass::parserSsrStepSize, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "ssrForwardStepsCount" )
			, &waterpass::parserSsrFwdStepCount, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "ssrBackwardStepsCount" )
			, &waterpass::parserSsrBckStepCount, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "ssrDepthMult" )
			, &waterpass::parserSsrDepthMult, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	bool WaterComponent::Plugin::isComponentNeeded( castor3d::TextureCombine const & textures
		, castor3d::ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, castor3d::ComponentModeFlag::eColour );
	}

	//*********************************************************************************************

	castor::String const WaterComponent::TypeName = C3D_PluginMakePassOtherComponentName( "water", "base" );

	WaterComponent::WaterComponent( castor3d::Pass & pass )
		: BaseDataPassComponentT< WaterData >{ pass, TypeName
			, { castor3d::BlendComponent::TypeName
				, castor3d::OpacityComponent::TypeName
				, castor3d::TransmissionComponent::TypeName } }
	{
		auto blend = pass.createComponent< castor3d::BlendComponent >();
		blend->setAlphaBlendMode( castor3d::BlendMode::eInterpolative );
		auto transmission = pass.createComponent< castor3d::TransmissionComponent >();
		transmission->setTransmission( 1.0f );
	}

	void WaterComponent::accept( castor3d::ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Water" ) );
		vis.visit( cuT( "Dampening factor" ), m_value.dampeningFactor );
		vis.visit( cuT( "Depth softening distance" ), m_value.depthSofteningDistance );
		vis.visit( cuT( "Refraction distortion factor" ), m_value.refractionDistortionFactor );
		vis.visit( cuT( "Refraction height factor" ), m_value.refractionHeightFactor );
		vis.visit( cuT( "Refraction distance factor" ), m_value.refractionDistanceFactor );
		vis.visit( cuT( "SSR step size" ), m_value.ssrStepSize );
		vis.visit( cuT( "SSR forward steps count" ), m_value.ssrForwardStepsCount );
		vis.visit( cuT( "SSR backward steps count" ), m_value.ssrBackwardStepsCount );
		vis.visit( cuT( "SSR depth mult." ), m_value.ssrDepthMult );
	}

	void WaterComponent::fillProfileBuffer( WaterProfileData & data )const
	{
		data.dampeningFactor = getDampeningFactor();
		data.depthSofteningDistance = getDepthSofteningDistance();
		data.refractionDistortionFactor = getRefractionDistortionFactor();
		data.refractionHeightFactor = getRefractionHeightFactor();
		data.refractionDistanceFactor = getRefractionDistanceFactor();
		data.ssr.stepSize = getSsrStepSize();
		data.ssr.forwardStepsCount = getSsrForwardStepsCount();
		data.ssr.backwardStepsCount = getSsrBackwardStepsCount();
		data.ssr.depthMult = getSsrDepthMult();
	}

	bool WaterComponent::isComponentAvailable( castor3d::ComponentModeFlags componentsMask
		, castor3d::shader::Materials const & materials )
	{
		return materials.hasSpecificsBuffer< shader::WaterProfile >()
			&& ( checkFlag( componentsMask, castor3d::ComponentModeFlag::eSpecifics )
				|| checkFlag( componentsMask, castor3d::ComponentModeFlag::eDiffuseLighting )
				|| checkFlag( componentsMask, castor3d::ComponentModeFlag::eSpecularLighting ) )
			&& ( checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eSpecifics )
				|| checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eDiffuseLighting )
				|| checkFlag( materials.getFilter(), castor3d::ComponentModeFlag::eSpecularLighting ) );
	}

	castor3d::PassComponentUPtr WaterComponent::doClone( castor3d::Pass & pass )const
	{
		auto result = castor::makeUnique< WaterComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool WaterComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return TextWriter< WaterComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}

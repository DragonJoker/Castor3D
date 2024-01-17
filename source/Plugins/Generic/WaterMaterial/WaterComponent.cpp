#include "WaterComponent.hpp"

#include "WaterReflRefrComponent.hpp"
#include "Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Base/BlendComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
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
				&& this->writeOpt( file, cuT( "depthSofteningDistance" ), pass.getDepthSofteningDistance(), 0.5f )
				&& this->writeOpt( file, cuT( "noiseTiling" ), pass.getNoiseTiling(), 1.0f )
				&& this->writeOpt( file, cuT( "refractionDistortionFactor" ), pass.getRefractionDistortionFactor(), 0.04f )
				&& this->writeOpt( file, cuT( "refractionHeightFactor" ), pass.getRefractionHeightFactor(), 2.5f )
				&& this->writeOpt( file, cuT( "refractionDistanceFactor" ), pass.getRefractionDistanceFactor(), 15.0f )
				&& this->writeOpt( file, cuT( "ssrStepSize" ), pass.getSsrStepSize(), 0.5f )
				&& this->writeOpt( file, cuT( "ssrForwardStepsCount" ), pass.getSsrForwardStepsCount(), 20u )
				&& this->writeOpt( file, cuT( "ssrBackwardStepsCount" ), pass.getSsrBackwardStepsCount(), 10u )
				&& this->writeOpt( file, cuT( "ssrDepthMult" ), pass.getSsrDepthMult(), 20.0f );
		}
	};

	//*********************************************************************************************

	namespace waterpass
	{
		static CU_ImplementAttributeParserBlock( parserDampeningFactor, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setDampeningFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDepthSofteningDistance, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setDepthSofteningDistance( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNoiseTiling, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setNoiseTiling( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRefrDistortionFactor, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setRefractionDistortionFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRefrHeightFactor, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setRefractionHeightFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRefrDistanceFactor, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setRefractionDistanceFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSsrStepSize, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setSsrStepSize( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSsrFwdStepCount, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setSsrForwardStepsCount( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSsrBckStepCount, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setSsrBackwardStepsCount( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSsrDepthMult, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setSsrDepthMult( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFoamHeightStart, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setFoamHeightStart( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFoamFadeDistance, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setFoamFadeDistance( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFoamTiling, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setFoamTiling( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFoamNoiseTiling, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setFoamNoiseTiling( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFoamAngleExponent, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setFoamAngleExponent( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFoamBrightness, castor3d::PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				auto & component = castor3d::getPassComponent< WaterComponent >( *blockContext );
				component.setFoamBrightness( params[0]->get< float >() );
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
		if ( !isComponentAvailable( componentsMask, materials ) )
		{
			return;
		}

		if ( !components.hasMember( "dampeningFactor" ) )
		{
			components.declMember( "dampeningFactor", sdw::type::Kind::eFloat );
			components.declMember( "depthSofteningDistance", sdw::type::Kind::eFloat );
			components.declMember( "noiseTiling", sdw::type::Kind::eFloat );
			components.declMember( "refractionDistortionFactor", sdw::type::Kind::eFloat );
			components.declMember( "refractionHeightFactor", sdw::type::Kind::eFloat );
			components.declMember( "refractionDistanceFactor", sdw::type::Kind::eFloat );
			components.declMember( "ssrStepSize", sdw::type::Kind::eFloat );
			components.declMember( "ssrForwardStepsCount", sdw::type::Kind::eUInt32 );
			components.declMember( "ssrBackwardStepsCount", sdw::type::Kind::eUInt32 );
			components.declMember( "ssrDepthMult", sdw::type::Kind::eFloat );
			components.declMember( "foamHeightStart", sdw::type::Kind::eFloat );
			components.declMember( "foamFadeDistance", sdw::type::Kind::eFloat );
			components.declMember( "foamTiling", sdw::type::Kind::eFloat );
			components.declMember( "foamNoiseTiling", sdw::type::Kind::eFloat );
			components.declMember( "foamAngleExponent", sdw::type::Kind::eFloat );
			components.declMember( "foamBrightness", sdw::type::Kind::eFloat );
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
			inits.emplace_back( sdw::makeExpr( waterProfile.noiseTiling() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.refractionDistortionFactor() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.refractionHeightFactor() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.refractionDistanceFactor() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.ssrStepSize() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.ssrForwardStepsCount() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.ssrBackwardStepsCount() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.ssrDepthMult() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.foamHeightStart() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.foamFadeDistance() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.foamTiling() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.foamNoiseTiling() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.foamAngleExponent() ) );
			inits.emplace_back( sdw::makeExpr( waterProfile.foamBrightness() ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
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
			res.getMember< sdw::Float >( "noiseTiling" ) += src.getMember< sdw::Float >( "noiseTiling" ) * passMultiplier;
			res.getMember< sdw::Float >( "refractionDistortionFactor" ) += src.getMember< sdw::Float >( "refractionDistortionFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "refractionHeightFactor" ) += src.getMember< sdw::Float >( "refractionHeightFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "refractionDistanceFactor" ) += src.getMember< sdw::Float >( "refractionDistanceFactor" ) * passMultiplier;
			res.getMember< sdw::Float >( "ssrStepSize" ) += src.getMember< sdw::Float >( "ssrStepSize" ) * passMultiplier;
			res.getMember< sdw::UInt32 >( "ssrForwardStepsCount" ) = max( res.getMember< sdw::UInt32 >( "ssrForwardStepsCount" ), src.getMember< sdw::UInt32 >( "ssrForwardStepsCount" ) );
			res.getMember< sdw::UInt32 >( "ssrBackwardStepsCount" ) = max( res.getMember< sdw::UInt32 >( "ssrBackwardStepsCount" ), src.getMember< sdw::UInt32 >( "ssrBackwardStepsCount" ) );
			res.getMember< sdw::Float >( "ssrDepthMult" ) += src.getMember< sdw::Float >( "ssrDepthMult" ) * passMultiplier;
			res.getMember< sdw::Float >( "foamHeightStart" ) += src.getMember< sdw::Float >( "foamHeightStart" ) * passMultiplier;
			res.getMember< sdw::Float >( "foamFadeDistance" ) += src.getMember< sdw::Float >( "foamFadeDistance" ) * passMultiplier;
			res.getMember< sdw::Float >( "foamTiling" ) += src.getMember< sdw::Float >( "foamTiling" ) * passMultiplier;
			res.getMember< sdw::Float >( "foamNoiseTiling" ) += src.getMember< sdw::Float >( "foamNoiseTiling" ) * passMultiplier;
			res.getMember< sdw::Float >( "foamAngleExponent" ) += src.getMember< sdw::Float >( "foamAngleExponent" ) * passMultiplier;
			res.getMember< sdw::Float >( "foamBrightness" ) += src.getMember< sdw::Float >( "foamBrightness" ) * passMultiplier;
		}
	}

	void WaterComponent::ComponentsShader::updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, castor3d::shader::Material const & material
		, castor3d::shader::BlendComponents & components
		, bool isFrontCulled )const
	{
		if ( !components.hasMember( "normal" )
			|| !components.hasMember( "tangent" )
			|| !components.hasMember( "bitangent" )
			|| ( !components.hasMember( "waterNormals2" ) && !components.hasMember( "waterNormals1" ) ) )
		{
			return;
		}

		auto & writer{ *components.getWriter() };
		auto tbn = writer.declLocale( "waterTBN"
			, castor3d::shader::Utils::getTBN( components.normal
				, components.getMember< sdw::Vec4 >( "tangent" ).xyz()
				, components.getMember< sdw::Vec3 >( "bitangent" ) ) );
		auto finalNormal = writer.declLocale( "finalNormal"
			, vec3( 0.0_f ) );

		if ( components.hasMember( "waterNormals1" ) )
		{
			auto waterNormals1 = components.getMember< sdw::Vec3 >( "waterNormals1" );
			finalNormal += normalize( tbn * waterNormals1.xyz() );
		}

		if ( components.hasMember( "waterNormals2" ) )
		{
			auto waterNormals2 = components.getMember< sdw::Vec3 >( "waterNormals2" );
			finalNormal += normalize( tbn * waterNormals2.xyz() );
		}

		if ( components.hasMember( "waterNormals1" )
			|| components.hasMember( "waterNormals2" ) )
		{
			if ( isFrontCulled )
			{
				finalNormal = -finalNormal;
			}

			components.normal = normalize( finalNormal );
		}

		if ( components.hasMember( "transmission" ) )
		{
			components.transmission = 1.0_f;
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
			, cuT( "noiseTiling" )
			, &waterpass::parserNoiseTiling, { castor::makeParameter< castor::ParameterType::eFloat >() } );
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
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "foamHeightStart" )
			, &waterpass::parserFoamHeightStart, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "foamFadeDistance" )
			, &waterpass::parserFoamFadeDistance, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "foamTiling" )
			, &waterpass::parserFoamTiling, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "foamNoiseTiling" )
			, &waterpass::parserFoamNoiseTiling, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "foamAngleExponent" )
			, &waterpass::parserFoamAngleExponent, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, uint32_t( castor3d::CSCNSection::ePass )
			, cuT( "foamBrightness" )
			, &waterpass::parserFoamBrightness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
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
				, castor3d::TransmissionComponent::TypeName
				, WaterReflRefrComponent::TypeName } }
	{
	}

	void WaterComponent::onAddToPass()const
	{
		auto & pass = *getOwner();
		pass.getComponent< castor3d::BlendComponent >()->setAlphaBlendMode( castor3d::BlendMode::eInterpolative );
		pass.getComponent< castor3d::TransmissionComponent >()->setTransmission( 1.0f );
	}

	void WaterComponent::accept( castor3d::ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Water" ) );
		vis.visit( cuT( "Dampening factor" ), m_value.dampeningFactor );
		vis.visit( cuT( "Depth softening distance" ), m_value.depthSofteningDistance );
		vis.visit( cuT( "Noise Tiling" ), m_value.noiseTiling );
		vis.visit( cuT( "Refraction distortion factor" ), m_value.refractionDistortionFactor );
		vis.visit( cuT( "Refraction height factor" ), m_value.refractionHeightFactor );
		vis.visit( cuT( "Refraction distance factor" ), m_value.refractionDistanceFactor );
		vis.visit( cuT( "SSR step size" ), m_value.ssrStepSize );
		vis.visit( cuT( "SSR forward steps count" ), m_value.ssrForwardStepsCount );
		vis.visit( cuT( "SSR backward steps count" ), m_value.ssrBackwardStepsCount );
		vis.visit( cuT( "SSR depth mult." ), m_value.ssrDepthMult );
		vis.visit( cuT( "Foam Height Start" ), m_value.foamHeightStart );
		vis.visit( cuT( "Foam Fade Distance" ), m_value.foamFadeDistance );
		vis.visit( cuT( "Foam Tiling" ), m_value.foamTiling );
		vis.visit( cuT( "Foam Noise Tiling" ), m_value.foamNoiseTiling );
		vis.visit( cuT( "Foam Angle Exponent" ), m_value.foamAngleExponent );
		vis.visit( cuT( "Foam Brightness" ), m_value.foamBrightness );
	}

	void WaterComponent::fillProfileBuffer( WaterProfileData & data )const
	{
		data.dampeningFactor = getDampeningFactor();
		data.depthSofteningDistance = getDepthSofteningDistance();
		data.noiseTiling = getNoiseTiling();
		data.refractionDistortionFactor = getRefractionDistortionFactor();
		data.refractionHeightFactor = getRefractionHeightFactor();
		data.refractionDistanceFactor = getRefractionDistanceFactor();
		data.ssr.stepSize = getSsrStepSize();
		data.ssr.forwardStepsCount = getSsrForwardStepsCount();
		data.ssr.backwardStepsCount = getSsrBackwardStepsCount();
		data.ssr.depthMult = getSsrDepthMult();
		data.foamHeightStart = getFoamHeightStart();
		data.foamFadeDistance = getFoamFadeDistance();
		data.foamTiling = getFoamTiling();
		data.foamNoiseTiling = getFoamNoiseTiling();
		data.foamAngleExponent = getFoamAngleExponent();
		data.foamBrightness = getFoamBrightness();
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

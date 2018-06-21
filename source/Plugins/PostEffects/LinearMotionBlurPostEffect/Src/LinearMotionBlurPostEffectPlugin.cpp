#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Scene/SceneFileParser.hpp>

#include "LinearMotionBlurParsers.hpp"
#include "LinearMotionBlurPostEffect.hpp"

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_LinearMotionBlur_API
#else
#	ifdef LinearMotionBlurPostEffect_EXPORTS
#		define C3D_LinearMotionBlur_API __declspec( dllexport )
#	else
#		define C3D_LinearMotionBlur_API __declspec( dllimport )
#	endif
#endif

namespace
{
	void parseError( castor::String const & error )
	{
		castor::StringStream stream{ castor::makeStringStream() };
		stream << cuT( "Error, : " ) << error;
		castor::Logger::logError( stream.str() );
	}

	void addParser( castor::FileParser::AttributeParsersBySection & parsers
		, uint32_t section
		, castor::String const & name
		, castor::ParserFunction function
		, castor::ParserParameterArray && array = castor::ParserParameterArray{} )
	{
		auto sectionIt = parsers.find( section );

		if ( sectionIt != parsers.end()
			&& sectionIt->second.find( name ) != sectionIt->second.end() )
		{
			parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
		}
		else
		{
			parsers[section][name] = { function, array };
		}
	}

	castor::FileParser::AttributeParsersBySection createParsers()
	{
		castor::FileParser::AttributeParsersBySection result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "linear_motion_blur" ), &motion_blur::parserMotionBlur );

		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "vectorDivider" ), &motion_blur::parserDivider, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "samples" ), &motion_blur::parserSamples, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "fpsScale" ), &motion_blur::parserFpsScale, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "}" ), &motion_blur::parserMotionBlurEnd );

		return result;
	}

	castor::StrUIntMap createSections()
	{
		return
		{
			{ uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "motion_blur" ) },
		};
	}
}

extern "C"
{
	C3D_LinearMotionBlur_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_LinearMotionBlur_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_LinearMotionBlur_API void getName( char const ** p_name )
	{
		*p_name = motion_blur::PostEffect::Name.c_str();
	}

	C3D_LinearMotionBlur_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getRenderTargetCache().getPostEffectFactory().registerType( motion_blur::PostEffect::Type
			, &motion_blur::PostEffect::create );
		engine->registerParsers( motion_blur::PostEffect::Type, createParsers() );
		engine->registerSections( motion_blur::PostEffect::Type, createSections() );
	}

	C3D_LinearMotionBlur_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( motion_blur::PostEffect::Type );
		engine->getRenderTargetCache().getPostEffectFactory().unregisterType( motion_blur::PostEffect::Type );
	}
}

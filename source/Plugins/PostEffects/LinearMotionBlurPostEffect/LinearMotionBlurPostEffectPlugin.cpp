#include "LinearMotionBlurPostEffect/LinearMotionBlurParsers.hpp"
#include "LinearMotionBlurPostEffect/LinearMotionBlurPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>

#ifndef CU_PlatformWindows
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

	void addParser( castor::AttributeParsersBySection & parsers
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

	castor::AttributeParsersBySection createParsers()
	{
		castor::AttributeParsersBySection result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "linear_motion_blur" ), &motion_blur::parserMotionBlur );

		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "vectorDivider" ), &motion_blur::parserDivider, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "samples" ), &motion_blur::parserSamples, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "fpsScale" ), &motion_blur::parserFpsScale, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( motion_blur::MotionBlurSection::eRoot ), cuT( "}" ), &motion_blur::parserMotionBlurEnd );

		return result;
	}

	castor::StrUInt32Map createSections()
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
		engine->unregisterSections( motion_blur::PostEffect::Type );
		engine->unregisterParsers( motion_blur::PostEffect::Type );
		engine->getRenderTargetCache().getPostEffectFactory().unregisterType( motion_blur::PostEffect::Type );
	}
}

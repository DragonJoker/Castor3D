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
	castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;

		addParserT( result
			, castor3d::CSCNSection::eRenderTarget
			, motion_blur::MotionBlurSection::eRoot
			, cuT( "linear_motion_blur" )
			, &motion_blur::parserMotionBlur );

		addParserT( result
			, motion_blur::MotionBlurSection::eRoot
			, cuT( "vectorDivider" )
			, &motion_blur::parserDivider, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, motion_blur::MotionBlurSection::eRoot
			, cuT( "samples" )
			, &motion_blur::parserSamples, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, motion_blur::MotionBlurSection::eRoot
			, cuT( "fpsScale" )
			, &motion_blur::parserFpsScale, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParserT( result
			, motion_blur::MotionBlurSection::eRoot
			, castor3d::CSCNSection::eRenderTarget
			, cuT( "}" )
			, &motion_blur::parserMotionBlurEnd );

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
	C3D_LinearMotionBlur_API void getRequiredVersion( castor3d::Version * version );
	C3D_LinearMotionBlur_API void getType( castor3d::PluginType * type );
	C3D_LinearMotionBlur_API void getName( char const ** name );
	C3D_LinearMotionBlur_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_LinearMotionBlur_API void OnUnload( castor3d::Engine * engine );

	C3D_LinearMotionBlur_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_LinearMotionBlur_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_LinearMotionBlur_API void getName( char const ** name )
	{
		*name = motion_blur::PostEffect::Name.c_str();
	}

	C3D_LinearMotionBlur_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getPostEffectFactory().registerType( motion_blur::PostEffect::Type
			, &motion_blur::PostEffect::create );
		engine->registerParsers( motion_blur::PostEffect::Type
			, createParsers()
			, createSections()
			, nullptr );
	}

	C3D_LinearMotionBlur_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( motion_blur::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( motion_blur::PostEffect::Type );
	}
}

#include <Log/Logger.hpp>

#include <Engine.hpp>
#include <Cache/TargetCache.hpp>

#include <Plugin/PostFxPlugin.hpp>
#include <Render/RenderSystem.hpp>

#include "BloomPostEffect.hpp"

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_Bloom_API
#else
#	ifdef BloomPostEffect_EXPORTS
#		define C3D_Bloom_API __declspec( dllexport )
#	else
#		define C3D_Bloom_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_Bloom_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Bloom_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::ePostEffect;
	}

	C3D_Bloom_API void GetName( char const ** p_name )
	{
		*p_name = Bloom::BloomPostEffect::Name.c_str();
	}

	C3D_Bloom_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		p_engine->GetRenderTargetCache().GetPostEffectFactory().Register( Bloom::BloomPostEffect::Type
			, &Bloom::BloomPostEffect::Create );
	}

	C3D_Bloom_API void OnUnload( Castor3D::Engine * p_engine )
	{
		p_engine->GetRenderTargetCache().GetPostEffectFactory().Unregister( Bloom::BloomPostEffect::Type );
	}
}

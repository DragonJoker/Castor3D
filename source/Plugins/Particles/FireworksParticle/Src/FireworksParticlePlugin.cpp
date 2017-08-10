#include <Log/Logger.hpp>

#include <Engine.hpp>

#include <Plugin/ParticlePlugin.hpp>
#include <Render/RenderSystem.hpp>

#include "FireworksParticle.hpp"

#ifndef CASTOR_PLATFORM_WINDOWS
#	define C3D_Fireworks_API
#else
#	ifdef FireworksParticle_EXPORTS
#		define C3D_Fireworks_API __declspec( dllexport )
#	else
#		define C3D_Fireworks_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_Fireworks_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Fireworks_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::ePostEffect;
	}

	C3D_Fireworks_API void getName( char const ** p_name )
	{
		*p_name = Fireworks::ParticleSystem::Name.c_str();
	}

	C3D_Fireworks_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->getParticleFactory().registerType( Fireworks::ParticleSystem::Type
			, &Fireworks::ParticleSystem::create );
	}

	C3D_Fireworks_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getParticleFactory().unregisterType( Fireworks::ParticleSystem::Type );
	}
}

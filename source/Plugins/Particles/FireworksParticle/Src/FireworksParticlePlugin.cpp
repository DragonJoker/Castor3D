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
	C3D_Fireworks_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Fireworks_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::ePostEffect;
	}

	C3D_Fireworks_API void GetName( char const ** p_name )
	{
		*p_name = Fireworks::ParticleSystem::Name.c_str();
	}

	C3D_Fireworks_API void OnLoad( Castor3D::Engine * engine, Castor3D::Plugin * p_plugin )
	{
		engine->GetParticleFactory().Register( Fireworks::ParticleSystem::Type
			, &Fireworks::ParticleSystem::Create );
	}

	C3D_Fireworks_API void OnUnload( Castor3D::Engine * engine )
	{
		engine->GetParticleFactory().Unregister( Fireworks::ParticleSystem::Type );
	}
}

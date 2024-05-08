#include "FireworksParticle/FireworksParticle.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/ParticlePlugin.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleModule.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Design/Factory.hpp>

#ifndef CU_PlatformWindows
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
	C3D_Fireworks_API void getRequiredVersion( castor3d::Version * version );
	C3D_Fireworks_API void getType( castor3d::PluginType * type );
	C3D_Fireworks_API void isDebug( int * value );
	C3D_Fireworks_API void getName( char const ** name );
	C3D_Fireworks_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_Fireworks_API void OnUnload( castor3d::Engine * engine );

	C3D_Fireworks_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Fireworks_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_Fireworks_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_Fireworks_API void getName( char const ** name )
	{
		*name = fireworks::ParticleSystem::Name.c_str();
	}

	C3D_Fireworks_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getParticleFactory().registerType( fireworks::ParticleSystem::Type
			, &fireworks::ParticleSystem::create );
	}

	C3D_Fireworks_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getParticleFactory().unregisterType( fireworks::ParticleSystem::Type );
	}
}

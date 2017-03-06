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

C3D_Fireworks_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_Fireworks_API Castor3D::PluginType GetType()
{
	return Castor3D::PluginType::eParticle;
}

C3D_Fireworks_API Castor::String GetName()
{
	return Fireworks::ParticleSystem::Name;
}

C3D_Fireworks_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetParticleFactory().Register( Fireworks::ParticleSystem::Type, &Fireworks::ParticleSystem::Create );
}

C3D_Fireworks_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetParticleFactory().Unregister( Fireworks::ParticleSystem::Type );
}

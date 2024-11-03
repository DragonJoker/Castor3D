#include "AnisotropicMaterial/AnisotropyComponent.hpp"
#include "AnisotropicMaterial/AnisotropyDirectionMapComponent.hpp"
#include "AnisotropicMaterial/AnisotropyStrengthMapComponent.hpp"
#include "AnisotropicMaterial/Shaders/GlslAnisotropicBRDF.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>

extern "C"
{
	C3D_AnisotropicMaterial_API void getRequiredVersion( castor3d::Version * version );
	C3D_AnisotropicMaterial_API void getType( castor3d::PluginType * type );
	C3D_AnisotropicMaterial_API void isDebug( int * value );
	C3D_AnisotropicMaterial_API void getName( char const ** name );
	C3D_AnisotropicMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_AnisotropicMaterial_API void OnUnload( castor3d::Engine * engine );

	C3D_AnisotropicMaterial_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_AnisotropicMaterial_API void isDebug( int * value )
	{
		*value = castor::system::isDebug() ? 1 : 0;
	}

	C3D_AnisotropicMaterial_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_AnisotropicMaterial_API void getName( char const ** name )
	{
		*name = "Anisotropic Material";
	}

	C3D_AnisotropicMaterial_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerPassComponent< anisotropy::AnisotropyComponent >();
		engine->registerPassComponent< anisotropy::AnisotropyDirectionMapComponent >();
		engine->registerPassComponent< anisotropy::AnisotropyStrengthMapComponent >();
		engine->registerSpecularBrdf( { castor::String{ anisotropy::shader::AnisotropicBRDF::Name }
			, anisotropy::shader::AnisotropicBRDF::create } );
	}

	C3D_AnisotropicMaterial_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterSpecularBrdf( castor::String{ anisotropy::shader::AnisotropicBRDF::Name } );
		engine->unregisterPassComponent( anisotropy::AnisotropyStrengthMapComponent::TypeName );
		engine->unregisterPassComponent( anisotropy::AnisotropyDirectionMapComponent::TypeName );
		engine->unregisterPassComponent( anisotropy::AnisotropyComponent::TypeName );
	}
}

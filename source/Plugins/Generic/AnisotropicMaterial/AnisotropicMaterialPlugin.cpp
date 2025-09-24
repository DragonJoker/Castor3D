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
	C3D_AnisotropicMaterial_API void getRequiredVersion( c3d::Version * version );
	C3D_AnisotropicMaterial_API void getType( c3d::PluginType * type );
	C3D_AnisotropicMaterial_API void isDebug( int * value );
	C3D_AnisotropicMaterial_API void getName( char const ** name );
	C3D_AnisotropicMaterial_API void onLoad( c3d::Engine * engine );
	C3D_AnisotropicMaterial_API void onUnload( c3d::Engine * engine );

	C3D_AnisotropicMaterial_API void getRequiredVersion( c3d::Version * version )
	{
		*version = c3d::Version();
	}

	C3D_AnisotropicMaterial_API void isDebug( int * value )
	{
		*value = c3d::system::isDebug() ? 1 : 0;
	}

	C3D_AnisotropicMaterial_API void getType( c3d::PluginType * type )
	{
		*type = c3d::PluginType::eGeneric;
	}

	C3D_AnisotropicMaterial_API void getName( char const ** name )
	{
		*name = "Anisotropic Material";
	}

	C3D_AnisotropicMaterial_API void onLoad( c3d::Engine * engine )
	{
		engine->registerPassComponent< anisotropy::AnisotropyComponent >();
		engine->registerPassComponent< anisotropy::AnisotropyDirectionMapComponent >();
		engine->registerPassComponent< anisotropy::AnisotropyStrengthMapComponent >();
		engine->registerSpecularBrdf( { c3d::String{ anisotropy::shader::AnisotropicBRDF::Name }
			, anisotropy::shader::AnisotropicBRDF::create } );
	}

	C3D_AnisotropicMaterial_API void onUnload( c3d::Engine * engine )
	{
		engine->unregisterSpecularBrdf( c3d::String{ anisotropy::shader::AnisotropicBRDF::Name } );
		engine->unregisterPassComponent( anisotropy::AnisotropyStrengthMapComponent::TypeName );
		engine->unregisterPassComponent( anisotropy::AnisotropyDirectionMapComponent::TypeName );
		engine->unregisterPassComponent( anisotropy::AnisotropyComponent::TypeName );
	}
}

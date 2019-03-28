#include "Gl3RenderSystem/Gl3RenderSystem.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/RendererPlugin.hpp>
#include <Castor3D/Render/RenderSystemFactory.hpp>

extern "C"
{
	C3D_Gl3_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Gl3_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eRenderer;
	}

	C3D_Gl3_API void getName( char const ** p_name )
	{
		*p_name = Gl3Render::RenderSystem::Name.c_str();
	}

	C3D_Gl3_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::RendererPlugin * >( p_plugin );
		plugin->setRendererType( Gl3Render::RenderSystem::Type );
		engine->getRenderSystemFactory().registerType( Gl3Render::RenderSystem::Type
			, &Gl3Render::RenderSystem::create );
	}

	C3D_Gl3_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderSystemFactory().unregisterType( Gl3Render::RenderSystem::Type );
	}
}

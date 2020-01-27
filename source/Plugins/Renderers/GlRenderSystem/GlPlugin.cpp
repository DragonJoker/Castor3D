#include "GlRenderSystem/GlRenderSystem.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin/RendererPlugin.hpp>
#include <Castor3D/Render/RenderSystemFactory.hpp>

#ifdef CU_PlatformWindows
#	ifdef castor3dGlRenderSystem_EXPORTS
#		define C3D_Gl_API __declspec( dllexport )
#	else
#		define C3D_Gl_API __declspec( dllimport )
#	endif
#else
#	define C3D_Gl_API
#endif

extern "C"
{
	C3D_Gl_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Gl_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eRenderer;
	}

	C3D_Gl_API void getName( char const ** p_name )
	{
		*p_name = castor3d::gl::RenderSystem::Name.c_str();
	}

	C3D_Gl_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::RendererPlugin * >( p_plugin );
		plugin->setRendererType( castor3d::gl::RenderSystem::Type );
		engine->getRenderSystemFactory().registerType( castor3d::gl::RenderSystem::Type
			, &castor3d::gl::RenderSystem::create );
	}

	C3D_Gl_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderSystemFactory().unregisterType( castor3d::gl::RenderSystem::Type );
	}
}

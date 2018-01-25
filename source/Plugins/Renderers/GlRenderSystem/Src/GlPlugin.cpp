#include "GlRenderSystem.hpp"

#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace GlRender;
using namespace castor3d;
using namespace castor;

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
		*p_name = GlRenderSystem::Name.c_str();
	}

	C3D_Gl_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::RendererPlugin * >( p_plugin );
		plugin->setRendererType( GlRenderSystem::Type );
		engine->getRenderSystemFactory().registerType( GlRenderSystem::Type
			, &GlRenderSystem::create );
	}

	C3D_Gl_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderSystemFactory().unregisterType( GlRenderSystem::Type );
	}
}

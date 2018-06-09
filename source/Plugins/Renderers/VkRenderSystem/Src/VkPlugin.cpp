#include "VkRenderSystem.hpp"

#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>
#include <Render/RenderSystemFactory.hpp>

extern "C"
{
	C3D_Vk_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Vk_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eRenderer;
	}

	C3D_Vk_API void getName( char const ** p_name )
	{
		*p_name = VkRender::RenderSystem::Name.c_str();
	}

	C3D_Vk_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::RendererPlugin * >( p_plugin );
		plugin->setRendererType( VkRender::RenderSystem::Type );
		engine->getRenderSystemFactory().registerType( VkRender::RenderSystem::Type
			, &VkRender::RenderSystem::create );
	}

	C3D_Vk_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderSystemFactory().unregisterType( VkRender::RenderSystem::Type );
	}
}

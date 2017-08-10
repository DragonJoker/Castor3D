#include "Render/TestRenderSystem.hpp"

#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace TestRender;
using namespace castor3d;
using namespace castor;

extern "C"
{
	C3D_Test_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_Test_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eRenderer;
	}

	C3D_Test_API void getName( char const ** p_name )
	{
		*p_name = TestRenderSystem::Name.c_str();
	}

	C3D_Test_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		auto plugin = static_cast< castor3d::RendererPlugin * >( p_plugin );
		plugin->setRendererType( TestRenderSystem::Type );
		engine->getRenderSystemFactory().registerType( TestRenderSystem::Type
			, &TestRenderSystem::create );
	}

	C3D_Test_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getRenderSystemFactory().unregisterType( TestRenderSystem::Type );
	}
}

#include "Render/TestRenderSystem.hpp"

#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace TestRender;
using namespace Castor3D;
using namespace Castor;

extern "C"
{
	C3D_Test_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Test_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eRenderer;
	}

	C3D_Test_API void GetName( char const ** p_name )
	{
		*p_name = TestRenderSystem::Name.c_str();
	}

	C3D_Test_API void OnLoad( Castor3D::Engine * engine, Castor3D::Plugin * p_plugin )
	{
		auto plugin = static_cast< Castor3D::RendererPlugin * >( p_plugin );
		plugin->SetRendererType( TestRenderSystem::Type );
		engine->GetRenderSystemFactory().Register( TestRenderSystem::Type
			, &TestRenderSystem::Create );
	}

	C3D_Test_API void OnUnload( Castor3D::Engine * engine )
	{
		engine->GetRenderSystemFactory().Unregister( TestRenderSystem::Type );
	}
}

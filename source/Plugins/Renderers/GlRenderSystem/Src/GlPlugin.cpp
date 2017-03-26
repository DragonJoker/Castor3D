#include "Render/GlRenderSystem.hpp"

#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

extern "C"
{
	C3D_Gl_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_Gl_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eRenderer;
	}

	C3D_Gl_API void GetName( char const ** p_name )
	{
		*p_name = GlRenderSystem::Name.c_str();
	}

	C3D_Gl_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		auto l_plugin = static_cast< Castor3D::RendererPlugin * >( p_plugin );
		l_plugin->SetRendererType( GlRenderSystem::Type );
		p_engine->GetRenderSystemFactory().Register( GlRenderSystem::Type
			, &GlRenderSystem::Create );
	}

	C3D_Gl_API void OnUnload( Castor3D::Engine * p_engine )
	{
		p_engine->GetRenderSystemFactory().Unregister( GlRenderSystem::Type );
	}
}

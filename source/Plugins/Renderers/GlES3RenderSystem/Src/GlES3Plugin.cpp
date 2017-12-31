#include "Render/GlES3RenderSystem.hpp"

#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace GlES3Render;
using namespace Castor3D;
using namespace Castor;

extern "C"
{
	void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eRenderer;
	}

	void GetName( char const ** p_name )
	{
		*p_name = GlES3RenderSystem::Name.c_str();
	}

	void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		auto l_plugin = static_cast< Castor3D::RendererPlugin * >( p_plugin );
		l_plugin->SetRendererType( GlES3RenderSystem::Type );
		p_engine->GetRenderSystemFactory().Register( GlES3RenderSystem::Type
			, &GlES3RenderSystem::Create );
	}

	void OnUnload( Castor3D::Engine * p_engine )
	{
		p_engine->GetRenderSystemFactory().Unregister( GlES3RenderSystem::Type );
	}
}

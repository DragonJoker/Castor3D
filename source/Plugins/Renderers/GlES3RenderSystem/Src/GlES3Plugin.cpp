#include "Render/GlES3RenderSystem.hpp"

#include <Engine.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace GlES3Render;
using namespace Castor3D;
using namespace Castor;

void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

PluginType GetType()
{
	return PluginType::eRenderer;
}

Castor::String GetRendererType()
{
	return GlES3RenderSystem::Name;
}

String GetName()
{
	return cuT( "OpenGL ES 3.x Renderer" );
}

void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderSystemFactory().Register( GlES3RenderSystem::Name, GlES3RenderSystem::Create );
}

void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderSystemFactory().Unregister( GlES3RenderSystem::Name );
}

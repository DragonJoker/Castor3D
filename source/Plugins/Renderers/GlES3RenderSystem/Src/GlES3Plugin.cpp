#include "Render/GlES3RenderSystem.hpp"

#include <Engine.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace GlES3Render;
using namespace Castor3D;
using namespace Castor;

C3D_GlES3_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_GlES3_API PluginType GetType()
{
	return PluginType::eRenderer;
}

C3D_GlES3_API Castor::String GetRendererType()
{
	return GlES3RenderSystem::Name;
}

C3D_GlES3_API String GetName()
{
	return cuT( "OpenGL ES 3.0 Renderer" );
}

C3D_GlES3_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderSystemFactory().Register( GlES3RenderSystem::Name, GlES3RenderSystem::Create );
}

C3D_GlES3_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderSystemFactory().Unregister( GlES3RenderSystem::Name );
}

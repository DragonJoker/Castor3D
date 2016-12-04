#include "Render/GlRenderSystem.hpp"

#include <Engine.hpp>
#include <Render/RenderSystemFactory.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

C3D_Gl_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Gl_API PluginType GetType()
{
	return PluginType::eRenderer;
}

C3D_Gl_API Castor::String GetRendererType()
{
	return GlRenderSystem::Name;
}

C3D_Gl_API String GetName()
{
	return cuT( "OpenGL Renderer" );
}

C3D_Gl_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderSystemFactory().Register( GlRenderSystem::Name, GlRenderSystem::Create );
}

C3D_Gl_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetRenderSystemFactory().Unregister( GlRenderSystem::Name );
}

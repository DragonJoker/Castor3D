#include "GlRenderSystem.hpp"

#include <Engine.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

C3D_Gl_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Gl_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_RENDERER;
}

C3D_Gl_API eRENDERER_TYPE GetRendererType()
{
	return eRENDERER_TYPE_OPENGL;
}

C3D_Gl_API String GetName()
{
	return cuT( "OpenGL Renderer" );
}

C3D_Gl_API RenderSystem * CreateRenderSystem( Engine * p_pEngine )
{
	return new GlRenderSystem( p_pEngine );
}

C3D_Gl_API void DestroyRenderSystem( RenderSystem * p_pRenderSystem )
{
	delete p_pRenderSystem;
}

C3D_Gl_API void OnLoad( Castor3D::Engine * p_engine )
{
	Castor::Logger::Initialise( p_engine->GetLoggerInstance() );
}

C3D_Gl_API void OnUnload( Castor3D::Engine * p_engine )
{
	Castor::Logger::Cleanup();
}

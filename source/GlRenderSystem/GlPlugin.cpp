#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlPlugin.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"

using namespace Castor3D;

//******************************************************************************

C3D_Gl_API void GetRequiredVersion( Version & p_version)
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
	return cuT( "OpenGL Renderer");
}

C3D_Gl_API RenderSystem * CreateRenderSystem()
{
	return new GlRenderSystem();
}

//******************************************************************************

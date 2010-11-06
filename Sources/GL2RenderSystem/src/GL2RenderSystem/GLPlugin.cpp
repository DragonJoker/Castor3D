#include "PrecompiledHeader.h"

#include "GLPlugin.h"
#include "GLRenderSystem.h"

using namespace Castor3D;

//******************************************************************************

extern "C" CS3D_GL_API void GetRequiredVersion( int & p_version)
{
	p_version = 1;
}

extern "C" CS3D_GL_API void RegisterPlugin( Root & p_root)
{
	p_root.GetRendererServer().AddRendererDriver( new GLRendererDriver());
}

//******************************************************************************

const String & GLRendererDriver :: GetName() const
{
	static String l_name( "OpenGL graphics driver");
	return l_name;
}

/// Create a renderer
RenderSystem * GLRendererDriver :: CreateRenderSystem()
{
	GLRenderSystem * l_system = new GLRenderSystem;
//	Root::SetRenderSystem( l_system);
	return l_system;
}

//******************************************************************************

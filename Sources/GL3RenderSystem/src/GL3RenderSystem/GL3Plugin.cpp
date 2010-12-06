#include "PrecompiledHeader.h"

#include "GL3Plugin.h"
#include "GL3RenderSystem.h"

using namespace Castor3D;

extern "C" C3D_GL3_API void GetRequiredVersion( int & p_version)
{
	p_version = 1;
}

extern "C" C3D_GL3_API void RegisterPlugin( Root & p_root)
{
	p_root.GetRendererServer().AddRenderer( RendererDriver::eOpenGL3, RendererDriverPtr( new GL3RendererDriver()));
}

//******************************************************************************

const String & GL3RendererDriver :: GetName() const
{
	static String l_name( "OpenGL 3.x graphics driver");
	return l_name;
}

void GL3RendererDriver :: CreateRenderSystem()
{
	new GL3RenderSystem;
}

//******************************************************************************

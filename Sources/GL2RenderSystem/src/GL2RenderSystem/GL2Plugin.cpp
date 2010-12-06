#include "PrecompiledHeader.h"

#include "GL2Plugin.h"
#include "GL2RenderSystem.h"

using namespace Castor3D;

//******************************************************************************

extern "C" C3D_GL2_API void GetRequiredVersion( int & p_version)
{
	p_version = 1;
}

extern "C" C3D_GL2_API void RegisterPlugin( Root & p_root)
{
	p_root.GetRendererServer().AddRenderer( RendererDriver::eOpenGL2, RendererDriverPtr( new GL2RendererDriver()));
}

//******************************************************************************

const String & GL2RendererDriver :: GetName() const
{
	static String l_name( "OpenGL 2.x graphics driver");
	return l_name;
}

void GL2RendererDriver :: CreateRenderSystem()
{
	new GL2RenderSystem;
}

//******************************************************************************

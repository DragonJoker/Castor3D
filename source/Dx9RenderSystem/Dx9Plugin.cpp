#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9Plugin.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"

using namespace Castor3D;

//******************************************************************************

extern "C" C3D_Dx9_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 5);
}

extern "C" C3D_Dx9_API void RegisterPlugin( Root & p_root)
{
	p_root.GetRendererServer().AddRenderer( RendererDriver::eDirect3D9, RendererDriverPtr( new Dx9RendererDriver()));
}

//******************************************************************************

const String & Dx9RendererDriver :: GetName() const
{
	static String l_name( "Direct3D 9 graphics renderer");
	return l_name;
}

void Dx9RendererDriver :: CreateRenderSystem( SceneManager * p_pSceneManager)
{
	new Dx9RenderSystem( p_pSceneManager);
}

//******************************************************************************

#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9Plugin.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"

using namespace Castor3D;

//******************************************************************************

C3D_Dx9_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Dx9_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_RENDERER;
}

C3D_Dx9_API eRENDERER_TYPE GetRendererType()
{
	return eRENDERER_TYPE_DIRECT3D;
}

C3D_Dx9_API String GetName()
{
	return cuT( "Direct3D Renderer");
}

C3D_Dx9_API RenderSystem * CreateRenderSystem()
{
	return new Dx9RenderSystem();
}

//******************************************************************************

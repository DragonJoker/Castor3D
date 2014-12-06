#include "Dx11RenderSystem.hpp"

#include <Version.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

C3D_Dx11_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_Dx11_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_RENDERER;
}

C3D_Dx11_API eRENDERER_TYPE GetRendererType()
{
	return eRENDERER_TYPE_DIRECT3D11;
}

C3D_Dx11_API String GetName()
{
	return cuT( "Direct3D11 Renderer" );
}

C3D_Dx11_API RenderSystem * CreateRenderSystem( Engine * p_pEngine, Logger * p_pLogger )
{
	return new DxRenderSystem( p_pEngine, p_pLogger );
}

C3D_Dx11_API void DestroyRenderSystem( RenderSystem * p_pRenderSystem )
{
	delete p_pRenderSystem;
}

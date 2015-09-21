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
	return eRENDERER_TYPE_DIRECT3D;
}

C3D_Dx11_API String GetName()
{
	return cuT( "Direct3D11 Renderer" );
}

C3D_Dx11_API RenderSystem * CreateRenderSystem( Engine * p_engine )
{
	return new DxRenderSystem( p_engine );
}

C3D_Dx11_API void DestroyRenderSystem( RenderSystem * p_pRenderSystem )
{
	delete p_pRenderSystem;
}

C3D_Dx11_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Dx11_API void OnUnload( Castor3D::Engine * p_engine )
{
}

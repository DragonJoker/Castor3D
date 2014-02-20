#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_Direct3D11 )
#include "ShaderProgramCg/CgDx11ShaderProgram.hpp"
#include "ShaderProgramCg/CgDx11ShaderObject.hpp"
#include "ShaderProgramCg/CgDx11FrameVariable.hpp"
#include <Dx11RenderSystem/DxRenderSystem.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;

CgD3D11ShaderProgram :: CgD3D11ShaderProgram( RenderSystem * p_pRenderSystem )
	:	CgShaderProgram( p_pRenderSystem )
{
}

CgD3D11ShaderProgram :: ~CgD3D11ShaderProgram()
{
	ShaderProgramBase::Cleanup();
}

void CgD3D11ShaderProgram :: Begin( uint8_t p_byIndex, uint8_t p_byCount )
{
	m_pRenderSystem->GetPipeline()->UpdateFunctions( this );
	ShaderProgramBase::Begin( p_byIndex, p_byCount );
}

ShaderObjectBaseSPtr CgD3D11ShaderProgram :: DoCreateObject(eSHADER_TYPE p_eType)
{
	ShaderObjectBaseSPtr l_pReturn( new CgD3D11ShaderObject( this, p_eType));
	return l_pReturn;
}

std::shared_ptr< OneTextureFrameVariable > CgD3D11ShaderProgram :: DoCreateTextureVariable( int p_iNbOcc )
{
	std::shared_ptr< OneTextureFrameVariable > l_pReturn( new CgD3D11OneFrameVariable< TextureBaseRPtr >( this, p_iNbOcc ) );
	return l_pReturn;
}

#endif

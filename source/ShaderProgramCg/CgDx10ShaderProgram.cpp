#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_Direct3D10 )
#include "ShaderProgramCg/CgDx10ShaderProgram.hpp"
#include "ShaderProgramCg/CgDx10ShaderObject.hpp"
#include "ShaderProgramCg/CgDx10FrameVariable.hpp"
#include <Dx10RenderSystem/DxRenderSystem.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;

CgD3D10ShaderProgram :: CgD3D10ShaderProgram( RenderSystem * p_pRenderSystem )
	:	CgShaderProgram( p_pRenderSystem )
{
}

CgD3D10ShaderProgram :: ~CgD3D10ShaderProgram()
{
	ShaderProgramBase::Cleanup();
}

void CgD3D10ShaderProgram :: Begin( uint8_t p_byIndex, uint8_t p_byCount )
{
	m_pRenderSystem->GetPipeline()->UpdateFunctions( this );
	ShaderProgramBase::Begin( p_byIndex, p_byCount );
}

ShaderObjectBaseSPtr CgD3D10ShaderProgram :: DoCreateObject(eSHADER_TYPE p_eType)
{
	ShaderObjectBaseSPtr l_pReturn( new CgD3D10ShaderObject( this, p_eType));
	return l_pReturn;
}

std::shared_ptr< OneTextureFrameVariable > CgD3D10ShaderProgram :: DoCreateTextureVariable( int p_iNbOcc )
{
	std::shared_ptr< OneTextureFrameVariable > l_pReturn( new CgD3D10OneFrameVariable< TextureBaseRPtr >( this, p_iNbOcc ) );
	return l_pReturn;
}

#endif

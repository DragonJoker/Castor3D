#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_Direct3D9 )
#include "ShaderProgramCg/CgDx9ShaderProgram.hpp"
#include "ShaderProgramCg/CgDx9ShaderObject.hpp"
#include "ShaderProgramCg/CgDx9FrameVariable.hpp"
#include <Dx9RenderSystem/DxRenderSystem.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;

CgD3D9ShaderProgram :: CgD3D9ShaderProgram( RenderSystem * p_pRenderSystem )
	:	CgShaderProgram( p_pRenderSystem )
{
}

CgD3D9ShaderProgram :: ~CgD3D9ShaderProgram()
{
	ShaderProgramBase::Cleanup();
}

void CgD3D9ShaderProgram :: Begin( uint8_t p_byIndex, uint8_t p_byCount )
{
	m_pRenderSystem->GetPipeline()->UpdateFunctions( this );
	ShaderProgramBase::Begin( p_byIndex, p_byCount );
}

ShaderObjectBaseSPtr CgD3D9ShaderProgram :: DoCreateObject(eSHADER_TYPE p_eType)
{
	ShaderObjectBaseSPtr l_pReturn( new CgD3D9ShaderObject( this, p_eType));
	return l_pReturn;
}

std::shared_ptr< OneTextureFrameVariable > CgD3D9ShaderProgram :: DoCreateTextureVariable( int p_iNbOcc )
{
	std::shared_ptr< OneTextureFrameVariable > l_pReturn( new CgD3D9OneFrameVariable< TextureBaseRPtr >( this, p_iNbOcc ) );
	return l_pReturn;
}

#endif

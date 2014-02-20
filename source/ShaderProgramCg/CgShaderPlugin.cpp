#include "ShaderProgramCg/PrecompiledHeader.hpp"

#include "ShaderProgramCg/CgShaderPlugin.hpp"
#include "ShaderProgramCg/CgPipeline.hpp"

#if defined( Cg_OpenGL )
#	include "ShaderProgramCg/CgGlShaderProgram.hpp"
#endif
#if defined( Cg_Direct3D11 )
#	include "ShaderProgramCg/CgDx11ShaderProgram.hpp"
#endif
#if defined( Cg_Direct3D10 )
#	include "ShaderProgramCg/CgDx10ShaderProgram.hpp"
#endif
#if defined( Cg_Direct3D9 )
#	include "ShaderProgramCg/CgDx9ShaderProgram.hpp"
#endif

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;

//******************************************************************************

struct Deleter
{
	void operator()( ShaderProgramBase * p_pShaderProgram )
	{
		delete p_pShaderProgram;
	}
};

//******************************************************************************

C3D_Cg_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version();
}

C3D_Cg_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_PROGRAM;
}

C3D_Cg_API eSHADER_LANGUAGE GetShaderLanguage()
{
	return eSHADER_LANGUAGE_CG;
}

C3D_Cg_API String GetName()
{
	return cuT( "Cg Shader program" );
}

C3D_Cg_API ShaderProgramBaseSPtr CreateShader( RenderSystem * p_pRenderSystem )
{
	ShaderProgramBaseSPtr l_pReturn;

#if defined( Cg_OpenGL )
	if( p_pRenderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
	{
		l_pReturn = std::shared_ptr< CgGlShaderProgram >( new CgGlShaderProgram( p_pRenderSystem ), Deleter() );
	}
#endif
#if defined( Cg_Direct3D11 )
	if( p_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D11 && !l_pReturn )
	{
		l_pReturn = std::shared_ptr< CgD3D11ShaderProgram >( new CgD3D11ShaderProgram( p_pRenderSystem ), Deleter() );
	}
#endif
#if defined( Cg_Direct3D10 )
	if( p_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D10 && !l_pReturn )
	{
		l_pReturn = std::shared_ptr< CgD3D10ShaderProgram >( new CgD3D10ShaderProgram( p_pRenderSystem ), Deleter() );
	}
#endif
#if defined( Cg_Direct3D9 )
	if( p_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D9 && !l_pReturn )
	{
		l_pReturn = std::shared_ptr< CgD3D9ShaderProgram >( new CgD3D9ShaderProgram( p_pRenderSystem ), Deleter() );
	}
#endif
	if( !l_pReturn )
	{
		Logger::LogWarning( cuT( "CgShaderPlugin::CreateShader - Unsupported renderer type, errors may occur" ) );
	}

	return l_pReturn;
}

C3D_Cg_API IPipelineImpl * CreatePipeline( Pipeline * p_pPipeline, RenderSystem * p_pRenderSystem )
{
	Logger::Initialise( p_pRenderSystem->GetEngine()->GetLoggerInstance() );
	IPipelineImpl * l_pReturn = NULL;

#if defined( Cg_OpenGL )
	if( p_pRenderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
	{
		l_pReturn = new CgGlPipeline( reinterpret_cast< GlRender::GlPipeline* >( p_pPipeline ) );
	}
#endif
#if defined( Cg_Direct3D11 )
	if( p_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D11 && !l_pReturn )
	{
		l_pReturn = new CgD3D11Pipeline( reinterpret_cast< Dx11Render::DxPipeline* >( p_pPipeline ) );
	}
#endif
#if defined( Cg_Direct3D10 )
	if( p_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D10 && !l_pReturn )
	{
		l_pReturn = new CgD3D10Pipeline( reinterpret_cast< Dx10Render::DxPipeline* >( p_pPipeline ) );
	}
#endif
#if defined( Cg_Direct3D9 )
	if( p_pRenderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D9 && !l_pReturn )
	{
		l_pReturn = new CgD3D9Pipeline( reinterpret_cast< Dx9Render::DxPipeline* >( p_pPipeline ) );
	}
#endif
	if( !l_pReturn )
	{
		Logger::LogWarning( cuT( "CgShaderPlugin::CreatePipeline - Unsupported renderer type, errors may occur" ) );
	}

	return l_pReturn;
}

C3D_Cg_API void DestroyPipeline( IPipelineImpl * p_pPipeline )
{
	delete p_pPipeline;
	Logger::Cleanup();
}

//******************************************************************************

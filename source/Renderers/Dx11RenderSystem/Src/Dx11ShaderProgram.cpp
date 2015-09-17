#include "Dx11ShaderProgram.hpp"
#include "Dx11ShaderObject.hpp"
#include "Dx11FrameVariableBuffer.hpp"
#include "Dx11OneFrameVariable.hpp"
#include "Dx11RenderSystem.hpp"
#include "Dx11VertexShader.hpp"
#include "Dx11FragmentShader.hpp"
#include "Dx11GeometryShader.hpp"
#include "Dx11HullShader.hpp"
#include "Dx11DomainShader.hpp"

using namespace Castor3D;
using namespace Castor;
namespace Dx11Render
{
	std::unique_ptr< UniformsBase > UniformsBase::Get( DxRenderSystem const & p_renderSystem )
	{
		std::unique_ptr< UniformsBase > l_pUniforms;

		switch ( p_renderSystem.GetFeatureLevel() )
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			l_pUniforms = std::make_unique< UniformsStd >();
			break;

		default:
			l_pUniforms = std::make_unique< UniformsBuf >();
			break;
		}

		return l_pUniforms;
	}

	std::unique_ptr< InOutsBase > InOutsBase::Get( DxRenderSystem const & p_renderSystem )
	{
		std::unique_ptr< InOutsBase > l_pInputs;

		switch ( p_renderSystem.GetFeatureLevel() )
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			l_pInputs = std::make_unique< InOutsOld >();
			break;

		default:
			l_pInputs = std::make_unique< InOutsNew >();
			break;
		}

		return l_pInputs;
	}

	DxShaderProgram::DxShaderProgram( DxRenderSystem * p_pRenderSystem )
		: ShaderProgramBase( p_pRenderSystem, eSHADER_LANGUAGE_HLSL )
		, m_pDxRenderSystem( p_pRenderSystem )
	{
		CreateObject( eSHADER_TYPE_VERTEX )->SetEntryPoint( cuT( "mainVx" ) );
		CreateObject( eSHADER_TYPE_PIXEL )->SetEntryPoint( cuT( "mainPx" ) );
	}

	DxShaderProgram::~DxShaderProgram()
	{
		Cleanup();
	}

	void DxShaderProgram::RetrieveLinkerLog( String & strLog )
	{
		if ( !m_renderSystem->UseShaders() )
		{
			strLog = DirectX11::GetHlslErrorString( 0 );
		}
	}

	void DxShaderProgram::Initialise()
	{
		ShaderProgramBase::Initialise();
	}

	void DxShaderProgram::Cleanup()
	{
		ShaderProgramBase::Cleanup();
	}

	void DxShaderProgram::Bind( uint8_t p_byIndex, uint8_t p_byCount )
	{
		ShaderProgramBase::Bind( p_byIndex, p_byCount );
	}

	void DxShaderProgram::Unbind()
	{
		ShaderProgramBase::Unbind();
	}

	ID3DBlob * DxShaderProgram::GetCompiled( Castor3D::eSHADER_TYPE p_eType )
	{
		ID3DBlob * l_pReturn = NULL;
		ShaderObjectBaseSPtr l_pObject = m_pShaders[p_eType];

		if ( l_pObject && l_pObject->GetStatus() == eSHADER_STATUS_COMPILED )
		{
			l_pReturn = std::static_pointer_cast< DxShaderObject >( l_pObject )->GetCompiled();
		}

		return l_pReturn;
	}

	ShaderObjectBaseSPtr DxShaderProgram::DoCreateObject( eSHADER_TYPE p_eType )
	{
		ShaderObjectBaseSPtr l_pReturn;

		switch ( p_eType )
		{
		case eSHADER_TYPE_VERTEX:
			l_pReturn = std::make_shared< DxVertexShader >( this );
			break;

		case eSHADER_TYPE_PIXEL:
			l_pReturn = std::make_shared< DxFragmentShader >( this );
			break;

		case eSHADER_TYPE_GEOMETRY:
			l_pReturn = std::make_shared< DxGeometryShader >( this );
			break;

		case eSHADER_TYPE_HULL:
			l_pReturn = std::make_shared< DxHullShader >( this );
			break;

		case eSHADER_TYPE_DOMAIN:
			l_pReturn = std::make_shared< DxDomainShader >( this );
			break;
		}

		return l_pReturn;
	}

	std::shared_ptr< OneTextureFrameVariable > DxShaderProgram::DoCreateTextureVariable( int p_iNbOcc )
	{
		return std::make_shared< DxOneFrameVariable< TextureBaseRPtr > >( static_cast< DxRenderSystem * >( m_renderSystem ), this, p_iNbOcc );
	}
}

#include "Dx11OverlayRenderer.hpp"
#include "Dx11RenderSystem.hpp"
#include "Dx11ShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

//*************************************************************************************************

namespace
{
	static String OverlayVS =
		cuT( "struct VtxInput\n" )
		cuT( "{\n" )
		cuT( "	uint4 Position: POSITION;\n" )
		cuT( "	float2 TextureUV: TEXCOORD0;\n" )
		cuT( "};\n" )
		cuT( "struct VtxOutput\n" )
		cuT( "{\n" )
		cuT( "	float4 Position: SV_POSITION;\n" )
		cuT( "	float2 TextureUV: TEXCOORD0;\n" )
		cuT( "};\n" )
		cuT( "VtxOutput mainVx( in VtxInput p_input )\n" )
		cuT( "{\n" )
		cuT( "	VtxOutput l_output;\n" )
		cuT( "	p_input.Position.w = 1.0f;\n" )
		cuT( "	l_output.Position = mul( p_input.Position, c3d_mtxProjectionModelView );\n" )
		cuT( "	l_output.TextureUV = p_input.TextureUV;\n" )
		cuT( "	return l_output;\n" )
		cuT( "}\n" );

	static String PanelPSDecl =
		cuT( "struct PxlInput\n" )
		cuT( "{\n" )
		cuT( "	float4 Position: SV_POSITION;\n" )
		cuT( "	float2 TextureUV: TEXCOORD0;\n" )
		cuT( "};\n" );

	static String PanelPSMain =
		cuT( "float4 mainPx( in PxlInput p_input ) : SV_Target\n" )
		cuT( "{\n" )
		cuT( "	float4 l_v4Ambient = c3d_v4MatAmbient;\n" )
		cuT( "	float l_fAlpha = c3d_fMatOpacity;\n" );

	static String PanelPSEnd =
		cuT( "	return float4( l_v4Ambient.xyz, l_fAlpha );\n" )
		cuT( "}\n" );
}

//*************************************************************************************************

DxOverlayRenderer::DxOverlayRenderer( DxRenderSystem * p_pRenderSystem )
	: OverlayRenderer( p_pRenderSystem )
{
}

DxOverlayRenderer::~DxOverlayRenderer()
{
}

ShaderProgramBaseSPtr DxOverlayRenderer::DoGetProgram( uint32_t p_uiFlags )
{
	std::unique_ptr< UniformsBase > l_pUniforms = UniformsBase::Get( static_cast< const DxRenderSystem & >( *m_pRenderSystem ) );

	// Vertex shader
	String l_strVs;
	l_strVs += l_pUniforms->GetVertexInMatrices();
	l_strVs += OverlayVS;

	// Pixel shader
	String l_strPs, l_strPsMain;
	l_strPs	+= PanelPSDecl;
	l_strPs += l_pUniforms->GetPixelInMatrices();
	l_strPs += l_pUniforms->GetPixelPass();
	l_strPsMain += PanelPSMain;

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_TEXT ) == eTEXTURE_CHANNEL_TEXT )
	{
		l_strPs += cuT( "Texture2D testTexture: register( t0 );\n" );
		l_strPs += cuT( "SamplerState textSampler: register( s0 );\n" );
		l_strPsMain	+= cuT( "	l_fAlpha *= testTexture.Sample( textSampler, p_input.TextureUV ).r;\n" );
	}

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
	{
		l_strPs += cuT( "Texture2D colourTexture: register( t1 );\n" );
		l_strPs += cuT( "SamplerState colourSampler: register( s1 );\n" );
		l_strPsMain	+= cuT( "	l_v4Ambient = colourTexture.Sample( colourSampler, p_input.TextureUV );\n" );
	}

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
	{
		l_strPs += cuT( "Texture2D opacityTexture: register( t2 );\n" );
		l_strPs += cuT( "SamplerState opacitySampler: register( s2 );\n" );
		l_strPsMain	+= cuT( "	l_fAlpha *= opacityTexture.Sample( opacitySampler, p_input.TextureUV ).r;\n" );
	}

	l_strPs += l_strPsMain;
	l_strPs += PanelPSEnd;

	// Shader program
	ShaderProgramBaseSPtr l_pProgram = m_pRenderSystem->GetEngine()->GetShaderManager().GetNewProgram();

	for ( int i = 0; i < eSHADER_MODEL_COUNT; ++i )
	{
		l_pProgram->SetSource( eSHADER_TYPE_VERTEX,	eSHADER_MODEL( i ), l_strVs );
		l_pProgram->SetSource( eSHADER_TYPE_PIXEL,	eSHADER_MODEL( i ), l_strPs );
	}

	return l_pProgram;
}

void DxOverlayRenderer::DoInitialise()
{
}

void DxOverlayRenderer::DoCleanup()
{
}

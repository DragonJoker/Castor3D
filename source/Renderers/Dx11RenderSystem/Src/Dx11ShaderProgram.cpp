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
using namespace Dx11Render;

class UniformsBase
{
protected:
	String m_strVertexInMatrices;
	String m_strPixelInMatrices;
	String m_strPixelPass;
	String m_strPixelScene;

public:
	inline String const & GetVertexInMatrices()const
	{
		return m_strVertexInMatrices;
	}
	inline String const & GetPixelInMatrices()const
	{
		return m_strPixelInMatrices;
	}
	inline String const & GetPixelPass()const
	{
		return m_strPixelPass;
	}
	inline String const & GetPixelScene()const
	{
		return m_strPixelScene;
	}
};

class UniformsBuf : public UniformsBase
{
public:
	UniformsBuf()
	{
		m_strVertexInMatrices =
			cuT( "cbuffer Matrices : register( cb0 )\n"	)
			cuT( "{\n"	)
			cuT( "	matrix	c3d_mtxProjection;\n"	)
			cuT( "	matrix	c3d_mtxModel;\n"	)
			cuT( "	matrix	c3d_mtxView;\n"	)
			cuT( "	matrix	c3d_mtxModelView;\n"	)
			cuT( "	matrix	c3d_mtxProjectionModelView;\n"	)
			cuT( "	matrix	c3d_mtxNormal;\n"	)
			cuT( "	matrix	c3d_mtxTexture0;\n"	)
			cuT( "	matrix	c3d_mtxTexture1;\n"	)
			cuT( "	matrix	c3d_mtxTexture2;\n"	)
			cuT( "	matrix	c3d_mtxTexture3;\n"	)
			cuT( "};\n"	);
		m_strPixelInMatrices =
			cuT( "cbuffer Matrices : register( cb0 )\n"	)
			cuT( "{\n"	)
			cuT( "	matrix	c3d_mtxProjection;\n"	)
			cuT( "	matrix	c3d_mtxModel;\n"	)
			cuT( "	matrix	c3d_mtxView;\n"	)
			cuT( "	matrix	c3d_mtxModelView;\n"	)
			cuT( "	matrix	c3d_mtxProjectionModelView;\n"	)
			cuT( "	matrix	c3d_mtxNormal;\n"	)
			cuT( "	matrix	c3d_mtxTexture0;\n"	)
			cuT( "	matrix	c3d_mtxTexture1;\n"	)
			cuT( "	matrix	c3d_mtxTexture2;\n"	)
			cuT( "	matrix	c3d_mtxTexture3;\n"	)
			cuT( "};\n"	);
		m_strPixelScene =
			cuT( "cbuffer Scene : register( cb1 )\n"	)
			cuT( "{\n"	)
			cuT( "	int		c3d_iLightsCount;\n"	)
			cuT( "	float4	c3d_v4AmbientLight;\n"	)
			cuT( "	float4	c3d_v4BackgroundColour;\n"	)
			cuT( "	float3	c3d_v3CameraPosition;\n"	)
			cuT( "};\n"	);
		m_strPixelPass =
			cuT( "cbuffer PassBuffer : register( cb3 )\n"	)
			cuT( "{\n"	)
			cuT( "	float4	c3d_v4MatAmbient;\n"	)
			cuT( "	float4	c3d_v4MatDiffuse;\n"	)
			cuT( "	float4	c3d_v4MatEmissive;\n"	)
			cuT( "	float4	c3d_v4MatSpecular;\n"	)
			cuT( "	float	c3d_fMatShininess;\n"	)
			cuT( "	float	c3d_fMatOpacity;\n"	)
			cuT( "};\n"	);
	}
};

class UniformsStd : public UniformsBase
{
public:
	UniformsStd()
	{
		m_strVertexInMatrices =
			cuT( "uniform	float4x4	c3d_mtxProjection;\n"	)
			cuT( "uniform	float4x4	c3d_mtxModel;\n"	)
			cuT( "uniform	float4x4	c3d_mtxView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxModelView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxProjectionModelView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxNormal;\n"	)
			cuT( "uniform	float4x4	c3d_mtxTexture0;\n"	)
			cuT( "uniform	float4x4	c3d_mtxTexture1;\n"	)
			cuT( "uniform	float4x4	c3d_mtxTexture2;\n"	)
			cuT( "uniform	float4x4	c3d_mtxTexture3;\n"	);
		m_strPixelInMatrices =
			cuT( "uniform	float4x4	c3d_mtxProjection;\n"	)
			cuT( "uniform	float4x4	c3d_mtxModel;\n"	)
			cuT( "uniform	float4x4	c3d_mtxView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxModelView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxProjectionModelView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxNormal;\n"	)
			cuT( "uniform	float4x4	c3d_mtxTexture0;\n"	)
			cuT( "uniform	float4x4	c3d_mtxTexture1;\n"	)
			cuT( "uniform	float4x4	c3d_mtxTexture2;\n"	)
			cuT( "uniform	float4x4	c3d_mtxTexture3;\n"	);
		m_strPixelScene =
			cuT( "uniform	int 		c3d_iLightsCount;\n"	)
			cuT( "uniform	float4 		c3d_v4AmbientLight;\n"	)
			cuT( "uniform	float4 		c3d_v4BackgroundColour;\n"	)
			cuT( "uniform	float3 		c3d_v3CameraPosition;\n"	);
		m_strPixelPass =
			cuT( "uniform	float4 		c3d_v4MatAmbient;\n"	)
			cuT( "uniform	float4 		c3d_v4MatDiffuse;\n"	)
			cuT( "uniform	float4 		c3d_v4MatEmissive;\n"	)
			cuT( "uniform	float4 		c3d_v4MatSpecular;\n"	)
			cuT( "uniform	float 		c3d_fMatShininess;\n"	)
			cuT( "uniform	float		c3d_fMatOpacity;\n"	);
	}
};

class InOutsBase
{
protected:
	String	m_strVtxInput;
	String	m_strVtxOutput;
	String	m_strPxlInput;
	String	m_strPxlOutput;

public:
	String const & GetVtxInput()const
	{
		return m_strVtxInput;
	}
	String const & GetVtxOutput()const
	{
		return m_strVtxOutput;
	}
	String const & GetPxlInput()const
	{
		return m_strPxlInput;
	}
	String const & GetPxlOutput()const
	{
		return m_strPxlOutput;
	}
};

class InOutsOld : public InOutsBase
{
public:
	InOutsOld()
	{
		m_strVtxInput =
			cuT( "struct VtxInput\n"	)
			cuT( "{\n"	)
			cuT( "	float4		Position	:	POSITION0;\n"	)
			cuT( "	float3		Normal		:	NORMAL0;\n"	)
			cuT( "	float3		Tangent		:	TANGENT0;\n"	)
			cuT( "	float3		Binormal	:	BINORMAL0;\n"	)
			cuT( "	float3		TextureUV	:	TEXCOORD0;\n"	)
// 			cuT( "	int4		BoneIDs		:	BLENDINDICES;\n")
// 			cuT( "	float4		Weights		:	BLENDWEIGHT;\n"	)
// 			cuT( "	float4x4	Matrix		:	MATRIX;\n"		)
			cuT( "};\n"	);
		m_strVtxOutput =
			cuT( "struct VtxOutput\n"	)
			cuT( "{\n"	)
			cuT( "	float4		Position	:	POSITION0;\n"	)
			cuT( "	float3		Vertex		:	POSITION1;\n"	)
			cuT( "	float3		Normal		:	NORMAL0;\n"	)
			cuT( "	float3		Tangent		:	TANGENT0;\n"	)
			cuT( "	float3		Binormal	:	BINORMAL0;\n"	)
			cuT( "	float3		TextureUV	:	TEXCOORD0;\n"	)
			cuT( "	float3		Eye			:	POSITION2;\n"	)
			cuT( "};\n"	);
		m_strPxlInput =
			cuT( "struct PxInput\n"	)
			cuT( "{\n"	)
			cuT( "	float4		Position	:	POSITION0;\n"	)
			cuT( "	float3		Vertex		:	POSITION1;\n"	)
			cuT( "	float3		Normal		:	NORMAL0;\n"	)
			cuT( "	float3		Tangent		:	TANGENT0;\n"	)
			cuT( "	float3		Binormal	:	BINORMAL0;\n"	)
			cuT( "	float3		TextureUV	:	TEXCOORD0;\n"	)
			cuT( "	float3		Eye			:	POSITION2;\n"	)
			cuT( "};\n"	);
		m_strPxlOutput = cuT( "COLOR0" );
	}
};

class InOutsNew : public InOutsBase
{
public:
	InOutsNew()
	{
		m_strVtxInput =
			cuT( "struct VtxInput\n"	)
			cuT( "{\n"	)
			cuT( "	float4		Position	:	POSITION0;\n"	)
			cuT( "	float3		Normal		:	NORMAL0;\n"	)
			cuT( "	float3		Tangent		:	TANGENT0;\n"	)
			cuT( "	float3		Binormal	:	BINORMAL0;\n"	)
			cuT( "	float3		TextureUV	:	TEXCOORD0;\n"	)
// 			cuT( "	int4		BoneIDs		:	BLENDINDICES;\n")
// 			cuT( "	float4		Weights		:	BLENDWEIGHT;\n"	)
// 			cuT( "	float4x4	Matrix		:	MATRIX;\n"		)
			cuT( "};\n"	);
		m_strVtxOutput =
			cuT( "struct VtxOutput\n"	)
			cuT( "{\n"	)
			cuT( "	float4		Position	:	SV_POSITION;\n"	)
			cuT( "	float3		Vertex		:	POSITION0;\n"	)
			cuT( "	float3		Normal		:	NORMAL0;\n"	)
			cuT( "	float3		Tangent		:	TANGENT0;\n"	)
			cuT( "	float3		Binormal	:	BINORMAL0;\n"	)
			cuT( "	float3		TextureUV	:	TEXCOORD0;\n"	)
			cuT( "	float3		Eye			:	POSITION1;\n"	)
			cuT( "};\n"	);
		m_strPxlInput =
			cuT( "struct PxInput\n"	)
			cuT( "{\n"	)
			cuT( "	float4		Position	:	SV_POSITION;\n"	)
			cuT( "	float3		Vertex		:	POSITION0;\n"	)
			cuT( "	float3		Normal		:	NORMAL0;\n"	)
			cuT( "	float3		Tangent		:	TANGENT0;\n"	)
			cuT( "	float3		Binormal	:	BINORMAL0;\n"	)
			cuT( "	float3		TextureUV	:	TEXCOORD0;\n"	)
			cuT( "	float3		Eye			:	POSITION1;\n"	)
			cuT( "};\n"	);
		m_strPxlOutput = cuT( "SV_TARGET" );
	}
};

class ShaderModelBase
{
protected:
};

class PixelShaderSource
{
private:
	String m_strDeclarations				;
	String m_strLights						;
	String m_strMainDeclarations			;
	String m_strMainLightsLoop				;
	String m_strMainLightsLoopAfterLightDir	;
	String m_strMainLightsLoopEnd			;
	String m_strMainEnd						;
	String m_strOutput						;

public:
	PixelShaderSource()
	{
		m_strDeclarations =
			cuT( "Texture2D	c3d_sLights : register( t0 );\n"	)
			cuT( "SamplerState LightsSampler : register( s0 );\n"	)
			cuT( "SamplerState DefaultSampler\n"	)
			cuT( "{\n"	)
			cuT( "	AddressU  = WRAP;\n"	)
			cuT( "	AddressV  = WRAP;\n"	)
			cuT( "	AddressW  = WRAP;\n"	)
			cuT( "	MipFilter = NONE;\n"	)
			cuT( "	MinFilter = LINEAR;\n"	)
			cuT( "	MagFilter = LINEAR;\n"	)
			cuT( "};\n"	);
		m_strLights =
			cuT( "struct Light\n"	)
			cuT( "{\n"	)
			cuT( "	float4		m_v4Ambient;\n"	)
			cuT( "	float4		m_v4Diffuse;\n"	)
			cuT( "	float4		m_v4Specular;\n"	)
			cuT( "	float4		m_v4Position;\n"	)
			cuT( "	int			m_iType;\n"	)
			cuT( "	float3		m_v3Attenuation;\n"	)
			cuT( "	float4x4	m_mtx4Orientation;\n"	)
			cuT( "	float		m_fExponent;\n"	)
			cuT( "	float		m_fCutOff;\n"	)
			cuT( "};\n"	)
			cuT( "Light GetLight( int i )\n"	)
			cuT( "{\n"	)
			cuT( "	Light l_lightReturn;\n"	)
			cuT( "	l_lightReturn.m_v4Ambient		= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (0 * 0.001) + 0.0005, 0.0 ) );\n"	)
			cuT( "	l_lightReturn.m_v4Diffuse		= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (1 * 0.001) + 0.0005, 0.0 ) );\n"	)
			cuT( "	l_lightReturn.m_v4Specular		= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (2 * 0.001) + 0.0005, 0.0 ) );\n"	)
			cuT( "	l_lightReturn.m_v4Position		= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (3 * 0.001) + 0.0005, 0.0 ) );\n"	)
			cuT( "	l_lightReturn.m_v3Attenuation	= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (4 * 0.001) + 0.0005, 0.0 ) ).xyz;\n"	)
			cuT( "	float4	l_v4A					= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (5 * 0.001) + 0.0005, 0.0 ) );\n"	)
			cuT( "	float4	l_v4B					= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (6 * 0.001) + 0.0005, 0.0 ) );\n"	)
			cuT( "	float4	l_v4C					= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (7 * 0.001) + 0.0005, 0.0 ) );\n"	)
			cuT( "	float4	l_v4D					= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (8 * 0.001) + 0.0005, 0.0 ) );\n"	)
			cuT( "	float2	l_v2Spot				= c3d_sLights.Sample( LightsSampler, float2( (i * 0.01) + (9 * 0.001) + 0.0005, 0.0 ) ).xy;\n"	)
			cuT( "	l_lightReturn.m_iType			= int( l_lightReturn.m_v4Position.w );\n"	)
			cuT( "	l_lightReturn.m_mtx4Orientation	= float4x4( l_v4A, l_v4B, l_v4C, l_v4D );\n"	)
			cuT( "	l_lightReturn.m_fExponent		= l_v2Spot.x;\n"	)
			cuT( "	l_lightReturn.m_fCutOff			= l_v2Spot.x;\n"	)
			cuT( "	return l_lightReturn;\n"	)
			cuT( "}\n"	);
		m_strMainDeclarations =
			cuT( "float4 mainPx( in PxInput p_input ) : [PxlOutput]\n"	)
			cuT( "{\n"	)
			cuT( "	float4	l_v4Return	= float4( 0, 0, 0, 0 );\n"	)
			cuT( "	Light	l_light;\n"	)
			cuT( "	float3	l_v3LightDir;\n"	)
			cuT( "	float	l_fLambert;\n"	)
			cuT( "	float	l_fSpecular;\n"	)
			cuT( "	float3	l_v3Normal			= p_input.Normal;\n"	)
			cuT( "	float4	l_v4Ambient			= float4( 0, 0, 0, 0 );\n"	)
			cuT( "	float4	l_v4Diffuse			= float4( 0, 0, 0, 0 );\n"	)
			cuT( "	float4	l_v4Specular		= float4( 0, 0, 0, 0 );\n"	)
			cuT( "	float4	l_v4TmpAmbient		= float4( 0, 0, 0, 0 );\n"	)
			cuT( "	float4	l_v4TmpDiffuse		= float4( 0, 0, 0, 0 );\n"	)
			cuT( "	float4	l_v4TmpSpecular		= float4( 0, 0, 0, 0 );\n"	)
			cuT( "	float3	l_v3EyeVec			= normalize( p_input.Eye );\n"	)
			cuT( "	float	l_fAlpha			= c3d_fMatOpacity;\n"	);
		m_strMainLightsLoop =
			cuT( "	for( int i = 0; i < c3d_iLightsCount; i++ )\n"	)
			cuT( "	{\n"	)
			cuT( "		l_light			= GetLight( i );\n"	)
			cuT( "		l_v3LightDir	= normalize( mul( l_light.m_v4Position, c3d_mtxModelView ).xyz - p_input.Vertex );\n"	);
		m_strMainLightsLoopAfterLightDir =
			cuT( "		l_fLambert		= max( dot( l_v3Normal, l_v3LightDir ), 0.0 );\n"	)
			cuT( "		l_fSpecular		= pow( clamp( dot( reflect( -l_v3LightDir, l_v3Normal ), l_v3EyeVec ), 0.0, 1.0 ), c3d_fMatShininess );\n"	)
			cuT( "		l_v4TmpAmbient	= l_light.m_v4Ambient	* c3d_v4MatAmbient;\n"	)
			cuT( "		l_v4TmpDiffuse	= l_light.m_v4Diffuse	* c3d_v4MatDiffuse 	* l_fLambert;\n"	)
			cuT( "		l_v4TmpSpecular	= l_light.m_v4Specular	* c3d_v4MatSpecular	* l_fSpecular;\n"	);
		m_strMainLightsLoopEnd =
			cuT( "		l_v4Ambient		+= l_v4TmpAmbient;\n"	)
			cuT( "		l_v4Diffuse		+= l_v4TmpDiffuse;\n"	)
			cuT( "		l_v4Specular	+= l_v4TmpSpecular;\n"	)
			cuT( "	}\n"	);
		m_strMainEnd =
			cuT( "	return l_v4Return;\n"	)
			cuT( "}\n"	);
		m_strOutput =
			cuT( "l_v4Return"	);
	}

public:
	virtual String const & GetDeclarations()
	{
		return m_strDeclarations;
	}
	virtual String const & GetLights()
	{
		return m_strLights;
	}
	virtual String const & GetMainDeclarations()
	{
		return m_strMainDeclarations;
	}
	virtual String const & GetMainLightsLoop()
	{
		return m_strMainLightsLoop;
	}
	virtual String const & GetMainLightsLoopAfterLightDir()
	{
		return m_strMainLightsLoopAfterLightDir;
	}
	virtual String const & GetMainLightsLoopEnd()
	{
		return m_strMainLightsLoopEnd;
	}
	virtual String const & GetMainEnd()
	{
		return m_strMainEnd;
	}
	virtual String const & GetOutput()
	{
		return m_strOutput;
	}
};

class DeferredShaderSource
{
protected:
	String m_strGSVertexProgram						;
	String m_strGSPixelDeclarations					;
	String m_strGSPixelMainDeclarations				;
	String m_strGSPixelMainLightsLoopAfterLightDir	;
	String m_strGSPixelMainEnd						;
	String m_strLSVertexProgram						;
	String m_strLSPixelProgram						;

public:
	DeferredShaderSource()
	{
		m_strGSVertexProgram =
			cuT( "uniform	float4x4	c3d_mtxProjectionModelView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxModelView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxView;\n"	)
			cuT( "uniform	float4x4	c3d_mtxNormal;\n"	)
			cuT( "struct VtxInput\n"	)
			cuT( "{\n"	)
			cuT( "	float4	Position	:	POSITION0;\n"	)
			cuT( "	float3	Normal		:	NORMAL0;\n"	)
			cuT( "	float3	Tangent		:	TANGENT0;\n"	)
			cuT( "	float3	Bitangent	:	TANGENT1;\n"	)
			cuT( "	float3	TextureUV	:	TEXCOORD0;\n"	)
			cuT( "};\n"	)
			cuT( "struct VtxOutput\n"	)
			cuT( "{\n"	)
			cuT( "	float4	Position	:	POSITION0;\n"	)
			cuT( "	float3	Vertex		:	POSITION1;\n"	)
			cuT( "	float3	Normal		:	NORMAL0;\n"	)
			cuT( "	float3	Tangent		:	TANGENT0;\n"	)
			cuT( "	float3	Bitangent	:	TANGENT1;\n"	)
			cuT( "	float3	TextureUV	:	TEXCOORD0;\n"	)
			cuT( "};\n"	)
			cuT( "VtxOutput mainVx( VtxInput p_input )\n"	)
			cuT( "{\n"	)
			cuT( "	VtxOutput l_output;\n"	)
			cuT( "	p_input.Position.w = 1.0f;\n"	)
			cuT( "	l_output.Position	= mul( p_input.Position, c3d_mtxProjectionModelView );\n"	)
			cuT( "	l_output.Vertex		= l_output.Position.xyz;\n"	)
			cuT( "	l_output.Normal		= normalize( mul( float4( p_input.Normal, 1.0 ),	c3d_mtxNormal ) ).xyz;\n"	)
			cuT( "	l_output.Tangent	= normalize( mul( float4( p_input.Tangent, 1.0 ),	c3d_mtxNormal ) ).xyz;\n"	)
			cuT( "	l_output.Bitangent	= normalize( mul( float4( p_input.Bitangent, 1.0 ),	c3d_mtxNormal ) ).xyz;\n"	)
			cuT( "	l_output.TextureUV 	= p_input.TextureUV;\n"	)
			cuT( "	return l_output;\n"	)
			cuT( "}\n"	);
		m_strGSPixelDeclarations =
			cuT( "uniform	float4		c3d_v4AmbientLight;\n"	)
			cuT( "uniform	float4		c3d_v4MatAmbient;\n"	)
			cuT( "uniform	float4		c3d_v4MatEmissive;\n"	)
			cuT( "uniform	float4		c3d_v4MatDiffuse;\n"	)
			cuT( "uniform	float4		c3d_v4MatSpecular;\n"	)
			cuT( "uniform	float		c3d_fMatShininess;\n"	)
			cuT( "uniform	float		c3d_fMatOpacity;\n"	)
			cuT( "float pack2Floats(float f1, float f2)\n"	)
			cuT( "{\n"	)
			cuT( "	return (f2 + clamp(f1, 0.f, 0.999f)) * 10.f; // * 10.f, because I would loose the coef sometimes. pow = 128 and coef = 0.1 were bad for example\n"	)
			cuT( "}\n"	)
			cuT( "struct PxlOutput\n"	)
			cuT( "{\n"	)
			cuT( "	float4	Position		:	COLOR0;\n"	)
			cuT( "	float4	DiffSpecular	:	COLOR1;\n"	)
			cuT( "	float4	Normals			:	COLOR2;\n"	)
			cuT( "	float4	TanBitangent	:	COLOR3;\n"	)
			cuT( "};\n"	);
		m_strGSPixelMainDeclarations =
			cuT( "PxlOutput mainPx( in VtxOutput p_input )\n"	)
			cuT( "{\n"	)
			cuT( "	PxlOutput l_output;\n"	)
			cuT( "	float4	l_v4Position		= float4( p_input.Vertex, 1 );\n"	)
			cuT( "	float4	l_v4Normal			= float4( p_input.Normal, 1 );\n"	)
			cuT( "	float4	l_v4Tangent			= float4( p_input.Tangent, 1 );\n"	)
			cuT( "	float4	l_v4Bitangent		= float4( p_input.Bitangent, 1 );\n"	)
			cuT( "	float4	l_v4Diffuse			= float4( 0, 0, 0, 0 );\n"	)
			cuT( "	float4	l_v4Texture			= float4( p_input.TextureUV, 1 );\n"	)
			cuT( "	float4	l_v4Specular		= float4( c3d_v4MatSpecular.xyz, c3d_fMatShininess );\n"	)
			cuT( "	float	l_fAlpha			= c3d_fMatOpacity;\n"	);
		m_strGSPixelMainLightsLoopAfterLightDir =
			cuT( "	l_v4Diffuse		= c3d_v4MatDiffuse;\n"	);
		m_strGSPixelMainEnd =
			cuT( "	return l_output;\n"	)
			cuT( "}\n"	);
		m_strLSVertexProgram =
			cuT( "uniform	float4x4	c3d_mtxProjectionModelView;\n"	)
			cuT( "struct VtxInput\n"	)
			cuT( "{\n"	)
			cuT( "	float4	Position	:	POSITION0;\n"	)
			cuT( "	float2	TextureUV	:	TEXCOORD0;\n"	)
			cuT( "};\n"	)
			cuT( "struct VtxOutput\n"	)
			cuT( "{\n"	)
			cuT( "	float4	Position	:	POSITION0;\n"	)
			cuT( "	float2	TextureUV	:	TEXCOORD0;\n"	)
			cuT( "};\n"	)
			cuT( "VtxOutput mainVx( in VtxInput p_input )\n"	)
			cuT( "{\n"	)
			cuT( "	VtxOutput l_out;\n"	)
			cuT( "	p_input.Position.w = 1.0f;\n"	)
			cuT( "	l_out.Position = mul( p_input.Position, c3d_mtxProjectionModelView );\n"	)
			cuT( "	l_out.TextureUV = p_input.TextureUV;\n"	)
			cuT( "	return l_out;\n"	)
			cuT( "}\n"	);
		m_strLSPixelProgram =
			cuT( "uniform	int 		c3d_iLightsCount;\n"	)
			cuT( "uniform	float3		c3d_v3CameraPosition;\n"	)
			cuT( "Texture2D				c3d_sLights;\n"	)
			cuT( "SamplerState LightsSampler\n"	)
			cuT( "{\n"	)
			cuT( "	MagFilter = POINT;\n"	)
			cuT( "	MinFilter = POINT;\n"	)
			cuT( "	MipFilter = POINT;\n"	)
			cuT( "};\n"	)
			cuT( "SamplerState DefaultSampler\n"	)
			cuT( "{\n"	)
			cuT( "	AddressU  = WRAP;\n"	)
			cuT( "	AddressV  = WRAP;\n"	)
			cuT( "	AddressW  = WRAP;\n"	)
			cuT( "	MipFilter = NONE;\n"	)
			cuT( "	MinFilter = LINEAR;\n"	)
			cuT( "	MagFilter = LINEAR;\n"	)
			cuT( "};\n"	)
			cuT( "Texture2D				c3d_mapPosition;\n"	)
			cuT( "Texture2D				c3d_mapDiffuse;\n"	)
			cuT( "Texture2D				c3d_mapNormals;\n"	)
			cuT( "Texture2D				c3d_mapTangent;\n"	)
			cuT( "Texture2D				c3d_mapBitangent;\n"	)
			cuT( "Texture2D				c3d_mapSpecular;\n"	)
			cuT( "struct Light\n"	)
			cuT( "{\n"	)
			cuT( "	float4		m_v4Ambient;\n"	)
			cuT( "	float4		m_v4Diffuse;\n"	)
			cuT( "	float4		m_v4Specular;\n"	)
			cuT( "	float4		m_v4Position;\n"	)
			cuT( "	int			m_iType;\n"	)
			cuT( "	float3		m_v3Attenuation;\n"	)
			cuT( "	float4x4	m_mtx4Orientation;\n"	)
			cuT( "	float		m_fExponent;\n"	)
			cuT( "	float		m_fCutOff;\n"	)
			cuT( "};\n"	)
			cuT( "Light GetLight( int i )\n"	)
			cuT( "{\n"	)
			cuT( "	Light l_lightReturn;\n"	)
			cuT( "	l_lightReturn.m_v4Ambient		= tex2D( LightsSampler, float2( (i * 0.1) + (0 * 0.01) + 0.005, 0.0 ) );\n"	)
			cuT( "	l_lightReturn.m_v4Diffuse		= tex2D( LightsSampler, float2( (i * 0.1) + (1 * 0.01) + 0.005, 0.0 ) );\n"	)
			cuT( "	l_lightReturn.m_v4Specular		= tex2D( LightsSampler, float2( (i * 0.1) + (2 * 0.01) + 0.005, 0.0 ) );\n"	)
			cuT( "	l_lightReturn.m_v4Position		= tex2D( LightsSampler, float2( (i * 0.1) + (3 * 0.01) + 0.005, 0.0 ) );\n"	)
			cuT( "	l_lightReturn.m_v3Attenuation	= tex2D( LightsSampler, float2( (i * 0.1) + (4 * 0.01) + 0.005, 0.0 ) ).xyz;\n"	)
			cuT( "	float4	l_v4A					= tex2D( LightsSampler, float2( (i * 0.1) + (5 * 0.01) + 0.005, 0.0 ) );\n"	)
			cuT( "	float4	l_v4B					= tex2D( LightsSampler, float2( (i * 0.1) + (6 * 0.01) + 0.005, 0.0 ) );\n"	)
			cuT( "	float4	l_v4C					= tex2D( LightsSampler, float2( (i * 0.1) + (7 * 0.01) + 0.005, 0.0 ) );\n"	)
			cuT( "	float4	l_v4D					= tex2D( LightsSampler, float2( (i * 0.1) + (8 * 0.01) + 0.005, 0.0 ) );\n"	)
			cuT( "	float2	l_v2Spot				= tex2D( LightsSampler, float2( (i * 0.1) + (9 * 0.01) + 0.005, 0.0 ) ).xy;\n"	)
			cuT( "	l_lightReturn.m_iType			= int( l_lightReturn.m_v4Position.w );\n"	)
			cuT( "	l_lightReturn.m_mtx4Orientation	= float4x4( l_v4A, l_v4B, l_v4C, l_v4D );\n"	)
			cuT( "	l_lightReturn.m_fExponent		= l_v2Spot.x;\n"	)
			cuT( "	l_lightReturn.m_fCutOff			= l_v2Spot.x;\n"	)
			cuT( "	return l_lightReturn;\n"	)
			cuT( "}\n"	)
			cuT( "float2 unpack2Floats(float fBoth)\n"	)
			cuT( "{\n"	)
			cuT( "	fBoth *= 0.1f;\n"	)
			cuT( "	float f1 = frac(fBoth);\n"	)
			cuT( "	float f2 = fBoth - f1;\n"	)
			cuT( "	return float2(f1, f2);\n"	)
			cuT( "}\n"	)
			cuT( "float4 mainPx( in VtxOutput p_input ) : COLOR0\n"	)
			cuT( "{\n"	)
			cuT( "	float4	l_output;\n"	)
			cuT( "	Light	l_light;\n"	)
			cuT( "	float4	l_v4Diffuses;\n"	)
			cuT( "	float4	l_v4Speculars;\n"	)
			cuT( "	float4	l_v4Tangents;\n"	)
			cuT( "	float4	l_v4Bitangents;\n"	)
			cuT( "	float4	l_v4Positions	= c3d_mapPosition.Sample( DefaultSampler,	p_input.TextureUV );\n"	)
			cuT( "	float4	l_v4Diffuses	= c3d_mapDiffuse.Sample( DefaultSampler,	p_input.TextureUV );\n"	)
			cuT( "	float4	l_v4Speculars	= c3d_mapSpecular.Sample( DefaultSampler,	p_input.TextureUV );\n"	)
			cuT( "	float4	l_v4Normals		= c3d_mapNormals.Sample( DefaultSampler,	p_input.TextureUV ) / 2.0;\n"	)
			cuT( "	float4	l_v4Tangents	= c3d_mapTangent.Sample( DefaultSampler,	p_input.TextureUV );\n"	)
			cuT( "	float4	l_v4Bitangents	= c3d_mapBitangent.Sample( DefaultSampler,	p_input.TextureUV );\n"	)
			cuT( "	float3	l_v3Binormal	= l_v4Bitangents.xyz;\n"	)
			cuT( "	float	l_fInvRadius	= 0.02;\n"	)
			cuT( "	float3	l_v3Specular	= float3( 0, 0, 0 );\n"	)
			cuT( "	float3	l_v3Diffuse		= float3( 0, 0, 0 );\n"	)
			cuT( "	float3	l_v3TmpVec		= -l_v4Positions.xyz;\n"	)
			cuT( "	float3	l_v3EyeVec;\n"	)
			cuT( "	float3	l_v3HalfVector;\n"	)
			cuT( "	float3	l_v3LightDir;\n"	)
			cuT( "	float	l_fLambert;\n"	)
			cuT( "	float	l_fSpecular;\n"	)
			cuT( "	float	l_fSqrLength;\n"	)
			cuT( "	float	l_fAttenuation;\n"	)
			cuT( "	float3	l_v3TmpDiffuse;\n"	)
			cuT( "	float3	l_v3TmpSpecular;\n"	)
			cuT( "	l_v3EyeVec.x = dot( l_v3TmpVec, l_v4Tangents.xyz	);\n"	)
			cuT( "	l_v3EyeVec.y = dot( l_v3TmpVec, l_v3Binormal		);\n"	)
			cuT( "	l_v3EyeVec.z = dot( l_v3TmpVec, l_v4Normals.xyz		);\n"	)
			cuT( "	l_v3EyeVec = normalize( l_v3EyeVec );\n"	)
			cuT( "	for( int i = 0; i < c3d_iLightsCount; i++ )\n"	)
			cuT( "	{\n"	)
			cuT( "		l_light			= GetLight( i );\n"	)
			cuT( "		l_v3LightDir	= normalize( l_light.m_v4Position.xyz - l_v4Positions.xyz );\n"	)
			cuT( "		l_v3LightDir 	= float3( dot( l_v3LightDir, l_v4Tangents.xyz ), dot( l_v3LightDir, l_v3Binormal	), dot( l_v3LightDir, l_v4Normals.xyz ) );\n"	)
			cuT( "		l_fSqrLength	= dot( l_v3LightDir, l_v3LightDir );\n"	)
			cuT( "		l_v3LightDir	= l_v3LightDir * rsqrt( l_fSqrLength );\n"	)
			cuT( "		l_fAttenuation	= clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n"	)
			cuT( "		l_fLambert		= max( dot( l_v4Normals.xyz, l_v3LightDir ), 0.0 );\n"	)
			cuT( "		l_fSpecular		= pow( clamp( dot( reflect( -l_v3LightDir, l_v4Normals.xyz ), l_v3EyeVec ), 0.0, 1.0 ), 10.0 );\n"	)
			cuT( "		l_v3TmpDiffuse	= l_light.m_v4Diffuse.xyz	* l_v4Diffuses.xyz	* l_fLambert;\n"	)
			cuT( "		l_v3TmpSpecular	= l_light.m_v4Specular.xyz	* l_v4Speculars.xyz	* l_fSpecular;\n"	)
			cuT( "		l_v3Diffuse		+= l_v3TmpDiffuse * l_fAttenuation;\n"	)
			cuT( "		l_v3Specular	+= l_v3TmpSpecular * l_fAttenuation;\n"	)
			cuT( "	}\n"	)
			cuT( "	l_output = float4( l_v3Diffuse + l_v3Specular, 1 );\n"	)
			cuT( "//	l_output = float4( l_v4Tangents );\n"	)
			cuT( "	return l_output;\n"	)
			cuT( "}\n"	);
	}
	virtual const String & GetGSVertexProgram()
	{
		return m_strGSVertexProgram;
	}
	virtual const String & GetGSPixelDeclarations()
	{
		return m_strGSPixelDeclarations;
	}
	virtual const String & GetGSPixelMainDeclarations()
	{
		return m_strGSPixelMainDeclarations;
	}
	virtual const String & GetGSPixelMainLightsLoopAfterLightDir()
	{
		return m_strGSPixelMainLightsLoopAfterLightDir;
	}
	virtual const String & GetGSPixelMainEnd()
	{
		return m_strGSPixelMainEnd;
	}
	virtual const String & GetLSVertexProgram()
	{
		return m_strLSVertexProgram;
	}
	virtual const String & GetLSPixelProgram()
	{
		return m_strLSPixelProgram;
	}
};

PixelShaderSource g_ps;
DeferredShaderSource g_ds;

DxShaderProgram::DxShaderProgram( DxRenderSystem * p_pRenderSystem )
	:	ShaderProgramBase( p_pRenderSystem, eSHADER_LANGUAGE_HLSL	)
	,	m_pDxRenderSystem( p_pRenderSystem	)
{
	CreateObject( eSHADER_TYPE_VERTEX	)->SetEntryPoint( cuT( "mainVx" ) );
	CreateObject( eSHADER_TYPE_PIXEL	)->SetEntryPoint( cuT( "mainPx" ) );
	m_wpDxMatrixBuffer		= std::static_pointer_cast< DxFrameVariableBuffer >( m_pMatrixBuffer	);
	m_wpDxSceneBuffer		= std::static_pointer_cast< DxFrameVariableBuffer >( m_pSceneBuffer	);
	m_wpDxPassBuffer		= std::static_pointer_cast< DxFrameVariableBuffer >( m_pPassBuffer	);
	m_wpDxConstantsBuffer	= std::static_pointer_cast< DxFrameVariableBuffer >( m_pUserBuffer	);
}

DxShaderProgram::~DxShaderProgram()
{
	m_wpDxMatrixBuffer.reset();
	m_wpDxSceneBuffer.reset();
	m_wpDxPassBuffer.reset();
	m_wpDxConstantsBuffer.reset();
	Cleanup();
}

void DxShaderProgram::RetrieveLinkerLog( String & strLog )
{
	if ( !m_pRenderSystem->UseShaders() )
	{
		strLog = DirectX11::GetHlslErrorString( 0 );
		return;
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

void DxShaderProgram::Begin( uint8_t p_byIndex, uint8_t p_byCount )
{
	m_pRenderSystem->GetPipeline()->UpdateFunctions( this );
	ShaderProgramBase::Begin( p_byIndex, p_byCount );
	m_wpDxMatrixBuffer.lock()->Bind( 0 );
	m_wpDxSceneBuffer.lock()->Bind( 1 );
	m_wpDxPassBuffer.lock()->Bind( 2 );
	m_wpDxConstantsBuffer.lock()->Bind( 3 );

	for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
	{
		if ( m_pShaders[i] )
		{
			for ( FrameVariablePtrStrMapIt l_it = m_pShaders[i]->GetFrameVariablesBegin(); l_it != m_pShaders[i]->GetFrameVariablesEnd(); ++l_it )
			{
				l_it->second.lock()->Apply();
			}
		}
	}
}

void DxShaderProgram::End()
{
	m_wpDxMatrixBuffer.lock()->Bind( 0 );
	m_wpDxSceneBuffer.lock()->Bind( 1 );
	m_wpDxPassBuffer.lock()->Bind( 2 );
	m_wpDxConstantsBuffer.lock()->Bind( 3 );
	ShaderProgramBase::End();
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

String DxShaderProgram::DoGetVertexShaderSource( uint32_t p_uiProgramFlags )
{
	String							l_strReturn;
	std::unique_ptr< UniformsBase >	l_pUniforms;
	std::unique_ptr< InOutsBase >	l_pInputs;

	try
	{
		switch ( m_pDxRenderSystem->GetFeatureLevel() )
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			l_pUniforms.reset( new UniformsStd );
			l_pInputs.reset( new InOutsOld );
			break;

		default:
			l_pUniforms.reset( new UniformsBuf );
			l_pInputs.reset( new InOutsNew );
			break;
		}
	}
	catch ( ... )
	{
		l_pInputs.reset();
		l_pUniforms.reset();
	}

	if ( l_pUniforms && l_pInputs )
	{
		l_strReturn += l_pUniforms->GetVertexInMatrices();
		l_strReturn += l_pInputs->GetVtxInput();
		l_strReturn += l_pInputs->GetVtxOutput();
		l_strReturn +=
			cuT( "VtxOutput mainVx( in VtxInput p_input )\n"	)
			cuT( "{\n"	)
			cuT( "	VtxOutput l_output;\n"	)
			cuT( "	p_input.Position.w = 1.0f;\n"	)
			cuT( "	float3x3 l_mtxNormal	= (float3x3)c3d_mtxNormal;\n"	)
			cuT( "	l_output.Position		= mul( p_input.Position, c3d_mtxProjectionModelView );\n"	)
			cuT( "	l_output.Normal			= normalize( mul( p_input.Normal,	l_mtxNormal ) );\n"	)
			cuT( "	l_output.Tangent		= normalize( mul( p_input.Tangent,	l_mtxNormal ) );\n"	)
			cuT( "	l_output.Binormal		= cross( l_output.Tangent, l_output.Normal );\n"	)
			cuT( "	l_output.Vertex			= mul( p_input.Position, c3d_mtxModelView ).xyz;\n"	)
			cuT( "	l_output.TextureUV		= p_input.TextureUV;\n"	)
			cuT( "	float3 tmpVec			= -l_output.Vertex;\n"	)
			cuT( "	l_output.Eye.x			= dot( tmpVec, l_output.Tangent		);\n"	)
			cuT( "	l_output.Eye.y			= dot( tmpVec, l_output.Binormal	);\n"	)
			cuT( "	l_output.Eye.z			= dot( tmpVec, l_output.Normal		);\n"	)
			cuT( "	return l_output;\n"	)
			cuT( "}\n"	);
	}

	return l_strReturn;
}

String DxShaderProgram::DoGetPixelShaderSource( uint32_t p_uiFlags )
{
	String							l_strReturn;
	std::unique_ptr< UniformsBase >	l_pUniforms;
	std::unique_ptr< InOutsBase >	l_pInputs;

	try
	{
		switch ( m_pDxRenderSystem->GetFeatureLevel() )
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			l_pUniforms.reset( new UniformsStd );
			l_pInputs.reset( new InOutsOld );
			break;

		default:
			l_pUniforms.reset( new UniformsBuf );
			l_pInputs.reset( new InOutsNew );
			break;
		}
	}
	catch ( ... )
	{
		l_pInputs.reset();
		l_pUniforms.reset();
	}

	if ( l_pUniforms && l_pInputs )
	{
		String	l_strIndex;
		String	l_strOutput							= g_ps.GetOutput();
		String	l_strDeclarations					= g_ps.GetDeclarations();
		String	l_strLights							= g_ps.GetLights();
		String	l_strMainDeclarations				= g_ps.GetMainDeclarations();
		String	l_strMainLightsLoop					= g_ps.GetMainLightsLoop();
		String	l_strMainLightsLoopEnd				= g_ps.GetMainLightsLoopEnd();
		String	l_strMainEnd						= g_ps.GetMainEnd();
		String	l_strMainLightsLoopAfterLightDir	= g_ps.GetMainLightsLoopAfterLightDir();
		l_strDeclarations += l_pUniforms->GetPixelInMatrices();
		l_strDeclarations += l_pUniforms->GetPixelScene();
		l_strDeclarations += l_pUniforms->GetPixelPass();
		l_strDeclarations += l_pInputs->GetPxlInput();
		l_strMainDeclarations = str_utils::replace( l_strMainDeclarations, cuT( "[PxlOutput]" ), l_pInputs->GetPxlOutput() );

		if ( p_uiFlags )
		{
			int l_iIndex = 1;

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
			{
				l_strIndex.clear();
				l_strIndex << l_iIndex++;
				l_strDeclarations					+= cuT( "Texture2D			c3d_mapColour : register( t" ) + l_strIndex + cuT( " );\n" );
				l_strMainDeclarations				+= cuT( "	float4	l_v4MapColour		= c3d_mapColour.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				l_strMainLightsLoopEnd				+= cuT( "	l_v4Ambient		*= l_v4MapColour;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
			{
				l_strIndex.clear();
				l_strIndex << l_iIndex++;
				l_strDeclarations					+= cuT( "Texture2D			c3d_mapAmbient : register( t" ) + l_strIndex + cuT( " );\n" );
				l_strMainDeclarations				+= cuT( "	float4	l_v4MapAmbient		= c3d_mapAmbient.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				l_strMainLightsLoopEnd				+= cuT( "	l_v4Ambient		*= l_v4MapAmbient;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
			{
				l_strIndex.clear();
				l_strIndex << l_iIndex++;
				l_strDeclarations					+= cuT( "Texture2D			c3d_mapDiffuse : register( t" ) + l_strIndex + cuT( " );\n" );
				l_strMainDeclarations				+= cuT( "	float4	l_v4MapDiffuse		= c3d_mapDiffuse.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				l_strMainLightsLoopEnd				+= cuT( "	l_v4Ambient		*= l_v4MapDiffuse;\n" );
				l_strMainLightsLoopEnd				+= cuT( "	l_v4Diffuse		*= l_v4MapDiffuse;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
			{
				l_strIndex.clear();
				l_strIndex << l_iIndex++;
				l_strDeclarations					+= cuT( "Texture2D			c3d_mapNormal : register( t" ) + l_strIndex + cuT( " );\n" );
				l_strMainDeclarations				+= cuT( "	float4	l_v4MapNormal		= c3d_mapNormal.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				l_strMainDeclarations				+= cuT( "	float 	l_fSqrLength;\n" );
				l_strMainDeclarations				+= cuT( "	float 	l_fAttenuation;\n" );
				l_strMainDeclarations				+= cuT( "	float	l_fInvRadius		= 0.02;\n" );
				l_strMainDeclarations				+= cuT( "	l_v3Normal					= normalize( l_v4MapNormal.xyz * 2.0 - 1.0 );\n" );
//				l_strMainLightsLoop					+= cuT( "		l_v3LightDir 	= (vtx_mtxView * vec4( l_v3LightDir, 1 )).xyz;\n" );
				l_strMainLightsLoop					+= cuT( "		l_v3LightDir 	= float3( dot( l_v3LightDir, p_input.Tangent ), dot( l_v3LightDir, p_input.Binormal	), dot( l_v3LightDir, p_input.Normal ) );\n" );
				l_strMainLightsLoop					+= cuT( "		l_fSqrLength	= dot( l_v3LightDir, l_v3LightDir );\n" );
				l_strMainLightsLoop					+= cuT( "		l_v3LightDir	= l_v3LightDir * rsqrt( l_fSqrLength );\n" );
				l_strMainLightsLoop					+= cuT( "		l_fAttenuation	= clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n" );
				l_strMainLightsLoopAfterLightDir	+= cuT( "		l_v4Ambient		*= l_fAttenuation;\n" );
				l_strMainLightsLoopAfterLightDir	+= cuT( "		l_v4Diffuse		*= l_fAttenuation;\n" );
				l_strMainLightsLoopAfterLightDir	+= cuT( "		l_v4Specular	*= l_fAttenuation;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			{
				l_strIndex.clear();
				l_strIndex << l_iIndex++;
				l_strDeclarations					+= cuT( "Texture2D			c3d_mapOpacity : register( t" ) + l_strIndex + cuT( " );\n" );
				l_strMainDeclarations				+= cuT( "	float4	l_v4MapOpacity		= c3d_mapOpacity.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				l_strMainLightsLoopEnd				+= cuT( "	l_fAlpha		= l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
			{
				l_strIndex.clear();
				l_strIndex << l_iIndex++;
				l_strDeclarations					+= cuT( "Texture2D			c3d_mapSpecular : register( t" ) + l_strIndex + cuT( " );\n" );
				l_strMainDeclarations				+= cuT( "	float4	l_v4MapSpecular		= c3d_mapSpecular.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
				l_strMainLightsLoopEnd				+= cuT( "	l_v4Specular	*= l_v4MapSpecular;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
			{
				l_strIndex.clear();
				l_strIndex << l_iIndex++;
				l_strDeclarations					+= cuT( "Texture2D			c3d_mapHeight : register( t" ) + l_strIndex + cuT( " );\n" );
				l_strMainDeclarations				+= cuT( "	float4	l_v4MapHeight		= c3d_mapHeight.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
			{
				l_strIndex.clear();
				l_strIndex << l_iIndex++;
				l_strDeclarations					+= cuT( "Texture2D			c3d_mapGloss : register( t" ) + l_strIndex + cuT( " );\n" );
				l_strMainDeclarations				+= cuT( "	float4	l_v4MapGloss		= c3d_mapGloss.Sample( DefaultSampler, p_input.TextureUV.xy );\n" );
			}
		}

		l_strMainLightsLoopEnd	+= cuT( "	" ) + l_strOutput + cuT( " = float4( (l_v4Ambient + l_v4Diffuse + l_v4Specular).xyz, l_fAlpha );\n" );
		l_strReturn = l_strDeclarations + l_strLights + l_strMainDeclarations + l_strMainLightsLoop + l_strMainLightsLoopAfterLightDir + l_strMainLightsLoopEnd + l_strMainEnd;
//		Logger::LogDebug( l_strReturn );
	}

	return l_strReturn;
}

String DxShaderProgram::DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass )
{
	String							l_strReturn;
	String							l_strVertex;
	std::unique_ptr< UniformsBase >	l_pUniforms;
	std::unique_ptr< InOutsBase >	l_pInputs;

	try
	{
		switch ( m_pDxRenderSystem->GetFeatureLevel() )
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			l_pUniforms.reset( new UniformsStd );
			l_pInputs.reset( new InOutsOld );
			break;

		default:
			l_pUniforms.reset( new UniformsBuf );
			l_pInputs.reset( new InOutsNew );
			break;
		}
	}
	catch ( ... )
	{
		l_pInputs.reset();
		l_pUniforms.reset();
	}

	if ( l_pUniforms && l_pInputs )
	{
		if ( p_bLightPass )
		{
			l_strReturn = g_ds.GetLSVertexProgram();
		}
		else
		{
			l_strVertex = g_ds.GetGSVertexProgram();
			l_strReturn = l_strVertex;
//			Logger::LogDebug( l_strReturn );
		}
	}

	return l_strReturn;
}

String DxShaderProgram::DoGetDeferredPixelShaderSource( uint32_t p_uiFlags )
{
	String							l_strReturn;
	String							l_strIndex;
	String							l_strPixelOutput;
	String							l_strPixelDeclarations;
	String							l_strPixelMainDeclarations;
	String							l_strPixelMainLightsLoopAfterLightDir;
	String							l_strPixelMainLightsLoopEnd;
	String							l_strPixelMainEnd;
	std::unique_ptr< UniformsBase >	l_pUniforms;
	std::unique_ptr< InOutsBase >	l_pInputs;

	try
	{
		switch ( m_pDxRenderSystem->GetFeatureLevel() )
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_3:
			l_pUniforms.reset( new UniformsStd );
			l_pInputs.reset( new InOutsOld );
			break;

		default:
			l_pUniforms.reset( new UniformsBuf );
			l_pInputs.reset( new InOutsNew );
			break;
		}
	}
	catch ( ... )
	{
		l_pInputs.reset();
		l_pUniforms.reset();
	}

	if ( l_pUniforms && l_pInputs )
	{
		if ( ( p_uiFlags & eTEXTURE_CHANNEL_LGHTPASS ) == eTEXTURE_CHANNEL_LGHTPASS )
		{
			l_strReturn = g_ds.GetLSPixelProgram();
		}
		else
		{
			l_strPixelDeclarations					= g_ds.GetGSPixelDeclarations();
			l_strPixelMainDeclarations				= g_ds.GetGSPixelMainDeclarations();
			l_strPixelMainLightsLoopAfterLightDir	= g_ds.GetGSPixelMainLightsLoopAfterLightDir();
			l_strPixelMainEnd						= g_ds.GetGSPixelMainEnd();

			if ( p_uiFlags != 0 )
			{
				int l_iIndex = 1;

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					l_strIndex.clear();
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations						+= cuT( "texture			c3d_mapColour;\n" );
					l_strPixelDeclarations						+= cuT( "sampler ColourTextureSampler\n" );
					l_strPixelDeclarations						+= cuT( "{\n" );
					l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapColour >;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressU  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressV  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressW  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	MipFilter = NONE;\n" );
					l_strPixelDeclarations						+= cuT( "	MinFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "	MagFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "};\n" );
					l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapColour		= tex2D( ColourTextureSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Diffuse		*= l_v4MapColour;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
				{
					l_strIndex.clear();
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations						+= cuT( "texture			c3d_mapAmbient;\n" );
					l_strPixelDeclarations						+= cuT( "sampler AmbientTextureSampler\n" );
					l_strPixelDeclarations						+= cuT( "{\n" );
					l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapAmbient >;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressU  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressV  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressW  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	MipFilter = NONE;\n" );
					l_strPixelDeclarations						+= cuT( "	MinFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "	MagFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "};\n" );
					l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapAmbient		= tex2D( AmbientTextureSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Diffuse		*= l_v4MapAmbient;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
				{
					l_strIndex.clear();
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations						+= cuT( "texture			c3d_mapDiffuse;\n" );
					l_strPixelDeclarations						+= cuT( "sampler DiffuseTextureSampler\n" );
					l_strPixelDeclarations						+= cuT( "{\n" );
					l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapDiffuse >;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressU  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressV  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressW  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	MipFilter = NONE;\n" );
					l_strPixelDeclarations						+= cuT( "	MinFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "	MagFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "};\n" );
					l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapDiffuse		= tex2D( DiffuseTextureSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Diffuse		*= l_v4MapDiffuse;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					l_strIndex.clear();
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations						+= cuT( "texture			c3d_mapNormal;\n" );
					l_strPixelDeclarations						+= cuT( "sampler NormalTextureSampler\n" );
					l_strPixelDeclarations						+= cuT( "{\n" );
					l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapNormal >;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressU  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressV  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressW  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	MipFilter = NONE;\n" );
					l_strPixelDeclarations						+= cuT( "	MinFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "	MagFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "};\n" );
					l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapNormal		= tex2D( NormalTextureSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainDeclarations					+= cuT( "	l_v4Normal					+= float4( normalize( (l_v4MapNormal.xyz * 2.0 - 1.0) ), 0 );\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					l_strIndex.clear();
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations						+= cuT( "texture			c3d_mapOpacity;\n" );
					l_strPixelDeclarations						+= cuT( "sampler OpacityTextureSampler\n" );
					l_strPixelDeclarations						+= cuT( "{\n" );
					l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapOpacity >;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressU  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressV  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressW  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	MipFilter = NONE;\n" );
					l_strPixelDeclarations						+= cuT( "	MinFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "	MagFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "};\n" );
					l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapOpacity		= tex2D( OpacityTextureSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd					+= cuT( "	l_fAlpha		= l_v4MapOpacity.r * l_v4MapOpacity.a;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					l_strIndex.clear();
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations						+= cuT( "texture			c3d_mapSpecular;\n" );
					l_strPixelDeclarations						+= cuT( "sampler SpecularTextureSampler)\n" );
					l_strPixelDeclarations						+= cuT( "{\n" );
					l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapSpecular >;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressU  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressV  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressW  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	MipFilter = NONE;\n" );
					l_strPixelDeclarations						+= cuT( "	MinFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "	MagFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "};\n" );
					l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapSpecular		= tex2D( SpecularTextureSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Specular	*= l_v4MapSpecular;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
				{
					l_strIndex.clear();
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations						+= cuT( "texture			c3d_mapHeight;\n" );
					l_strPixelDeclarations						+= cuT( "sampler HeightTextureSampler)\n" );
					l_strPixelDeclarations						+= cuT( "{\n" );
					l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapHeight >;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressU  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressV  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressW  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	MipFilter = NONE;\n" );
					l_strPixelDeclarations						+= cuT( "	MinFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "	MagFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "};\n" );
					l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapHeight		= tex2D( HeightTextureSampler, p_input.TextureUV.xy );\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
				{
					l_strIndex.clear();
					l_strIndex << l_iIndex++;
					l_strPixelDeclarations						+= cuT( "texture			c3d_mapGloss;\n" );
					l_strPixelDeclarations						+= cuT( "sampler GlossTextureSampler)\n" );
					l_strPixelDeclarations						+= cuT( "{\n" );
					l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapGloss >;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressU  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressV  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	AddressW  = WRAP;\n" );
					l_strPixelDeclarations						+= cuT( "	MipFilter = NONE;\n" );
					l_strPixelDeclarations						+= cuT( "	MinFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "	MagFilter = LINEAR;\n" );
					l_strPixelDeclarations						+= cuT( "};\n" );
					l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapGloss		= tex2D( GlossTextureSampler, p_input.TextureUV.xy );\n" );
					l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Specular.w		*= l_v4MapGloss.x;\n" );
				}
			}

			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.Position		= float4( l_v4Position.xyz,	1 );\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.Normals		= float4( l_v4Normal.xyz,	1 );\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.DiffSpecular.x	= pack2Floats(l_v4Diffuse.x,l_v4Diffuse.y);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.DiffSpecular.y	= pack2Floats(l_v4Diffuse.z,l_v4Diffuse.w);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.DiffSpecular.z	= pack2Floats(l_v4Specular.x,l_v4Specular.y);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.DiffSpecular.w	= pack2Floats(l_v4Specular.z,l_v4Specular.w);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.TanBitangent.x	= pack2Floats(l_v4Tangent.x,l_v4Tangent.y);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.TanBitangent.y	= pack2Floats(l_v4Tangent.z,l_v4Tangent.w);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.TanBitangent.z	= pack2Floats(l_v4Bitangent.x,l_v4Bitangent.y);\n" );
			l_strPixelMainLightsLoopEnd	+= cuT( "	l_output.TanBitangent.w	= pack2Floats(l_v4Bitangent.z,l_v4Bitangent.w);\n" );
			l_strReturn = l_strPixelDeclarations + l_strPixelMainDeclarations + l_strPixelMainLightsLoopAfterLightDir + l_strPixelMainLightsLoopEnd + l_strPixelMainEnd;
//			Logger::LogDebug( l_strReturn );
		}
	}

	return l_strReturn;
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
	return std::make_shared< DxOneFrameVariable< TextureBaseRPtr > >( static_cast< DxRenderSystem * >( m_pRenderSystem ), this, p_iNbOcc );
}

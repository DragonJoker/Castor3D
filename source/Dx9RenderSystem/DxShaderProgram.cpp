#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/DxShaderProgram.hpp"
#include "Dx9RenderSystem/DxShaderObject.hpp"
#include "Dx9RenderSystem/DxFrameVariable.hpp"
#include "Dx9RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx9Render;

//*************************************************************************************************

namespace
{
	const std::string StrCameraPos			=	str_utils::to_str( ShaderProgramBase::CameraPos			);
	const std::string StrAmbientLight		=	str_utils::to_str( ShaderProgramBase::AmbientLight		);
	const std::string StrBackgroundColour	=	str_utils::to_str( ShaderProgramBase::BackgroundColour	);
	const std::string StrLightsCount		=	str_utils::to_str( ShaderProgramBase::LightsCount		);
	const std::string StrLights				=	str_utils::to_str( ShaderProgramBase::Lights			);
	const std::string StrMatAmbient			=	str_utils::to_str( ShaderProgramBase::MatAmbient		);
	const std::string StrMatDiffuse			=	str_utils::to_str( ShaderProgramBase::MatDiffuse		);
	const std::string StrMatSpecular		=	str_utils::to_str( ShaderProgramBase::MatSpecular		);
	const std::string StrMatEmissive		=	str_utils::to_str( ShaderProgramBase::MatEmissive		);
	const std::string StrMatShininess		=	str_utils::to_str( ShaderProgramBase::MatShininess		);
	const std::string StrMatOpacity			=	str_utils::to_str( ShaderProgramBase::MatOpacity		);
	const std::string StrMapColour			=	str_utils::to_str( ShaderProgramBase::MapColour			);
	const std::string StrMapAmbient			=	str_utils::to_str( ShaderProgramBase::MapAmbient		);
	const std::string StrMapDiffuse			=	str_utils::to_str( ShaderProgramBase::MapDiffuse		);
	const std::string StrMapSpecular		=	str_utils::to_str( ShaderProgramBase::MapSpecular		);
	const std::string StrMapNormal			=	str_utils::to_str( ShaderProgramBase::MapNormal			);
	const std::string StrMapOpacity			=	str_utils::to_str( ShaderProgramBase::MapOpacity		);
	const std::string StrMapGloss			=	str_utils::to_str( ShaderProgramBase::MapGloss			);
	const std::string StrMapHeight			=	str_utils::to_str( ShaderProgramBase::MapHeight			);
}

//*************************************************************************************************

class ShaderModelBase
{
protected:
	String m_strVertexProgram						;
	String m_strPixelDeclarations					;
	String m_strPixelLights							;
	String m_strPixelMainDeclarations				;
	String m_strPixelMainLightsLoop					;
	String m_strPixelMainLightsLoopAfterLightDir	;
	String m_strPixelMainLightsLoopEnd				;
	String m_strPixelMainEnd						;
	String m_strPixelOutput							;

public:
	virtual const String & GetVertexProgram()						{ return m_strVertexProgram;					}
	virtual const String & GetPixelDeclarations()					{ return m_strPixelDeclarations;				}
	virtual const String & GetPixelLights()							{ return m_strPixelLights;						}
	virtual const String & GetPixelMainDeclarations()				{ return m_strPixelMainDeclarations;			}
	virtual const String & GetPixelMainLightsLoop()					{ return m_strPixelMainLightsLoop;				}
	virtual const String & GetPixelMainLightsLoopAfterLightDir()	{ return m_strPixelMainLightsLoopAfterLightDir;	}
	virtual const String & GetPixelMainLightsLoopEnd()				{ return m_strPixelMainLightsLoopEnd;			}
	virtual const String & GetPixelMainEnd()						{ return m_strPixelMainEnd;						}
	virtual const String & GetPixelOutput()							{ return m_strPixelOutput;						}
};

class ShaderModel_1_2 : public ShaderModelBase
{
public:
	ShaderModel_1_2()
	{
		m_strVertexProgram =
			cuT( "uniform		float4x4	c3d_mtxProjection;\n"																					)
			cuT( "uniform		float4x4	c3d_mtxModel;\n"																						)
			cuT( "uniform		float4x4	c3d_mtxView;\n"																							)
			cuT( "uniform		float4x4	c3d_mtxModelView;\n"																					)
			cuT( "uniform		float4x4	c3d_mtxProjectionModelView;\n"																			)
			cuT( "uniform		float4x4	c3d_mtxNormal;\n"																						)
			cuT( "struct VtxInput\n"																												)
			cuT( "{\n"																																)
			cuT( "	float4	Position	:	POSITION0;\n"																							)
			cuT( "	float3	Normal		:	NORMAL0;\n"																								)
			cuT( "	float3	Tangent		:	TANGENT0;\n"																							)
			cuT( "	float2	TextureUV	:	TEXCOORD0;\n"																							)
			cuT( "};\n"																																)
			cuT( "struct VtxOutput\n"																												)
			cuT( "{\n"																																)
			cuT( "	float4	Position	:	POSITION0;\n"																							)
			cuT( "	float3	Vertex		:	POSITION1;\n"																							)
			cuT( "	float3	Normal		:	NORMAL0;\n"																								)
			cuT( "	float3	Tangent		:	TANGENT0;\n"																							)
			cuT( "	float3	Binormal	:	TANGENT1;\n"																							)
			cuT( "	float2	TextureUV	:	TEXCOORD0;\n"																							)
			cuT( "	float3	Eye			:	NORMAL1;\n"																								)
			cuT( "};\n"																																)
			cuT( "VtxOutput mainVx( in VtxInput p_input )\n"																						)
			cuT( "{\n"																																)
			cuT( "	VtxOutput l_output;\n"																											)
			cuT( "	float3x3 l_mtxNormal	= (float3x3)c3d_mtxNormal;\n"																			)
			cuT( "	l_output.Position		= mul( p_input.Position, c3d_mtxProjectionModelView );\n"												)
			cuT( "	l_output.Normal			= normalize( mul( p_input.Normal,	l_mtxNormal ) );\n"													)
			cuT( "	l_output.Tangent		= normalize( mul( p_input.Tangent,	l_mtxNormal ) );\n"													)
			cuT( "	l_output.Binormal		= cross( l_output.Tangent, l_output.Normal );\n"														)
			cuT( "	l_output.Vertex			= mul( p_input.Position, c3d_mtxModelView ).xyz;\n"														)
			cuT( "	l_output.TextureUV		= p_input.TextureUV;\n"																					)
			cuT( "	float3 tmpVec			= -l_output.Vertex;\n"																					)
			cuT( "	l_output.Eye.x			= dot( tmpVec, l_output.Tangent		);\n"																)
			cuT( "	l_output.Eye.y			= dot( tmpVec, l_output.Binormal	);\n"																)
			cuT( "	l_output.Eye.z			= dot( tmpVec, l_output.Normal		);\n"																)
			cuT( "	return l_output;\n"																												)
			cuT( "}\n"																																);

		m_strPixelDeclarations =
			cuT( "uniform	float4		c3d_v4AmbientLight;\n"																						)
			cuT( "uniform	float4		c3d_v4MatAmbient;\n"																						)
			cuT( "uniform	float4		c3d_v4MatEmissive;\n"																						)
			cuT( "uniform	float4		c3d_v4MatDiffuse;\n"																						)
			cuT( "uniform	float4		c3d_v4MatSpecular;\n"																						)
			cuT( "uniform	float		c3d_fMatShininess;\n"																						)
			cuT( "uniform	float		c3d_fMatOpacity;\n"																							)
			cuT( "uniform	int			c3d_iLightsCount;\n"																						)
			cuT( "texture				c3d_sLights;\n"																								)
			cuT( "sampler LightsSampler = \n"																										)
			cuT( "sampler_state\n"																													)
			cuT( "{\n"																																)
			cuT( "	Texture		= < c3d_sLights >;\n"																								)
			cuT( "	MagFilter	= POINT;\n"																											)
			cuT( "	MinFilter	= POINT;\n"																											)
			cuT( "	MipFilter	= POINT;\n"																											)
			cuT( "};\n"																																);

		m_strPixelLights =
			cuT( "struct Light\n"																													)
			cuT( "{\n"																																)
			cuT( "	float4		m_v4Ambient;\n"																										)
			cuT( "	float4		m_v4Diffuse;\n"																										)
			cuT( "	float4		m_v4Specular;\n"																									)
			cuT( "	float4		m_v4Position;\n"																									)
			cuT( "	int			m_iType;\n"																											)
			cuT( "	float3		m_v3Attenuation;\n"																									)
			cuT( "	float4x4	m_mtx4Orientation;\n"																								)
			cuT( "	float		m_fExponent;\n"																										)
			cuT( "	float		m_fCutOff;\n"																										)
			cuT( "};\n"																																)
			cuT( "Light GetLight( int i )\n"																										)
			cuT( "{\n"																																)
			cuT( "	Light l_lightReturn;\n"																											)
			cuT( "	l_lightReturn.m_v4Ambient		= tex2D( LightsSampler, float2( (i * 0.01) + (0 * 0.001) + 0.0005, 0.0 ) );\n"					)
			cuT( "	l_lightReturn.m_v4Diffuse		= tex2D( LightsSampler, float2( (i * 0.01) + (1 * 0.001) + 0.0005, 0.0 ) );\n"					)
			cuT( "	l_lightReturn.m_v4Specular		= tex2D( LightsSampler, float2( (i * 0.01) + (2 * 0.001) + 0.0005, 0.0 ) );\n"					)
			cuT( "	l_lightReturn.m_v4Position		= tex2D( LightsSampler, float2( (i * 0.01) + (3 * 0.001) + 0.0005, 0.0 ) );\n"					)
			cuT( "	l_lightReturn.m_v3Attenuation	= tex2D( LightsSampler, float2( (i * 0.01) + (4 * 0.001) + 0.0005, 0.0 ) ).xyz;\n"				)
			cuT( "	float4	l_v4A					= tex2D( LightsSampler, float2( (i * 0.01) + (5 * 0.001) + 0.0005, 0.0 ) );\n"					)
			cuT( "	float4	l_v4B					= tex2D( LightsSampler, float2( (i * 0.01) + (6 * 0.001) + 0.0005, 0.0 ) );\n"					)
			cuT( "	float4	l_v4C					= tex2D( LightsSampler, float2( (i * 0.01) + (7 * 0.001) + 0.0005, 0.0 ) );\n"					)
			cuT( "	float4	l_v4D					= tex2D( LightsSampler, float2( (i * 0.01) + (8 * 0.001) + 0.0005, 0.0 ) );\n"					)
			cuT( "	float2	l_v2Spot				= tex2D( LightsSampler, float2( (i * 0.01) + (9 * 0.001) + 0.0005, 0.0 ) ).xy;\n"				)
			cuT( "	l_lightReturn.m_iType			= int( l_lightReturn.m_v4Position.w );\n"														)
			cuT( "	l_lightReturn.m_mtx4Orientation	= float4x4( l_v4A, l_v4B, l_v4C, l_v4D );\n"													)
			cuT( "	l_lightReturn.m_fExponent		= l_v2Spot.x;\n"																				)
			cuT( "	l_lightReturn.m_fCutOff			= l_v2Spot.x;\n"																				)
			cuT( "	return l_lightReturn;\n"																										)
			cuT( "}\n"																																);

		m_strPixelMainDeclarations =
			cuT( "float4 mainPx( in VtxOutput p_input ) : COLOR0\n"																					)
			cuT( "{\n"																																)
			cuT( "	float4	l_v4Return	= float4( 0, 0, 0, 0 );\n"																					)
			cuT( "	Light	l_light;\n"																												)
			cuT( "	float3	l_v3LightDir;\n"																										)
			cuT( "	float	l_fLambert;\n"																											)
			cuT( "	float	l_fSpecular;\n"																											)
			cuT( "	float3	l_v3Normal			= p_input.Normal;\n"																				)
			cuT( "	float4	l_v4Ambient			= float4( 0, 0, 0, 0 );\n"																			)
			cuT( "	float4	l_v4Diffuse			= float4( 0, 0, 0, 0 );\n"																			)
			cuT( "	float4	l_v4Specular		= float4( 0, 0, 0, 0 );\n"																			)
			cuT( "	float4	l_v4TmpAmbient		= float4( 0, 0, 0, 0 );\n"																			)
			cuT( "	float4	l_v4TmpDiffuse		= float4( 0, 0, 0, 0 );\n"																			)
			cuT( "	float4	l_v4TmpSpecular		= float4( 0, 0, 0, 0 );\n"																			)
			cuT( "	float3	l_v3EyeVec			= normalize( p_input.Eye );\n"																		)
			cuT( "	float	l_fAlpha			= c3d_fMatOpacity;\n"																				);

		m_strPixelMainLightsLoop =
			cuT( "	for( int i = 0; i < 3; i++ )\n"																									)
			cuT( "	{\n"																															)
			cuT( "		l_light			= GetLight( i );\n"																							)
			cuT( "		l_v3LightDir	= normalize( mul( l_light.m_v4Position, c3d_mtxModelView ).xyz - p_input.Vertex );\n"						);

		m_strPixelMainLightsLoopAfterLightDir =
			cuT( "		l_fLambert		= max( dot( l_v3Normal, l_v3LightDir ), 0.0 );\n"															)
			cuT( "		l_fSpecular		= pow( clamp( dot( reflect( -l_v3LightDir, l_v3Normal ), l_v3EyeVec ), 0.0, 1.0 ), c3d_fMatShininess );\n"	)
			cuT( "		l_v4TmpAmbient	= l_light.m_v4Ambient	* c3d_v4MatAmbient;\n"																)
			cuT( "		l_v4TmpDiffuse	= l_light.m_v4Diffuse	* c3d_v4MatDiffuse 	* l_fLambert;\n"												)
			cuT( "		l_v4TmpSpecular	= l_light.m_v4Specular	* c3d_v4MatSpecular	* l_fSpecular;\n"												);

		m_strPixelMainLightsLoopEnd =
			cuT( "		l_v4Ambient		+= l_v4TmpAmbient;\n"																						)
			cuT( "		l_v4Diffuse		+= l_v4TmpDiffuse;\n"																						)
			cuT( "		l_v4Specular	+= l_v4TmpSpecular;\n"																						)
			cuT( "	}\n"																															);

		m_strPixelMainEnd =
			cuT( "	return l_v4Return;\n"																											)
			cuT( "}\n"																																)
			cuT( "technique RenderPass\n"																											)
			cuT( "{\n"																																)
			cuT( "	pass p0\n"																														)
			cuT( "	{\n"																															)
			cuT( "		PixelShader = compile ps_3_0 mainPx();\n"																					)
			cuT( "		VertexShader = compile vs_3_0 mainVx();\n"																					)
			cuT( "	}\n"																															)
			cuT( "};\n"																																);

		m_strPixelOutput =
			cuT( "l_v4Return"																														);
	}
};

class DeferredShaderModelBase
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
	virtual const String & GetGSVertexProgram()						{ return m_strGSVertexProgram;						}
	virtual const String & GetGSPixelDeclarations()					{ return m_strGSPixelDeclarations;					}
	virtual const String & GetGSPixelMainDeclarations()				{ return m_strGSPixelMainDeclarations;				}
	virtual const String & GetGSPixelMainLightsLoopAfterLightDir()	{ return m_strGSPixelMainLightsLoopAfterLightDir;	}
	virtual const String & GetGSPixelMainEnd()						{ return m_strGSPixelMainEnd;						}
	virtual const String & GetLSVertexProgram()						{ return m_strLSVertexProgram;						}
	virtual const String & GetLSPixelProgram()						{ return m_strLSPixelProgram;						}
};

class DeferredShaderModel_1_2 : public DeferredShaderModelBase
{
public:
	DeferredShaderModel_1_2()
	{
		m_strGSVertexProgram =
			cuT( "uniform	float4x4	c3d_mtxProjectionModelView;\n"																									)
			cuT( "uniform	float4x4	c3d_mtxModelView;\n"																											)
			cuT( "uniform	float4x4	c3d_mtxView;\n"																													)
			cuT( "uniform	float4x4	c3d_mtxNormal;\n"																												)
			cuT( "struct VtxInput\n"																																	)
			cuT( "{\n"																																					)
			cuT( "	float4	Position	:	POSITION0;\n"																												)
			cuT( "	float3	Normal		:	NORMAL0;\n"																													)
			cuT( "	float3	Tangent		:	TANGENT0;\n"																												)
			cuT( "	float3	Bitangent	:	TANGENT1;\n"																												)
			cuT( "	float2	TextureUV	:	TEXCOORD0;\n"																												)
			cuT( "};\n"																																					)
			cuT( "struct VtxOutput\n"																																	)
			cuT( "{\n"																																					)
			cuT( "	float4	Position	:	POSITION0;\n"																												)
			cuT( "	float3	Vertex		:	POSITION1;\n"																												)
			cuT( "	float3	Normal		:	NORMAL0;\n"																													)
			cuT( "	float3	Tangent		:	TANGENT0;\n"																												)
			cuT( "	float3	Bitangent	:	TANGENT1;\n"																												)
			cuT( "	float2	TextureUV	:	TEXCOORD0;\n"																												)
			cuT( "};\n"																																					)
			cuT( "VtxOutput mainVx( VtxInput p_input )\n"																												)
			cuT( "{\n"																																					)
			cuT( "	VtxOutput l_output;\n"																																)
			cuT( "	l_output.Position	= mul( p_input.Position, c3d_mtxProjectionModelView );\n"																		)
			cuT( "	l_output.Vertex		= l_output.Position.xyz;\n"																										)
			cuT( "	l_output.Normal		= normalize( mul( float4( p_input.Normal, 1.0 ),	c3d_mtxNormal ) ).xyz;\n"													)
			cuT( "	l_output.Tangent	= normalize( mul( float4( p_input.Tangent, 1.0 ),	c3d_mtxNormal ) ).xyz;\n"													)
			cuT( "	l_output.Bitangent	= normalize( mul( float4( p_input.Bitangent, 1.0 ),	c3d_mtxNormal ) ).xyz;\n"													)
			cuT( "	l_output.TextureUV 	= p_input.TextureUV;\n"																											)
			cuT( "	return l_output;\n"																																	)
			cuT( "}\n"																																					);

		m_strGSPixelDeclarations =
			cuT( "uniform	float4		c3d_v4AmbientLight;\n"																											)
			cuT( "uniform	float4		c3d_v4MatAmbient;\n"																											)
			cuT( "uniform	float4		c3d_v4MatEmissive;\n"																											)
			cuT( "uniform	float4		c3d_v4MatDiffuse;\n"																											)
			cuT( "uniform	float4		c3d_v4MatSpecular;\n"																											)
			cuT( "uniform	float		c3d_fMatShininess;\n"																											)
			cuT( "uniform	float		c3d_fMatOpacity;\n"																												)
			cuT( "float pack2Floats(float f1, float f2)\n"																												)
			cuT( "{\n"																																					)
			cuT( "	return (f2 + clamp(f1, 0.f, 0.999f)) * 10.f; // * 10.f, because I would loose the coef sometimes. pow = 128 and coef = 0.1 were bad for example\n"	)
			cuT( "}\n"																																					)
			cuT( "struct PxlOutput\n"																																	)
			cuT( "{\n"																																					)
			cuT( "	float4	Position		:	COLOR0;\n"																												)
			cuT( "	float4	DiffSpecular	:	COLOR1;\n"																												)
			cuT( "	float4	Normals			:	COLOR2;\n"																												)
			cuT( "	float4	TanBitangent	:	COLOR3;\n"																												)
			cuT( "};\n"																																					);

		m_strGSPixelMainDeclarations =
			cuT( "PxlOutput mainPx( in VtxOutput p_input )\n"																											)
			cuT( "{\n"																																					)
			cuT( "	PxlOutput l_output;\n"																																)
			cuT( "	float4	l_v4Position		= float4( p_input.Vertex, 1 );\n"																						)
			cuT( "	float4	l_v4Normal			= float4( p_input.Normal, 1 );\n"																						)
			cuT( "	float4	l_v4Tangent			= float4( p_input.Tangent, 1 );\n"																						)
			cuT( "	float4	l_v4Bitangent		= float4( p_input.Bitangent, 1 );\n"																					)
			cuT( "	float4	l_v4Diffuse			= float4( 0, 0, 0, 0 );\n"																								)
			cuT( "	float4	l_v4Texture			= float4( p_input.TextureUV.xy, 0, 1 );\n"																				)
			cuT( "	float4	l_v4Specular		= float4( c3d_v4MatSpecular.xyz, c3d_fMatShininess );\n"																)
			cuT( "	float	l_fAlpha			= c3d_fMatOpacity;\n"																									);

		m_strGSPixelMainLightsLoopAfterLightDir =
			cuT( "	l_v4Diffuse		= c3d_v4MatDiffuse;\n"																												);

		m_strGSPixelMainEnd =
			cuT( "	return l_output;\n"																																	)
			cuT( "}\n"																																					)
			cuT( "technique RenderPass\n"																																)
			cuT( "{\n"																																					)
			cuT( "	pass p0\n"																																			)
			cuT( "	{\n"																																				)
			cuT( "		CullMode=none;\n"																																)
			cuT( "		PixelShader = compile ps_3_0 mainPx();\n"																										)
			cuT( "		VertexShader = compile vs_3_0 mainVx();\n"																										)
			cuT( "	}\n"																																				)
			cuT( "};\n"																																					);
		
		m_strLSVertexProgram =
			cuT( "uniform	float4x4	c3d_mtxProjectionModelView;\n"																									)
			cuT( "struct VtxInput\n"																																	)
			cuT( "{\n"																																					)
			cuT( "	float4	Position	:	POSITION0;\n"																												)
			cuT( "	float2	TextureUV	:	TEXCOORD0;\n"																												)
			cuT( "};\n"																																					)
			cuT( "struct VtxOutput\n"																																	)
			cuT( "{\n"																																					)
			cuT( "	float4	Position	:	POSITION0;\n"																												)
			cuT( "	float2	TextureUV	:	TEXCOORD0;\n"																												)
			cuT( "};\n"																																					)
			cuT( "VtxOutput mainVx( in VtxInput p_input )\n"																											)
			cuT( "{\n"																																					)
			cuT( "	VtxOutput l_out;\n"																																	)
			cuT( "	l_out.Position = mul( p_input.Position, c3d_mtxProjectionModelView );\n"																			)
			cuT( "	l_out.TextureUV = p_input.TextureUV;\n"																												)
			cuT( "	return l_out;\n"																																	)
			cuT( "}\n"																																					);
		
		m_strLSPixelProgram =
			cuT( "uniform	int 		c3d_iLightsCount;\n"																												)
			cuT( "uniform	float3		c3d_v3CameraPosition;\n"																											)
			cuT( "Texture2D				c3d_sLights;\n"																														)
			cuT( "sampler2D LightsSampler\n"																																)
			cuT( "{\n"																																						)
			cuT( "	Texture = < c3d_sLights >;\n"																															)
			cuT( "	MagFilter = POINT;\n"																																	)
			cuT( "	MinFilter = POINT;\n"																																	)
			cuT( "	MipFilter = POINT;\n"																																	)
			cuT( "};\n"																																						)
			cuT( "Texture2D				c3d_mapPosition;\n"																													)
			cuT( "sampler PositionSampler\n"																																)
			cuT( "{\n"																																						)
			cuT( "	Texture = < c3d_mapPosition >;\n"																														)
			cuT( "	AddressU  = WRAP;\n"																																	)
			cuT( "	AddressV  = WRAP;\n"																																	)
			cuT( "	AddressW  = WRAP;\n"																																	)
			cuT( "	MipFilter = NONE;\n"																																	)
			cuT( "	MinFilter = LINEAR;\n"																																	)
			cuT( "	MagFilter = LINEAR;\n"																																	)
			cuT( "};\n"																																						)
			cuT( "Texture2D				c3d_mapDiffuse;\n"																													)
			cuT( "sampler DiffuseSampler\n"																																	)
			cuT( "{\n"																																						)
			cuT( "	Texture = < c3d_mapDiffuse >;\n"																														)
			cuT( "	AddressU  = WRAP;\n"																																	)
			cuT( "	AddressV  = WRAP;\n"																																	)
			cuT( "	AddressW  = WRAP;\n"																																	)
			cuT( "	MipFilter = NONE;\n"																																	)
			cuT( "	MinFilter = LINEAR;\n"																																	)
			cuT( "	MagFilter = LINEAR;\n"																																	)
			cuT( "};\n"																																						)
			cuT( "Texture2D				c3d_mapNormals;\n"																													)
			cuT( "sampler NormalsSampler\n"																																	)
			cuT( "{\n"																																						)
			cuT( "	Texture = < c3d_mapNormals >;\n"																														)
			cuT( "	AddressU  = WRAP;\n"																																	)
			cuT( "	AddressV  = WRAP;\n"																																	)
			cuT( "	AddressW  = WRAP;\n"																																	)
			cuT( "	MipFilter = NONE;\n"																																	)
			cuT( "	MinFilter = LINEAR;\n"																																	)
			cuT( "	MagFilter = LINEAR;\n"																																	)
			cuT( "};\n"																																						)
			cuT( "Texture2D				c3d_mapTangent;\n"																													)
			cuT( "sampler TangentSampler\n"																																	)
			cuT( "{\n"																																						)
			cuT( "	Texture = < c3d_mapTangent >;\n"																														)
			cuT( "	AddressU  = WRAP;\n"																																	)
			cuT( "	AddressV  = WRAP;\n"																																	)
			cuT( "	AddressW  = WRAP;\n"																																	)
			cuT( "	MipFilter = NONE;\n"																																	)
			cuT( "	MinFilter = LINEAR;\n"																																	)
			cuT( "	MagFilter = LINEAR;\n"																																	)
			cuT( "};\n"																																						)
			cuT( "Texture2D				c3d_mapBitangent;\n"																												)
			cuT( "sampler BitangentSampler\n"																																)
			cuT( "{\n"																																						)
			cuT( "	Texture = < c3d_mapBitangent >;\n"																														)
			cuT( "	AddressU  = WRAP;\n"																																	)
			cuT( "	AddressV  = WRAP;\n"																																	)
			cuT( "	AddressW  = WRAP;\n"																																	)
			cuT( "	MipFilter = NONE;\n"																																	)
			cuT( "	MinFilter = LINEAR;\n"																																	)
			cuT( "	MagFilter = LINEAR;\n"																																	)
			cuT( "};\n"																																						)
			cuT( "Texture2D				c3d_mapSpecular;\n"																													)
			cuT( "sampler SpecularSampler\n"																																)
			cuT( "{\n"																																						)
			cuT( "	Texture = < c3d_mapSpecular >;\n"																														)
			cuT( "	AddressU  = WRAP;\n"																																	)
			cuT( "	AddressV  = WRAP;\n"																																	)
			cuT( "	AddressW  = WRAP;\n"																																	)
			cuT( "	MipFilter = NONE;\n"																																	)
			cuT( "	MinFilter = LINEAR;\n"																																	)
			cuT( "	MagFilter = LINEAR;\n"																																	)
			cuT( "};\n"																																						)
			cuT( "struct Light\n"																																			)
			cuT( "{\n"																																						)
			cuT( "	float4		m_v4Ambient;\n"																																)
			cuT( "	float4		m_v4Diffuse;\n"																																)
			cuT( "	float4		m_v4Specular;\n"																															)
			cuT( "	float4		m_v4Position;\n"																															)
			cuT( "	int			m_iType;\n"																																	)
			cuT( "	float3		m_v3Attenuation;\n"																															)
			cuT( "	float4x4	m_mtx4Orientation;\n"																														)
			cuT( "	float		m_fExponent;\n"																																)
			cuT( "	float		m_fCutOff;\n"																																)
			cuT( "};\n"																																						)
			cuT( "Light GetLight( int i )\n"																																)
			cuT( "{\n"																																						)
			cuT( "	Light l_lightReturn;\n"																																	)
			cuT( "	l_lightReturn.m_v4Ambient		= tex2D( LightsSampler, float2( (i * 0.1) + (0 * 0.01) + 0.005, 0.0 ) );\n"												)
			cuT( "	l_lightReturn.m_v4Diffuse		= tex2D( LightsSampler, float2( (i * 0.1) + (1 * 0.01) + 0.005, 0.0 ) );\n"												)
			cuT( "	l_lightReturn.m_v4Specular		= tex2D( LightsSampler, float2( (i * 0.1) + (2 * 0.01) + 0.005, 0.0 ) );\n"												)
			cuT( "	l_lightReturn.m_v4Position		= tex2D( LightsSampler, float2( (i * 0.1) + (3 * 0.01) + 0.005, 0.0 ) );\n"												)
			cuT( "	l_lightReturn.m_v3Attenuation	= tex2D( LightsSampler, float2( (i * 0.1) + (4 * 0.01) + 0.005, 0.0 ) ).xyz;\n"											)
			cuT( "	float4	l_v4A					= tex2D( LightsSampler, float2( (i * 0.1) + (5 * 0.01) + 0.005, 0.0 ) );\n"												)
			cuT( "	float4	l_v4B					= tex2D( LightsSampler, float2( (i * 0.1) + (6 * 0.01) + 0.005, 0.0 ) );\n"												)
			cuT( "	float4	l_v4C					= tex2D( LightsSampler, float2( (i * 0.1) + (7 * 0.01) + 0.005, 0.0 ) );\n"												)
			cuT( "	float4	l_v4D					= tex2D( LightsSampler, float2( (i * 0.1) + (8 * 0.01) + 0.005, 0.0 ) );\n"												)
			cuT( "	float2	l_v2Spot				= tex2D( LightsSampler, float2( (i * 0.1) + (9 * 0.01) + 0.005, 0.0 ) ).xy;\n"											)
			cuT( "	l_lightReturn.m_iType			= int( l_lightReturn.m_v4Position.w );\n"																				)
			cuT( "	l_lightReturn.m_mtx4Orientation	= float4x4( l_v4A, l_v4B, l_v4C, l_v4D );\n"																			)
			cuT( "	l_lightReturn.m_fExponent		= l_v2Spot.x;\n"																										)
			cuT( "	l_lightReturn.m_fCutOff			= l_v2Spot.x;\n"																										)
			cuT( "	return l_lightReturn;\n"																																)
			cuT( "}\n"																																						)
			cuT( "float2 unpack2Floats(float fBoth)\n"																														)
			cuT( "{\n"																																						)
			cuT( "	fBoth *= 0.1f;\n"																																		)
			cuT( "	float f1 = frac(fBoth);\n"																																)
			cuT( "	float f2 = fBoth - f1;\n"																																)
			cuT( "	return float2(f1, f2);\n"																																)
			cuT( "}\n"																																						)
			cuT( "float4 mainPx( in VtxOutput p_input ) : COLOR0\n"																											)
			cuT( "{\n"																																						)
			cuT( "	float4	l_output;\n"																																	)
			cuT( "	Light	l_light;\n"																																		)
			cuT( "	float4	l_v4Diffuses;\n"																																)
			cuT( "	float4	l_v4Speculars;\n"																																)
			cuT( "	float4	l_v4Tangents;\n"																																)
			cuT( "	float4	l_v4Bitangents;\n"																																)
			cuT( "	float4	l_v4Positions	= tex2D( PositionSampler,	p_input.TextureUV );\n"																				)
			cuT( "	float4	l_v4DiffSpec	= tex2D( DiffuseSampler,	p_input.TextureUV );\n"																				)
			cuT( "	float4	l_v4Normals		= tex2D( NormalsSampler,	p_input.TextureUV ) / 2.0;\n"																		)
			cuT( "	float4	l_v4TanBiTan	= tex2D( TangentSampler,	p_input.TextureUV );\n"																				)
			cuT( "	l_v4Diffuses.xy			= unpack2Floats(l_v4DiffSpec.x);\n"																								)
			cuT( "	l_v4Diffuses.zw			= unpack2Floats(l_v4DiffSpec.y);\n"																								)
			cuT( "	l_v4Speculars.xy		= unpack2Floats(l_v4DiffSpec.z);\n"																								)
			cuT( "	l_v4Speculars.zw		= unpack2Floats(l_v4DiffSpec.w);\n"																								)
			cuT( "	l_v4Tangents.xy			= unpack2Floats(l_v4TanBiTan.x);\n"																								)
			cuT( "	l_v4Tangents.zw			= unpack2Floats(l_v4TanBiTan.y);\n"																								)
			cuT( "	l_v4Bitangents.xy		= unpack2Floats(l_v4TanBiTan.z);\n"																								)
			cuT( "	l_v4Bitangents.zw		= unpack2Floats(l_v4TanBiTan.w);\n"																								)
			cuT( "	float3	l_v3Binormal	= l_v4Bitangents.xyz;\n"																										)
			cuT( "	float	l_fInvRadius	= 0.02;\n"																														)
			cuT( "	float3	l_v3Specular	= float3( 0, 0, 0 );\n"																											)
			cuT( "	float3	l_v3Diffuse		= float3( 0, 0, 0 );\n"																											)
			cuT( "	float3	l_v3TmpVec		= -l_v4Positions.xyz;\n"																										)
			cuT( "	float3	l_v3EyeVec;\n"																																	)
			cuT( "	float3	l_v3HalfVector;\n"																																)
			cuT( "	float3	l_v3LightDir;\n"																																)
			cuT( "	float	l_fLambert;\n"																																	)
			cuT( "	float	l_fSpecular;\n"																																	)
			cuT( "	float	l_fSqrLength;\n"																																)
			cuT( "	float	l_fAttenuation;\n"																																)
			cuT( "	float3	l_v3TmpDiffuse;\n"																																)
			cuT( "	float3	l_v3TmpSpecular;\n"																																)
			cuT( "	l_v3EyeVec.x = dot( l_v3TmpVec, l_v4Tangents.xyz	);\n"																								)
			cuT( "	l_v3EyeVec.y = dot( l_v3TmpVec, l_v3Binormal		);\n"																								)
			cuT( "	l_v3EyeVec.z = dot( l_v3TmpVec, l_v4Normals.xyz		);\n"																								)
			cuT( "	l_v3EyeVec = normalize( l_v3EyeVec );\n"																												)
			cuT( "	for( int i = 0; i < c3d_iLightsCount; i++ )\n"																										)
			cuT( "	{\n"																																					)
			cuT( "		l_light			= GetLight( i );\n"																													)
			cuT( "		l_v3LightDir	= normalize( l_light.m_v4Position.xyz - l_v4Positions.xyz );\n"																		)
			cuT( "		l_v3LightDir 	= float3( dot( l_v3LightDir, l_v4Tangents.xyz ), dot( l_v3LightDir, l_v3Binormal	), dot( l_v3LightDir, l_v4Normals.xyz ) );\n"	)
			cuT( "		l_fSqrLength	= dot( l_v3LightDir, l_v3LightDir );\n"																								)
			cuT( "		l_v3LightDir	= l_v3LightDir * rsqrt( l_fSqrLength );\n"																							)
			cuT( "		l_fAttenuation	= clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n"																	)
			cuT( "		l_fLambert		= max( dot( l_v4Normals.xyz, l_v3LightDir ), 0.0 );\n"																				)
			cuT( "		l_fSpecular		= pow( clamp( dot( reflect( -l_v3LightDir, l_v4Normals.xyz ), l_v3EyeVec ), 0.0, 1.0 ), 10.0 );\n"									)
			cuT( "		l_v3TmpDiffuse	= l_light.m_v4Diffuse.xyz	* l_v4Diffuses.xyz	* l_fLambert;\n"																	)
			cuT( "		l_v3TmpSpecular	= l_light.m_v4Specular.xyz	* l_v4Speculars.xyz	* l_fSpecular;\n"																	)
			cuT( "		l_v3Diffuse		+= l_v3TmpDiffuse * l_fAttenuation;\n"																								)
			cuT( "		l_v3Specular	+= l_v3TmpSpecular * l_fAttenuation;\n"																								)
			cuT( "	}\n"																																					)
			cuT( "	l_output = float4( l_v3Diffuse + l_v3Specular, 1 );\n"																									)
			cuT( "//	l_output = float4( l_v4Tangents );\n"																												)
			cuT( "	return l_output;\n"																																		)
			cuT( "}\n"																																						)
			cuT( "technique RenderPass\n"																																		)
			cuT( "{\n"																																						)
			cuT( "	pass p0\n"																																				)
			cuT( "	{\n"																																					)
			cuT( "		CullMode=none;\n"																																	)
			cuT( "		PixelShader = compile ps_3_0 mainPx();\n"																											)
			cuT( "		VertexShader = compile vs_3_0 mainVx();\n"																											)
			cuT( "	}\n"																																					)
			cuT( "};\n"																																						);
	}
};

ShaderModel_1_2 g_shaderModel_1_2;
DeferredShaderModel_1_2 g_deferredShaderModel_1_2;

DxShaderProgram :: DxShaderProgram( DxRenderSystem * p_pRenderSystem )
	:	ShaderProgramBase	( p_pRenderSystem, eSHADER_LANGUAGE_HLSL	)
	,	m_pEffect			( NULL										)
{
 	CreateObject( eSHADER_TYPE_VERTEX	)->SetEntryPoint( cuT( "mainVx" ) );
	CreateObject( eSHADER_TYPE_PIXEL	)->SetEntryPoint( cuT( "mainPx" ) );
}

DxShaderProgram :: ~DxShaderProgram()
{
	Cleanup();
}

void DxShaderProgram :: RetrieveLinkerLog( String & strLog)
{
	if ( ! m_pRenderSystem->UseShaders())
	{
		strLog = DirectX9::GetHlslErrorString( 0);
		return;
	}
}

void DxShaderProgram :: Initialise()
{
	if ( m_eStatus == ePROGRAM_STATUS_NOTLINKED )
	{
		if( m_pRenderSystem->UseShaders() )
		{
			bool l_bResult = true;
			DxRenderSystem * l_pRS = static_cast< DxRenderSystem * >( GetRenderSystem() );
			std::string l_strCombinedSource;

			// First we retrieve the shaders source to combine then into one complete source
			for( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT && l_bResult; i++ )
			{
				if( m_pShaders[i] && m_pShaders[i]->HasSource() )
				{
					std::string l_strSource;

					for( int j = eSHADER_MODEL_5; j >= eSHADER_MODEL_1 && l_strSource.empty(); j-- )
					{
						if( l_pRS->CheckSupport( eSHADER_MODEL( j ) ) )
						{
							l_strSource = str_utils::to_str( m_pShaders[i]->GetSource( eSHADER_MODEL( j ) ) );
						}
					}

					if( !l_strSource.empty( ) && l_strSource != l_strCombinedSource.substr( 0, l_strCombinedSource.size() - 1 ) )
					{
						l_strCombinedSource += l_strSource;
						l_strCombinedSource += "\n";
					}
				}
			}

			// Then we compile that source to retreieve the effect
			if( !l_strCombinedSource.empty() )
			{
				ID3DXBuffer * l_pErrors = NULL;
				DWORD l_dwFlags = 0;
#if !defined( NDEBUG )
				l_dwFlags = D3DXSHADER_DEBUG;
#endif
				if( D3DXCreateEffect( l_pRS->GetDevice( ), l_strCombinedSource.c_str( ), UINT( l_strCombinedSource.size() ), NULL, NULL, l_dwFlags, NULL, &m_pEffect, &l_pErrors ) != S_OK )
				{
					if( l_pErrors ) 
					{
						Logger::LogMessage( str_utils::from_str( reinterpret_cast< char * >( l_pErrors->GetBufferPointer() ) ) );
						SafeRelease( l_pErrors );
						l_bResult = false;
					}
				}
				else
				{
					Logger::LogMessage( cuT( "Shader compiled successfully" ) );
				}
			}
			else
			{
				l_bResult = false;
			}
			
			// Eventually we retrieve shader objects specifics from the effect
			if( l_bResult )
			{
				for( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT && l_bResult; i++ )
				{
					if( m_pShaders[i] && m_pShaders[i]->HasSource() )
					{
						m_pShaders[i]->DestroyProgram();
						m_pShaders[i]->CreateProgram();

						if ( !m_pShaders[i]->Compile() && m_pShaders[i]->GetStatus() == eSHADER_STATUS_ERROR )
						{
							Logger::LogError( cuT( "ShaderProgram :: Initialise - " ) + m_pShaders[i]->GetCurrentFile() + cuT( " - COMPILER ERROR" ), false );
							m_pShaders[i]->DestroyProgram();
							m_eStatus = ePROGRAM_STATUS_ERROR;
							l_bResult = false;
						}
					}
				}
			}

			if( l_bResult )
			{
				if ( !Link() )
				{
					Logger::LogError( cuT( "ShaderProgram :: Initialise - LINKER ERROR" ) );

					for (int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++)
					{
						if( m_pShaders[i] )
						{
							m_pShaders[i]->DestroyProgram();
						}
					}

					m_eStatus = ePROGRAM_STATUS_ERROR;
				}
				else
				{
					m_pLightsTexture->Initialise();
					Logger::LogMessage( cuT( "ShaderProgram :: Initialise - Program Linked successfully" ) );
				}
			}
		}
	}
}

void DxShaderProgram :: Cleanup()
{
	ShaderProgramBase::Cleanup();
	SafeRelease( m_pEffect );
}

void DxShaderProgram :: Begin( uint8_t p_byIndex, uint8_t p_byCount )
{
	m_pRenderSystem->GetPipeline()->UpdateFunctions( this );

	if( m_pEffect )
	{
		m_byIndex = p_byIndex;

		if( m_byIndex == 0 )
		{
			UINT l_uiCount = p_byCount;
			m_pEffect->SetTechnique( "RenderPass" );
			m_pEffect->Begin( &l_uiCount, 0 );
		}

		m_pEffect->BeginPass( p_byIndex );
	}

	ShaderProgramBase::Begin( p_byIndex, p_byCount );
}

void DxShaderProgram :: End()
{
	ShaderProgramBase::End();

	if( m_pEffect )
	{
		m_pEffect->EndPass();

		if( m_byIndex == 0 )
		{
			m_pEffect->End();
		}
	}
}

void DxShaderProgram :: SetCameraPos( Castor::Point3f const & p_ptPosition )
{
	if( m_pEffect )
	{
		D3DXVECTOR4 l_vCamera;
		l_vCamera.x = p_ptPosition[0];
		l_vCamera.y = p_ptPosition[1];
		l_vCamera.z = p_ptPosition[2];
		l_vCamera.w = 1;
		m_pEffect->SetVector( StrCameraPos.c_str(), &l_vCamera );
	}
}

void DxShaderProgram :: SetAmbientLight( Castor::Colour const & p_crColour )
{
	if( m_pEffect )
	{
		D3DXVECTOR4 l_vColour;
		l_vColour.x = p_crColour.red().value();
		l_vColour.y = p_crColour.green().value();
		l_vColour.z = p_crColour.blue().value();
		l_vColour.w = p_crColour.alpha().value();
		m_pEffect->SetVector( StrAmbientLight.c_str(), &l_vColour );
	}
}

void DxShaderProgram :: SetMaterialAmbient( Castor::Colour const & p_crColour )
{
	if( m_pEffect )
	{
		D3DXVECTOR4 l_vColour;
		l_vColour.x = p_crColour.red().value();
		l_vColour.y = p_crColour.green().value();
		l_vColour.z = p_crColour.blue().value();
		l_vColour.w = p_crColour.alpha().value();
		m_pEffect->SetVector( StrMatAmbient.c_str(), &l_vColour );
	}
}

void DxShaderProgram :: SetMaterialDiffuse( Castor::Colour const & p_crColour )
{
	if( m_pEffect )
	{
		D3DXVECTOR4 l_vColour;
		l_vColour.x = p_crColour.red().value();
		l_vColour.y = p_crColour.green().value();
		l_vColour.z = p_crColour.blue().value();
		l_vColour.w = p_crColour.alpha().value();
		m_pEffect->SetVector( StrMatDiffuse.c_str(), &l_vColour );
	}
}

void DxShaderProgram :: SetMaterialSpecular( Castor::Colour const & p_crColour )
{
	if( m_pEffect )
	{
		D3DXVECTOR4 l_vColour;
		l_vColour.x = p_crColour.red().value();
		l_vColour.y = p_crColour.green().value();
		l_vColour.z = p_crColour.blue().value();
		l_vColour.w = p_crColour.alpha().value();
		m_pEffect->SetVector( StrMatSpecular.c_str(), &l_vColour );
	}
}

void DxShaderProgram :: SetMaterialEmissive( Castor::Colour const & p_crColour )
{
	if( m_pEffect )
	{
		D3DXVECTOR4 l_vColour;
		l_vColour.x = p_crColour.red().value();
		l_vColour.y = p_crColour.green().value();
		l_vColour.z = p_crColour.blue().value();
		l_vColour.w = p_crColour.alpha().value();
		m_pEffect->SetVector( StrMatEmissive.c_str(), &l_vColour );
	}
}

void DxShaderProgram :: SetMaterialShininess( float p_fShine )
{
	if( m_pEffect )
	{
		m_pEffect->SetFloat( StrMatShininess.c_str(), p_fShine );
	}
}

void DxShaderProgram :: SetMaterialOpacity( float p_fOpacity )
{
	if( m_pEffect )
	{
		m_pEffect->SetFloat( StrMatOpacity.c_str(), p_fOpacity );
	}
}

String DxShaderProgram :: DoGetVertexShaderSource( uint32_t p_uiProgramFlags )
{
	String	l_strReturn;

	l_strReturn = g_shaderModel_1_2.GetVertexProgram();

	return l_strReturn;
}

String DxShaderProgram :: DoGetPixelShaderSource( uint32_t p_uiFlags )
{
	String	l_strReturn;
	String	l_strIndex;

	String l_strPixelOutput;
	String l_strPixelDeclarations;
	String l_strPixelLights;
	String l_strPixelMainDeclarations;
	String l_strPixelMainLightsLoop;
	String l_strPixelMainLightsLoopAfterLightDir;
	String l_strPixelMainLightsLoopEnd;
	String l_strPixelMainEnd;

	l_strPixelOutput						= g_shaderModel_1_2.GetPixelOutput();
	l_strPixelDeclarations					= g_shaderModel_1_2.GetPixelDeclarations();
	l_strPixelLights						= g_shaderModel_1_2.GetPixelLights();
	l_strPixelMainDeclarations				= g_shaderModel_1_2.GetPixelMainDeclarations();
	l_strPixelMainLightsLoop				= g_shaderModel_1_2.GetPixelMainLightsLoop();
	l_strPixelMainLightsLoopEnd				= g_shaderModel_1_2.GetPixelMainLightsLoopEnd();
	l_strPixelMainEnd						= g_shaderModel_1_2.GetPixelMainEnd();
	l_strPixelMainLightsLoopAfterLightDir	= g_shaderModel_1_2.GetPixelMainLightsLoopAfterLightDir();

	if( p_uiFlags != 0 )
	{
		int l_iIndex = 1;

		if( (p_uiFlags & eTEXTURE_CHANNEL_COLOUR) == eTEXTURE_CHANNEL_COLOUR )
		{
			l_strIndex.clear();
			l_strIndex << l_iIndex++;
			l_strPixelDeclarations						+= cuT( "texture			c3d_mapColour;\n" );
			l_strPixelDeclarations						+= cuT( "sampler ColourTextureSampler =\n" );
			l_strPixelDeclarations						+= cuT( "sampler_state\n" );
			l_strPixelDeclarations						+= cuT( "{\n" );
			l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapColour >;\n" );
			l_strPixelDeclarations						+= cuT( "};\n" );
			l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapColour		= tex2D( ColourTextureSampler, p_input.TextureUV );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Ambient		*= l_v4MapColour;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_AMBIENT) == eTEXTURE_CHANNEL_AMBIENT )
		{
			l_strIndex.clear();
			l_strIndex << l_iIndex++;
			l_strPixelDeclarations						+= cuT( "texture		c3d_mapAmbient;\n" );
			l_strPixelDeclarations						+= cuT( "sampler AmbientTextureSampler =\n" );
			l_strPixelDeclarations						+= cuT( "sampler_state\n" );
			l_strPixelDeclarations						+= cuT( "{\n" );
			l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapAmbient >;\n" );
			l_strPixelDeclarations						+= cuT( "};\n" );
			l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapAmbient		= tex2D( AmbientTextureSampler, p_input.TextureUV );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Ambient		*= l_v4MapAmbient;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE) == eTEXTURE_CHANNEL_DIFFUSE )
		{
			l_strIndex.clear();
			l_strIndex << l_iIndex++;
			l_strPixelDeclarations						+= cuT( "texture			c3d_mapDiffuse;\n" );
			l_strPixelDeclarations						+= cuT( "sampler DiffuseTextureSampler =\n" );
			l_strPixelDeclarations						+= cuT( "sampler_state\n" );
			l_strPixelDeclarations						+= cuT( "{\n" );
			l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapDiffuse >;\n" );
			l_strPixelDeclarations						+= cuT( "};\n" );
			l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapDiffuse		= tex2D( DiffuseTextureSampler, p_input.TextureUV );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Ambient		*= l_v4MapDiffuse;\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Diffuse		*= l_v4MapDiffuse;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_NORMAL) == eTEXTURE_CHANNEL_NORMAL )
		{
			l_strIndex.clear();
			l_strIndex << l_iIndex++;
			l_strPixelDeclarations						+= cuT( "texture			c3d_mapNormal;\n" );
			l_strPixelDeclarations						+= cuT( "sampler NormalTextureSampler =\n" );
			l_strPixelDeclarations						+= cuT( "sampler_state\n" );
			l_strPixelDeclarations						+= cuT( "{\n" );
			l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapNormal >;\n" );
			l_strPixelDeclarations						+= cuT( "};\n" );
			l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapNormal		= tex2D( NormalTextureSampler, p_input.TextureUV );\n" );
			l_strPixelMainDeclarations					+= cuT( "	float 	l_fSqrLength;\n" );
			l_strPixelMainDeclarations					+= cuT( "	float 	l_fAttenuation;\n" );
			l_strPixelMainDeclarations					+= cuT( "	float	l_fInvRadius		= 0.02;\n" );
			l_strPixelMainDeclarations					+= cuT( "	l_v3Normal					= normalize( l_v4MapNormal.xyz * 2.0 - 1.0 );\n" );
			l_strPixelMainLightsLoop					+= cuT( "		l_v3LightDir 	= float3( dot( l_v3LightDir, p_input.Tangent ), dot( l_v3LightDir, p_input.Binormal	), dot( l_v3LightDir, p_input.Normal ) );\n" );
			l_strPixelMainLightsLoop					+= cuT( "		l_fSqrLength	= dot( l_v3LightDir, l_v3LightDir );\n" );
			l_strPixelMainLightsLoop					+= cuT( "		l_v3LightDir	= l_v3LightDir * rsqrt( l_fSqrLength );\n" );
			l_strPixelMainLightsLoop					+= cuT( "		l_fAttenuation	= clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n" );
			l_strPixelMainLightsLoopAfterLightDir		+= cuT( "		l_v4Ambient		*= l_fAttenuation;\n" );
			l_strPixelMainLightsLoopAfterLightDir		+= cuT( "		l_v4Diffuse		*= l_fAttenuation;\n" );
			l_strPixelMainLightsLoopAfterLightDir		+= cuT( "		l_v4Specular	*= l_fAttenuation;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_OPACITY) == eTEXTURE_CHANNEL_OPACITY )
		{
			l_strIndex.clear();
			l_strIndex << l_iIndex++;
			l_strPixelDeclarations						+= cuT( "texture			c3d_mapOpacity;\n" );
			l_strPixelDeclarations						+= cuT( "sampler OpacityTextureSampler =\n" );
			l_strPixelDeclarations						+= cuT( "sampler_state\n" );
			l_strPixelDeclarations						+= cuT( "{\n" );
			l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapOpacity >;\n" );
			l_strPixelDeclarations						+= cuT( "};\n" );
			l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapOpacity		= tex2D( OpacityTextureSampler, p_input.TextureUV );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_fAlpha		= l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_SPECULAR) == eTEXTURE_CHANNEL_SPECULAR )
		{
			l_strIndex.clear();
			l_strIndex << l_iIndex++;
			l_strPixelDeclarations						+= cuT( "texture			c3d_mapSpecular;\n" );
			l_strPixelDeclarations						+= cuT( "sampler SpecularTextureSampler =\n" );
			l_strPixelDeclarations						+= cuT( "sampler_state\n" );
			l_strPixelDeclarations						+= cuT( "{\n" );
			l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapSpecular >;\n" );
			l_strPixelDeclarations						+= cuT( "};\n" );
			l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapSpecular		= tex2D( SpecularTextureSampler, p_input.TextureUV );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Specular	*= l_v4MapSpecular;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_HEIGHT) == eTEXTURE_CHANNEL_HEIGHT )
		{
			l_strIndex.clear();
			l_strIndex << l_iIndex++;
			l_strPixelDeclarations						+= cuT( "texture			c3d_mapHeight;\n" );
			l_strPixelDeclarations						+= cuT( "sampler HeightTextureSampler =\n" );
			l_strPixelDeclarations						+= cuT( "sampler_state\n" );
			l_strPixelDeclarations						+= cuT( "{\n" );
			l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapHeight >;\n" );
			l_strPixelDeclarations						+= cuT( "};\n" );
			l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapHeight		= tex2D( HeightTextureSampler, p_input.TextureUV );\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_GLOSS) == eTEXTURE_CHANNEL_GLOSS )
		{
			l_strIndex.clear();
			l_strIndex << l_iIndex++;
			l_strPixelDeclarations						+= cuT( "texture			c3d_mapGloss;\n" );
			l_strPixelDeclarations						+= cuT( "sampler GlossTextureSampler =\n" );
			l_strPixelDeclarations						+= cuT( "sampler_state\n" );
			l_strPixelDeclarations						+= cuT( "{\n" );
			l_strPixelDeclarations						+= cuT( "	Texture = < c3d_mapGloss >;\n" );
			l_strPixelDeclarations						+= cuT( "};\n" );
			l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapGloss		= tex2D( GlossTextureSampler, p_input.TextureUV );\n" );
		}
	}

	l_strPixelMainLightsLoopEnd	+= cuT( "	" ) + l_strPixelOutput + cuT( " = float4( (l_v4Ambient + l_v4Diffuse + l_v4Specular).xyz, l_fAlpha );\n" );

	l_strReturn = l_strPixelDeclarations + l_strPixelLights + l_strPixelMainDeclarations + l_strPixelMainLightsLoop + l_strPixelMainLightsLoopAfterLightDir + l_strPixelMainLightsLoopEnd + l_strPixelMainEnd;
//	Logger::LogDebug( l_strReturn );

	return l_strReturn;
}

String DxShaderProgram :: DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass )
{
	String	l_strReturn;
	String	l_strIndex;

	String l_strPixelOutput;
	String l_strVertex;
	String l_strPixelDeclarations;
	String l_strPixelMainDeclarations;
	String l_strPixelMainLightsLoopAfterLightDir;
	String l_strPixelMainLightsLoopEnd;
	String l_strPixelMainEnd;

	if( p_bLightPass )
	{
		l_strReturn = g_deferredShaderModel_1_2.GetLSVertexProgram();
	}
	else
	{
		l_strVertex = g_deferredShaderModel_1_2.GetGSVertexProgram();
		l_strReturn = l_strVertex;
//		Logger::LogDebug( l_strReturn );
	}

	return l_strReturn;
}

String DxShaderProgram :: DoGetDeferredPixelShaderSource( uint32_t p_uiFlags )
{
	String	l_strReturn;
	String	l_strIndex;

	String l_strPixelOutput;
	String l_strVertex;
	String l_strPixelDeclarations;
	String l_strPixelMainDeclarations;
	String l_strPixelMainLightsLoopAfterLightDir;
	String l_strPixelMainLightsLoopEnd;
	String l_strPixelMainEnd;

	if( (p_uiFlags & eTEXTURE_CHANNEL_LGHTPASS) == eTEXTURE_CHANNEL_LGHTPASS )
	{
		l_strReturn += g_deferredShaderModel_1_2.GetLSPixelProgram();
	}
	else
	{
		l_strPixelDeclarations					= g_deferredShaderModel_1_2.GetGSPixelDeclarations();
		l_strPixelMainDeclarations				= g_deferredShaderModel_1_2.GetGSPixelMainDeclarations();
		l_strPixelMainLightsLoopAfterLightDir	= g_deferredShaderModel_1_2.GetGSPixelMainLightsLoopAfterLightDir();
		l_strPixelMainEnd						= g_deferredShaderModel_1_2.GetGSPixelMainEnd();

		if( p_uiFlags != 0 )
		{
			int l_iIndex = 1;

			if( (p_uiFlags & eTEXTURE_CHANNEL_COLOUR) == eTEXTURE_CHANNEL_COLOUR )
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
				l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapColour		= tex2D( ColourTextureSampler, p_input.TextureUV );\n" );
				l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Diffuse		*= l_v4MapColour;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_AMBIENT) == eTEXTURE_CHANNEL_AMBIENT )
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
				l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapAmbient		= tex2D( AmbientTextureSampler, p_input.TextureUV );\n" );
				l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Diffuse		*= l_v4MapAmbient;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE) == eTEXTURE_CHANNEL_DIFFUSE )
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
				l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapDiffuse		= tex2D( DiffuseTextureSampler, p_input.TextureUV );\n" );
				l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Diffuse		*= l_v4MapDiffuse;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_NORMAL) == eTEXTURE_CHANNEL_NORMAL )
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
				l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapNormal		= tex2D( NormalTextureSampler, p_input.TextureUV );\n" );
				l_strPixelMainDeclarations					+= cuT( "	l_v4Normal					+= float4( normalize( (l_v4MapNormal.xyz * 2.0 - 1.0) ), 0 );\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_OPACITY) == eTEXTURE_CHANNEL_OPACITY )
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
				l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapOpacity		= tex2D( OpacityTextureSampler, p_input.TextureUV );\n" );
				l_strPixelMainLightsLoopEnd					+= cuT( "	l_fAlpha		= l_v4MapOpacity.r * l_v4MapOpacity.a;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_SPECULAR) == eTEXTURE_CHANNEL_SPECULAR )
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
				l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapSpecular		= tex2D( SpecularTextureSampler, p_input.TextureUV );\n" );
				l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Specular	*= l_v4MapSpecular;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_HEIGHT) == eTEXTURE_CHANNEL_HEIGHT )
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
				l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapHeight		= tex2D( HeightTextureSampler, p_input.TextureUV );\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_GLOSS) == eTEXTURE_CHANNEL_GLOSS )
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
				l_strPixelMainDeclarations					+= cuT( "	float4	l_v4MapGloss		= tex2D( GlossTextureSampler, p_input.TextureUV );\n" );
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
//		Logger::LogDebug( l_strReturn );
	}

	return l_strReturn;
}

ShaderObjectBaseSPtr DxShaderProgram :: DoCreateObject( eSHADER_TYPE p_eType )
{
	ShaderObjectBaseSPtr l_pReturn;

	switch (p_eType)
	{
	case eSHADER_TYPE_VERTEX:
		l_pReturn = std::make_shared< DxVertexShader >( this );
		break;

	case eSHADER_TYPE_PIXEL:
		l_pReturn = std::make_shared< DxFragmentShader >( this );
		break;
	}

	return l_pReturn;
}

std::shared_ptr< OneTextureFrameVariable > DxShaderProgram :: DoCreateTextureVariable( int p_iNbOcc )
{
	return std::make_shared< DxOneFrameVariable< TextureBaseRPtr > >( static_cast< DxRenderSystem * >( m_pRenderSystem ), this, p_iNbOcc );
}

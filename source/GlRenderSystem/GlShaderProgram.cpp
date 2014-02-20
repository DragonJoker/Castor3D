#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/GlShaderObject.hpp"
#include "GlRenderSystem/GlFrameVariable.hpp"
#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/GlLightRenderer.hpp"
#include "GlRenderSystem/GlFrameVariable.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlTextureRenderer.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

namespace GLSL
{
	class VariablesBase
	{
	public:
		virtual String GetVertexOutMatrices()const=0;
		virtual String GetVertexMatrixCopy()const=0;
		virtual String GetPixelInMatrices()const=0;
		virtual String GetPixelMtxModelView()const=0;
	};

	class VariablesUbo : public VariablesBase
	{
	public:
		virtual String GetVertexOutMatrices()const
		{
			return cuT( "" );
		}

		virtual String GetVertexMatrixCopy()const
		{
			return cuT( "" );
		}

		virtual String GetPixelInMatrices()const
		{
			return GlRender::GLSL::constantsUbo.Matrices();
		}

		virtual String GetPixelMtxModelView()const
		{
			return cuT( "c3d_mtxModelView" );
		}
	};

	class VariablesStd : public VariablesBase
	{
	public:
		virtual String GetVertexOutMatrices()const
		{
			return
				cuT( "out	mat4 	vtx_mtxModelView;\n"				)
				cuT( "out	mat4 	vtx_mtxView;\n"						);
		}

		virtual String GetVertexMatrixCopy()const
		{
			return
				cuT( "	vtx_mtxModelView = c3d_mtxModelView;\n"			)
				cuT( "	vtx_mtxView = c3d_mtxView;\n"					);
		}

		virtual String GetPixelInMatrices()const
		{
			return
				cuT( "in	mat4 	vtx_mtxModelView;\n"				)
				cuT( "in	mat4 	vtx_mtxView;\n"						);
		}

		virtual String GetPixelMtxModelView()const
		{
			return cuT( "vtx_mtxModelView" );
		}
	};

	VariablesUbo variablesUbo;
	VariablesStd variablesStd;
}

class ShaderSource
{
private:
	String m_strPixelLights;

public:
	ShaderSource()
	{
		m_strPixelLights =
			cuT(	"struct Light\n"																													)
			cuT(	"{\n"																																)
			cuT(	"	vec4	m_v4Ambient;\n"																											)
			cuT(	"	vec4	m_v4Diffuse;\n"																											)
			cuT(	"	vec4	m_v4Specular;\n"																										)
			cuT(	"	vec4	m_v4Position;\n"																										)
			cuT(	"	int		m_iType;\n"																												)
			cuT(	"	vec3	m_v3Attenuation;\n"																										)
			cuT(	"	mat4	m_mtx4Orientation;\n"																									)
			cuT(	"	float	m_fExponent;\n"																											)
			cuT(	"	float	m_fCutOff;\n"																											)
			cuT(	"};\n"																																)
			cuT(	"Light GetLight( int p_iIndex )\n"																									)
			cuT(	"{\n"																																)
			cuT(	"	Light	l_lightReturn;\n"																										)
//			cuT(	"	float l_fFactor = (float( p_iIndex ) * 0.1);\n"																					)
// 			cuT(	"	l_lightReturn.m_v4Ambient		= texture2D( c3d_sLights, vec2( l_fFactor + 0.005, 0.0 ) );\n"									)
// 			cuT(	"	l_lightReturn.m_v4Diffuse		= texture2D( c3d_sLights, vec2( l_fFactor + 0.015, 0.0 ) );\n"									)
// 			cuT(	"	l_lightReturn.m_v4Specular		= texture2D( c3d_sLights, vec2( l_fFactor + 0.025, 0.0 ) );\n"									)
// 			cuT(	"	vec4	l_v4Position			= texture2D( c3d_sLights, vec2( l_fFactor + 0.035, 0.0 ) );\n"									)
// 			cuT(	"	l_lightReturn.m_v3Attenuation	= texture2D( c3d_sLights, vec2( l_fFactor + 0.045, 0.0 ) ).xyz;\n"								)
// 			cuT(	"	vec4	l_v4A					= texture2D( c3d_sLights, vec2( l_fFactor + 0.055, 0.0 ) );\n"									)
// 			cuT(	"	vec4	l_v4B					= texture2D( c3d_sLights, vec2( l_fFactor + 0.065, 0.0 ) );\n"									)
// 			cuT(	"	vec4	l_v4C					= texture2D( c3d_sLights, vec2( l_fFactor + 0.075, 0.0 ) );\n"									)
// 			cuT(	"	vec4	l_v4D					= texture2D( c3d_sLights, vec2( l_fFactor + 0.085, 0.0 ) );\n"									)
// 			cuT(	"	vec2	l_v2Spot				= texture2D( c3d_sLights, vec2( l_fFactor + 0.095, 0.0 ) ).xy;\n"								)
 			cuT(	"	float l_fFactor = (float( p_iIndex ) * 0.01);\n"																				)
			cuT(	"	l_lightReturn.m_v4Ambient		= texture( c3d_sLights, l_fFactor + (0 * 0.001) + 0.0005 );\n"									)
			cuT(	"	l_lightReturn.m_v4Diffuse		= texture( c3d_sLights, l_fFactor + (1 * 0.001) + 0.0005 );\n"									)
			cuT(	"	l_lightReturn.m_v4Specular		= texture( c3d_sLights, l_fFactor + (2 * 0.001) + 0.0005 );\n"									)
			cuT(	"	vec4	l_v4Position			= texture( c3d_sLights, l_fFactor + (3 * 0.001) + 0.0005 );\n"									)
			cuT(	"	l_lightReturn.m_v3Attenuation	= texture( c3d_sLights, l_fFactor + (4 * 0.001) + 0.0005 ).xyz;\n"								)
			cuT(	"	vec4	l_v4A					= texture( c3d_sLights, l_fFactor + (5 * 0.001) + 0.0005 );\n"									)
			cuT(	"	vec4	l_v4B					= texture( c3d_sLights, l_fFactor + (6 * 0.001) + 0.0005 );\n"									)
			cuT(	"	vec4	l_v4C					= texture( c3d_sLights, l_fFactor + (7 * 0.001) + 0.0005 );\n"									)
			cuT(	"	vec4	l_v4D					= texture( c3d_sLights, l_fFactor + (8 * 0.001) + 0.0005 );\n"									)
			cuT(	"	vec2	l_v2Spot				= texture( c3d_sLights, l_fFactor + (9 * 0.001) + 0.0005 ).xy;\n"								)
			cuT(	"	l_lightReturn.m_v4Position		= vec4( l_v4Position.z, l_v4Position.y, l_v4Position.x, 0.0 );\n"								)
			cuT(	"	l_lightReturn.m_iType			= int( l_v4Position.w );\n"																		)
			cuT(	"	l_lightReturn.m_mtx4Orientation	= mat4( l_v4A, l_v4B, l_v4C, l_v4D );\n"														)
			cuT(	"	l_lightReturn.m_fExponent		= l_v2Spot.x;\n"																				)
			cuT(	"	l_lightReturn.m_fCutOff			= l_v2Spot.x;\n"																				)
			cuT(	"	return l_lightReturn;\n"																										)
			cuT(	"}\n"																																)
			cuT(	"vec4 BlinnPhongDir( in Light p_light, in vec3 p_vertex, in mat4 p_mtxModelView )\n"												)
			cuT(	"{\n"																																)
			cuT(	"//	vec4 l_v4Return = vec4( normalize( p_mtxModelView * -p_light.m_v4Position).xyz, 1.0 );\n"										)
			cuT(	"//	vec4 l_v4Return = vec4( normalize( p_vertex - (p_mtxModelView * p_light.m_v4Position).xyz ), 1.0 );\n"							)
			cuT(	"	vec4 l_v4Return = vec4( normalize( (p_mtxModelView * p_light.m_v4Position).xyz - p_vertex ), 1.0 );\n"							)
			cuT(	"	if( 0 != p_light.m_iType ) // non directional light?\n"																			)
			cuT(	"	{\n"																															)
			cuT(	"		float l_fDistance = distance( p_light.m_v4Position.xyz, p_vertex );\n"														)
			cuT(	"		float l_fAtt = p_light.m_v3Attenuation.x "																					)
			cuT(							"+ l_fDistance * (p_light.m_v3Attenuation.y + l_fDistance * p_light.m_v3Attenuation.z);\n"					)
			cuT(	"		l_v4Return.w = 1.0 / l_fAtt;\n"																								)
			cuT(	"	}\n"																															)
			cuT(	"	return l_v4Return;\n"																											)
			cuT(	"}\n"																																)
			cuT(	"vec2 Fresnel( in float p_fLambert, in vec3 p_v3LightDir, in vec3 p_v3Normal, in vec3 p_v3EyeVec, inout vec4 p_v4MatSpecular )\n"	)
			cuT(	"{\n"																																)
			cuT(	"	vec2 l_v2Return = vec2( 1.0, p_fLambert );\n"																					)
			cuT(	"	if( p_fLambert <= 0.0 ) // light source on the wrong side?\n"																	)
			cuT(	"	{\n"																															)
			cuT(	"		l_v2Return.x = 0.0; // no specular reflection\n"																			)
			cuT(	"		l_v2Return.y = 0.0;\n"																										)
			cuT(	"	}\n"																															)
			cuT(	"	else // light source on the right side\n"																						)
			cuT(	"	{\n"																															)
			cuT(	"		vec3 l_v3LightReflect = reflect( -p_v3LightDir, p_v3Normal );\n"															)
			cuT(	"//		vec3 l_v3LightReflect = reflect( p_v3LightDir, p_v3Normal );\n"																)
			cuT(	"		l_v2Return.x = pow( clamp( dot( p_v3EyeVec, l_v3LightReflect ), 0.0, 1.0 ), c3d_fMatShininess );\n"							)
			cuT(	"		float l_fFresnel = pow( 1.0 - clamp( dot( normalize( p_v3LightDir + p_v3EyeVec ), p_v3EyeVec ), 0.0, 1.0 ), 5.0 );\n"		)
			cuT(	"		p_v4MatSpecular = vec4( clamp( mix( vec3( c3d_v4MatSpecular ), vec3( 1.0 ), l_fFresnel ), 0.0, 1.0 ), 1.0 );\n"				)
			cuT(	"	}\n"																															)
			cuT(	"	return l_v2Return;\n"																											)
			cuT(	"}\n"																																)
			cuT(	"void Bump( in vec3 p_v3T, in vec3 p_v3B, in vec3 p_v3N, inout vec3 p_v3LightDir, inout float p_fAttenuation )\n"					)
			cuT(	"{\n"																																)
			cuT(	"	float l_fInvRadius = 0.02;\n"																									)
			cuT(	"	p_v3LightDir = vec3( dot( p_v3LightDir, p_v3T ), dot( p_v3LightDir, p_v3B ), dot( p_v3LightDir, p_v3N ) );\n"					)
			cuT(	"	float l_fSqrLength = dot( p_v3LightDir, p_v3LightDir );\n"																		)
			cuT(	"	p_v3LightDir = p_v3LightDir * inversesqrt( l_fSqrLength );\n"																	)
			cuT(	"	p_fAttenuation *= clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n"												)
			cuT(	"}\n"																																);
	}

	inline String const & GetPixelLights()const { return m_strPixelLights; }
};

class PixelShaderSource : public ShaderSource
{
protected:
	String m_strPixelMainDeclarations				;
	String m_strPixelMainLightsLoop					;
	String m_strPixelMainLightsLoopAfterLightDir	;
	String m_strPixelMainLightsLoopEnd				;
	String m_strPixelMainEnd						;

public:
	inline String const & GetPixelMainDeclarations()const				{ return m_strPixelMainDeclarations;			}
	inline String const & GetPixelMainLightsLoop()const					{ return m_strPixelMainLightsLoop;				}
	inline String const & GetPixelMainLightsLoopAfterLightDir()const	{ return m_strPixelMainLightsLoopAfterLightDir;	}
	inline String const & GetPixelMainLightsLoopEnd()const				{ return m_strPixelMainLightsLoopEnd;			}
	inline String const & GetPixelMainEnd()const						{ return m_strPixelMainEnd;						}

	PixelShaderSource()
	{
		m_strPixelMainDeclarations =
			cuT(	"void main()\n"																									)
			cuT(	"{\n"																											)
			cuT(	"	Light	l_light;\n"																							)
			cuT(	"	vec3	l_v3LightDir;\n"																					)
			cuT(	"	float	l_fAttenuation;\n"																					)
			cuT(	"	float	l_fLambert;\n"																						)
			cuT(	"	float	l_fSpecular;\n"																						)
			cuT(	"	vec3	l_v3Normal			= normalize( vtx_normal );\n"													)
			cuT(	"	vec4	l_v4Ambient			= vec4( 0, 0, 0, 0 );\n"														)
			cuT(	"	vec4	l_v4Diffuse			= vec4( 0, 0, 0, 0 );\n"														)
			cuT(	"	vec4	l_v4Specular		= vec4( 0, 0, 0, 0 );\n"														)
			cuT(	"	vec4	l_v4TmpAmbient		= vec4( 0, 0, 0, 0 );\n"														)
			cuT(	"	vec4	l_v4TmpDiffuse		= vec4( 0, 0, 0, 0 );\n"														)
			cuT(	"	vec4	l_v4TmpSpecular		= vec4( 0, 0, 0, 0 );\n"														)
			cuT(	"	vec3	l_v3EyeVec			= normalize( c3d_v3CameraPosition - vtx_vertex );\n"							)
			cuT(	"	float	l_fAlpha			= c3d_fMatOpacity;\n"															)
			cuT(	"	vec4	l_v4MatSpecular		= c3d_v4MatSpecular;\n"															);

		m_strPixelMainLightsLoop =
			cuT(	"	for( int i = 0; i < c3d_iLightsCount; i++ )\n"															)
			cuT(	"	{\n"																										)
			cuT(	"		l_light = GetLight( i );\n"																				)
			cuT(	"		vec4 l_v4LightDir = BlinnPhongDir( l_light, vtx_vertex, <pxlin_mtxModelView> );\n"						)
			cuT(	"		l_v3LightDir = l_v4LightDir.xyz;\n"																		)
			cuT(	"		l_fAttenuation = l_v4LightDir.w;\n"																		);

		m_strPixelMainLightsLoopAfterLightDir =
			cuT(	"		l_fLambert = dot( l_v3Normal, l_v3LightDir );\n"														)
			cuT(	"		l_v4MatSpecular = c3d_v4MatSpecular;\n"																	)
			cuT(	"		vec2 l_v2SpecLamb = Fresnel( l_fLambert, l_v3LightDir, l_v3Normal, l_v3EyeVec, l_v4MatSpecular );\n"	)
			cuT(	"		l_fSpecular = l_v2SpecLamb.x;\n"																		)
			cuT(	"		l_fLambert = l_v2SpecLamb.y;\n"																			)
			cuT(	"		l_v4TmpAmbient	= l_light.m_v4Ambient * c3d_v4MatAmbient;\n"											)
			cuT(	"		l_v4TmpDiffuse	= l_fAttenuation * l_light.m_v4Diffuse * c3d_v4MatDiffuse * l_fLambert;\n"				)
			cuT(	"		l_v4TmpSpecular	= l_fAttenuation * l_light.m_v4Specular * l_v4MatSpecular * l_fSpecular;\n"				);

		m_strPixelMainLightsLoopEnd=
			cuT(	"		l_v4Ambient		+= l_v4TmpAmbient;\n"																	)
			cuT(	"		l_v4Diffuse		+= l_v4TmpDiffuse;\n"																	)
			cuT(	"		l_v4Specular	+= l_v4TmpSpecular;\n"																	)
			cuT(	"	}\n"																										);

		m_strPixelMainEnd =
			cuT(	"}\n"																											);
	}
};

class DeferredShaderSource : public ShaderSource
{
protected:
	String m_strGSPixelDeclarations					;
	String m_strGSPixelMainDeclarations				;
	String m_strGSPixelMainLightsLoopAfterLightDir	;
	String m_strGSPixelMainEnd						;
	String m_strLSPixelProgram						;

public:
	inline String const & GetGSPixelDeclarations()					{ return m_strGSPixelDeclarations;					}
	inline String const & GetGSPixelMainDeclarations()				{ return m_strGSPixelMainDeclarations;				}
	inline String const & GetGSPixelMainLightsLoopAfterLightDir()	{ return m_strGSPixelMainLightsLoopAfterLightDir;	}
	inline String const & GetGSPixelMainEnd()						{ return m_strGSPixelMainEnd;						}
	inline String const & GetLSPixelProgram()						{ return m_strLSPixelProgram;						}

	DeferredShaderSource()
	{
		m_strGSPixelDeclarations = cuT( ""																							);

		m_strGSPixelMainDeclarations =
			cuT(	"void main()\n"																									)
			cuT(	"{\n"																											)
			cuT(	"	vec4	l_v4Position		= vec4( vtx_vertex.xyz, 1 );\n"													)
			cuT(	"	vec4	l_v4Normal			= vec4( vtx_normal, 1 );\n"														)
			cuT(	"	vec4	l_v4Tangent			= vec4( vtx_tangent, 1 );\n"													)
			cuT(	"	vec4	l_v4Bitangent		= vec4( vtx_bitangent, 1 );\n"													)
			cuT(	"	vec4	l_v4Diffuse			= vec4( 0, 0, 0, 0 );\n"														)
			cuT(	"	vec4	l_v4Texture			= vec4( vtx_texture.xy, 0, 1 );\n"												)
			cuT(	"	vec4	l_v4Specular		= vec4( c3d_v4MatSpecular.xyz, c3d_fMatShininess );\n"							)
			cuT(	"	float	l_fAlpha			= c3d_fMatOpacity;\n"															);

		m_strGSPixelMainLightsLoopAfterLightDir =
			cuT(	"	l_v4Diffuse		= c3d_v4MatDiffuse;\n"																		);

		m_strGSPixelMainEnd =
			cuT(	"}\n"																											);

		m_strLSPixelProgram = GetPixelLights() +
			cuT(	"uniform	sampler2D	c3d_mapPosition;\n"																		)
			cuT(	"uniform	sampler2D	c3d_mapDiffuse;\n"																		)
			cuT(	"uniform	sampler2D	c3d_mapNormals;\n"																		)
			cuT(	"uniform	sampler2D	c3d_mapTangent;\n"																		)
			cuT(	"uniform	sampler2D	c3d_mapBitangent;\n"																	)
			cuT(	"uniform	sampler2D	c3d_mapSpecular;\n"																		)
			cuT(	"in		vec2		vtx_texture;\n"																				)
			cuT(	"void main( void )\n"																							)
			cuT(	"{\n"																											)
			cuT(	"	Light	l_light;\n"																							)
			cuT(	"	vec4	l_v4Positions	= texture2D( c3d_mapPosition,	vtx_texture );\n"									)
			cuT(	"	vec4	l_v4Diffuses	= texture2D( c3d_mapDiffuse,	vtx_texture );\n"									)
			cuT(	"	vec4	l_v4Normals		= texture2D( c3d_mapNormals,	vtx_texture ) / 2.0;\n"								)
			cuT(	"	vec4	l_v4Tangents	= texture2D( c3d_mapTangent,	vtx_texture );\n"									)
			cuT(	"	vec4	l_v4Bitangents	= texture2D( c3d_mapBitangent,	vtx_texture );\n"									)
			cuT(	"	vec4	l_v4Speculars	= texture2D( c3d_mapSpecular,	vtx_texture );\n"									)
			cuT(	"	float	l_fShininess	= l_v4Speculars.w;\n"																)
			cuT(	"	vec3	l_v3Binormal	= l_v4Bitangents.xyz;\n"															)
			cuT(	"	vec3	l_v3Specular	= vec3( 0, 0, 0 );\n"																)
			cuT(	"	vec3	l_v3Diffuse		= vec3( 0, 0, 0 );\n"																)
			cuT(	"	vec3	l_v3TmpVec		= -l_v4Positions.xyz;\n"															)
			cuT(	"	vec3	l_v3EyeVec;\n"																						)
			cuT(	"	vec3	l_v3HalfVector;\n"																					)
			cuT(	"	vec3	l_v3LightDir;\n"																					)
			cuT(	"	float	l_fLambert;\n"																						)
			cuT(	"	float	l_fSpecular;\n"																						)
			cuT(	"	float	l_fSqrLength;\n"																					)
			cuT(	"	float	l_fAttenuation;\n"																					)
			cuT(	"	vec3	l_v3TmpDiffuse;\n"																					)
			cuT(	"	vec3	l_v3TmpSpecular;\n"																					)
			cuT(	"	vec4	l_v4MatSpecular;\n"																					)
			cuT(	"	vec3	l_v3Normal = l_v4Normals.xyz;\n"																	)
			cuT(	"	l_v3EyeVec.x = dot( l_v3TmpVec, l_v4Tangents.xyz	);\n"													)
			cuT(	"	l_v3EyeVec.y = dot( l_v3TmpVec, l_v3Binormal		);\n"													)
			cuT(	"	l_v3EyeVec.z = dot( l_v3TmpVec, l_v4Normals.xyz		);\n"													)
			cuT(	"	l_v3EyeVec = normalize( l_v3EyeVec );\n"																	)
			cuT(	"	for( int i = 0; i < c3d_iLightsCount; i++ )\n"																)
			cuT(	"	{\n"																										)
			cuT(	"		l_light = GetLight( i );\n"																				)
			cuT(	"		vec4 l_v4LightDir = BlinnPhongDir( l_light, l_v4Positions.xyz, <pxlin_mtxModelView> );\n"				)
			cuT(	"		l_v3LightDir = l_v4LightDir.xyz;\n"																		)
			cuT(	"		l_fAttenuation = l_v4LightDir.w;\n"																		)
			cuT(	"		Bump( l_v4Normals.xyz, l_v4Tangents.xyz, l_v3Binormal, l_v3LightDir, l_fAttenuation );\n"				)
			cuT(	"		l_fLambert = dot( l_v4Normals.xyz, l_v3LightDir );\n"													)
			cuT(	"		l_v4MatSpecular = c3d_v4MatSpecular;\n"																	)
			cuT(	"		vec2 l_v2SpecLamb = Fresnel( l_fLambert, l_v3LightDir, l_v3Normal, l_v3EyeVec, l_v4MatSpecular );\n"	)
			cuT(	"		l_fSpecular = l_v2SpecLamb.x;\n"																		)
			cuT(	"		l_fLambert = l_v2SpecLamb.y;\n"																			)
			cuT(	"		l_v3TmpDiffuse	= l_light.m_v4Diffuse.xyz	* l_v4Diffuses.xyz	* l_fLambert;\n"						)
			cuT(	"		l_v3TmpSpecular	= l_light.m_v4Specular.xyz	* l_v4MatSpecular.xyz	* l_fSpecular;\n"					)
			cuT(	"		l_v3Diffuse		+= l_v3TmpDiffuse * l_fAttenuation;\n"													)
			cuT(	"		l_v3Specular	+= l_v3TmpSpecular * l_fAttenuation;\n"													)
			cuT(	"	}\n"																										)
			cuT(	"	pxl_v4FragColor = vec4( l_v3Diffuse + l_v3Specular, 1 );\n"													)
			cuT(	"}\n"																											);
	}
};

PixelShaderSource g_pixelShaderSource;
DeferredShaderSource g_deferredShaderSource;

GlShaderProgram :: GlShaderProgram( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	ShaderProgramBase	( p_pRenderSystem, eSHADER_LANGUAGE_GLSL	)
	,	m_programObject		( 0											)
	,	m_gl				( p_gl										)
{
	CreateObject( eSHADER_TYPE_VERTEX	);
	CreateObject( eSHADER_TYPE_PIXEL	);
}

GlShaderProgram :: ~GlShaderProgram()
{
	ShaderProgramBase::Cleanup();

	if( m_pRenderSystem->UseShaders() )
	{
		if( m_programObject )
		{
			m_gl.DeleteProgram( m_programObject );
			m_programObject = 0;
		}
	}
}

void GlShaderProgram :: Initialise()
{
	if ( m_eStatus != ePROGRAM_STATUS_LINKED )
	{
		if (m_pRenderSystem->UseShaders() && !m_programObject )
		{
			m_programObject = m_gl.CreateProgram();
		}

		ShaderProgramBase::Initialise();
	}
}

bool GlShaderProgram :: Link()
{
	bool        l_bReturn       = false;
	uint32_t    l_uiNbAttached  = 0;
	int         l_iLinked       = 0;

	if (m_pRenderSystem->UseShaders() && m_eStatus != ePROGRAM_STATUS_ERROR )
	{
		l_bReturn = true;

		for (int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++)
		{
			if( m_pShaders[i] && m_pShaders[i]->GetStatus() == eSHADER_STATUS_COMPILED )
			{
				std::static_pointer_cast< GlShaderObject >( m_pShaders[i])->AttachTo( this );
				l_uiNbAttached++;
			}
		}

		Logger::LogDebug( cuT( "GlShaderProgram :: Link - Programs attached : %d" ), l_uiNbAttached );
		l_bReturn &= m_gl.LinkProgram( m_programObject);
		l_bReturn &= m_gl.GetProgramiv( m_programObject, eGL_SHADER_STATUS_LINK, &l_iLinked );
		Logger::LogDebug( cuT( "GlShaderProgram :: Link - Program link status : %i" ), l_iLinked );

		RetrieveLinkerLog( m_linkerLog );

		if( m_linkerLog.size() > 0 )
		{
			Logger::LogDebug( cuT( "GlShaderProgram :: Link - Log : " ) + m_linkerLog );
		}

		if( l_iLinked && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
		{
			ShaderProgramBase::Link();
		}
		else
		{
			m_eStatus = ePROGRAM_STATUS_ERROR;
		}

		l_bReturn = m_eStatus == ePROGRAM_STATUS_LINKED;
	}

	return l_bReturn;
}

void GlShaderProgram :: RetrieveLinkerLog( String & strLog)
{
	if ( !m_pRenderSystem->UseShaders() )
	{
		strLog = m_gl.GetGlslErrorString( 0 );
	}
	else
	{
		int l_iLength = 0;
		int l_iLength2 = 0;

		if( m_programObject == 0 )
		{
			strLog = m_gl.GetGlslErrorString( 2 );
		}
		else
		{
			m_gl.GetProgramiv( m_programObject, eGL_SHADER_STATUS_INFO_LOG_LENGTH , &l_iLength );

			if (l_iLength > 1)
			{
				char * l_pTmp = new char[l_iLength];
				m_gl.GetProgramInfoLog( m_programObject, l_iLength, &l_iLength2, l_pTmp );
				strLog = str_utils::from_str( l_pTmp );
				delete_array l_pTmp;
			}
		}
	}
}

void GlShaderProgram :: Begin( uint8_t p_byIndex, uint8_t p_byCount )
{
	if( m_pRenderSystem->UseShaders() && m_programObject != 0 && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
	{
		m_gl.UseProgram( m_programObject );
		m_pRenderSystem->GetPipeline()->UpdateFunctions( this );
		ShaderProgramBase::Begin( p_byIndex, p_byCount );

		m_pPassBuffer->Bind();
		m_pUserBuffer->Bind();
		m_pSceneBuffer->Bind();
		m_pMatrixBuffer->Bind();

		m_gl.UseProgram( m_programObject );
		for (int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++)
		{
			if( m_pShaders[i] )
			{
				for( FrameVariablePtrStrMapIt l_it = m_pShaders[i]->GetFrameVariablesBegin(); l_it != m_pShaders[i]->GetFrameVariablesEnd(); ++l_it )
				{
					l_it->second.lock()->Apply();
				}
			}
		}
	}
}

void GlShaderProgram :: End()
{
	m_pPassBuffer->Unbind();
	m_pSceneBuffer->Unbind();
	m_pMatrixBuffer->Unbind();
	m_pUserBuffer->Unbind();

	ShaderProgramBase::End();
}

int GlShaderProgram :: GetAttributeLocation( String const & p_strName)const
{
	int l_iReturn = eGL_INVALID_INDEX;

	if (m_programObject != eGL_INVALID_INDEX && m_gl.IsProgram( m_programObject))
	{
		l_iReturn = m_gl.GetAttribLocation( m_programObject, str_utils::to_str( p_strName ).c_str());
	}

	return l_iReturn;
}

String GlShaderProgram :: DoGetVertexShaderSource( uint32_t p_uiProgramFlags )
{
	String	l_strReturn;
	::GLSL::VariablesBase * l_pVariables = NULL;
	GLSL::ConstantsBase * l_pConstants = NULL;
	GLSL::KeywordsBase * l_pKeywords = NULL;

	if( m_gl.HasUbo() )
	{
		l_pVariables = &::GLSL::variablesUbo;
		l_pConstants = &GLSL::constantsUbo;
	}
	else
	{
		l_pVariables = &::GLSL::variablesStd;
		l_pConstants = &GLSL::constantsStd;
	}

	switch( m_gl.GetGlslVersion() )
	{
	case 110:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
	case 120:	l_pKeywords = new GLSL::Keywords< 120 >;	break;
	case 130:	l_pKeywords = new GLSL::Keywords< 130 >;	break;
	case 140:	l_pKeywords = new GLSL::Keywords< 140 >;	break;
	case 150:	l_pKeywords = new GLSL::Keywords< 150 >;	break;
	case 330:	l_pKeywords = new GLSL::Keywords< 330 >;	break;
	case 400:	l_pKeywords = new GLSL::Keywords< 400 >;	break;
	case 410:	l_pKeywords = new GLSL::Keywords< 410 >;	break;
	case 420:	l_pKeywords = new GLSL::Keywords< 420 >;	break;
	case 430:	l_pKeywords = new GLSL::Keywords< 430 >;	break;
	default:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
	}

	String l_strVersion		= l_pKeywords->GetVersion();
	String l_strAttribute0	= l_pKeywords->GetAttribute( 0 );
	String l_strAttribute1	= l_pKeywords->GetAttribute( 1 );
	String l_strAttribute2	= l_pKeywords->GetAttribute( 2 );
	String l_strAttribute3	= l_pKeywords->GetAttribute( 3 );
	String l_strAttribute4	= l_pKeywords->GetAttribute( 4 );
	String l_strAttribute5	= l_pKeywords->GetAttribute( 5 );
	String l_strAttribute6	= l_pKeywords->GetAttribute( 6 );
	String l_strAttribute7	= l_pKeywords->GetAttribute( 7 );
	String l_strIn			= l_pKeywords->GetIn();
	String l_strOut			= l_pKeywords->GetOut();
	
	String l_strVertexInMatrices	= l_pConstants->Matrices();
	String l_strVertexOutMatrices	= l_pVariables->GetVertexOutMatrices();
	String l_strVertexMatrixCopy	= l_pVariables->GetVertexMatrixCopy();
	str_utils::replace( l_strVertexOutMatrices, cuT( "out" ), l_strOut );

	l_strReturn += l_strVersion;
	l_strReturn += l_strAttribute0	+ cuT( "	<vec4> 	vertex;\n"			);
	l_strReturn += l_strAttribute1	+ cuT( "	<vec3> 	normal;\n"			);
	l_strReturn += l_strAttribute2	+ cuT( "	<vec3>	tangent;\n"			);
	l_strReturn += l_strAttribute3	+ cuT( "	<vec3>	bitangent;\n"		);
	l_strReturn += l_strAttribute4	+ cuT( "	<vec3> 	texture;\n"			);

	if( (p_uiProgramFlags & ePROGRAM_FLAG_SKINNING) == ePROGRAM_FLAG_SKINNING )
	{
		l_strReturn += l_strAttribute5	+ cuT( "	ivec4 	bone_ids;\n"	);
		l_strReturn += l_strAttribute6	+ cuT( "	<vec4> 	weights;\n"		);
	}

	if( (p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION) == ePROGRAM_FLAG_INSTANCIATION )
	{
		l_strReturn += l_strAttribute7	+ cuT( "	<mat4> 	transform;\n"	);
	}

	l_strReturn += l_strVertexInMatrices;
	l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_vertex;\n"		);
	l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_normal;\n"		);
	l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_tangent;\n"		);
	l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_bitangent;\n"	);
	l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_texture;\n"		);

	l_strReturn += l_strVertexOutMatrices;
	l_strReturn += cuT( "void main()\n"																		);
	l_strReturn += cuT( "{\n"																				);
	l_strReturn +=			l_strVertexMatrixCopy;
	l_strReturn += cuT( "	vtx_texture 	= texture;\n"													);
	l_strReturn += cuT( "	<vec4> l_v4Vertex		= vec4( vertex.xyz, 1.0 );\n"							);
	l_strReturn += cuT( "	<vec4> l_v4Normal 		= vec4( normal, 0.0 );\n"								);
	l_strReturn += cuT( "	<vec4> l_v4Tangent 		= vec4( tangent, 0.0 );\n"								);
	l_strReturn += cuT( "	<vec4> l_v4Bitangent	= vec4( bitangent, 0.0 );\n"							);

	if( (p_uiProgramFlags & ePROGRAM_FLAG_SKINNING) == ePROGRAM_FLAG_SKINNING )
	{
		l_strReturn += cuT( "	<mat4> l_mtxBoneTransform	 = c3d_mtxBones[bone_ids[0]] * weights[0];\n"	);
		l_strReturn += cuT( "	l_mtxBoneTransform			+= c3d_mtxBones[bone_ids[1]] * weights[1];\n"	);
		l_strReturn += cuT( "	l_mtxBoneTransform			+= c3d_mtxBones[bone_ids[2]] * weights[2];\n"	);
		l_strReturn += cuT( "	l_mtxBoneTransform			+= c3d_mtxBones[bone_ids[3]] * weights[3];\n"	);
//		l_strReturn += cuT( "	l_mtxBoneTransform = transpose( l_mtxBoneTransform );"						);
		l_strReturn += cuT( "	l_v4Vertex					= l_mtxBoneTransform * l_v4Vertex;\n"			);
		l_strReturn += cuT( "	l_v4Normal					= l_mtxBoneTransform * l_v4Normal;\n"			);
		l_strReturn += cuT( "	l_v4Tangent					= l_mtxBoneTransform * l_v4Tangent;\n"			);
		l_strReturn += cuT( "	l_v4Bitangent				= l_mtxBoneTransform * l_v4Bitangent;\n"		);
	}

	if( (p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION) == ePROGRAM_FLAG_INSTANCIATION )
	{
		l_strReturn += cuT( "	<mat4> l_mtxMV	= c3d_mtxView * transform;\n"								);
		l_strReturn += cuT( "	<mat4> l_mtxN	= transpose( inverse( l_mtxMV ) );\n"						);
		l_strReturn += cuT( "	l_v4Vertex		= l_mtxMV * l_v4Vertex;\n"									);
		l_strReturn += cuT( "	l_v4Normal 		= l_mtxN * l_v4Normal;\n"									);
		l_strReturn += cuT( "	l_v4Tangent 	= l_mtxN * l_v4Tangent;\n"									);
		l_strReturn += cuT( "	l_v4Bitangent	= l_mtxN * l_v4Bitangent;\n"								);
	}
	else
	{
		l_strReturn += cuT( "	l_v4Vertex		= c3d_mtxModelView * l_v4Vertex;\n"							);
		l_strReturn += cuT( "	l_v4Normal 		= c3d_mtxNormal * l_v4Normal;\n"							);
		l_strReturn += cuT( "	l_v4Tangent 	= c3d_mtxNormal * l_v4Tangent;\n"							);
		l_strReturn += cuT( "	l_v4Bitangent	= c3d_mtxNormal * l_v4Bitangent;\n"							);
	}

	l_strReturn += cuT( "	vtx_vertex		= l_v4Vertex.xyz;// / l_v4Vertex.w;\n"							);
	l_strReturn += cuT( "	vtx_normal		= normalize( l_v4Normal.xyz );\n"								);
	l_strReturn += cuT( "	vtx_tangent		= normalize( l_v4Tangent.xyz );\n"								);
	l_strReturn += cuT( "	vtx_bitangent	= normalize( l_v4Bitangent.xyz );\n"							);
	l_strReturn += cuT( "	gl_Position 	= c3d_mtxProjection * l_v4Vertex;\n"							);
	l_strReturn += cuT( "}\n"																				);

	GLSL::ConstantsBase::Replace( l_strReturn );

	delete l_pKeywords;
	return l_strReturn;
}

String GlShaderProgram :: DoGetPixelShaderSource( uint32_t p_uiFlags )
{
	String	l_strReturn;
	::GLSL::VariablesBase * l_pVariables = NULL;
	GLSL::ConstantsBase * l_pConstants = NULL;
	GLSL::KeywordsBase * l_pKeywords = NULL;

	if( m_gl.HasUbo() )
	{
		l_pVariables = &::GLSL::variablesUbo;
		l_pConstants = &GLSL::constantsUbo;
	}
	else
	{
		l_pVariables = &::GLSL::variablesStd;
		l_pConstants = &GLSL::constantsStd;
	}

	switch( m_gl.GetGlslVersion() )
	{
	case 110:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
	case 120:	l_pKeywords = new GLSL::Keywords< 120 >;	break;
	case 130:	l_pKeywords = new GLSL::Keywords< 130 >;	break;
	case 140:	l_pKeywords = new GLSL::Keywords< 140 >;	break;
	case 150:	l_pKeywords = new GLSL::Keywords< 150 >;	break;
	case 330:	l_pKeywords = new GLSL::Keywords< 330 >;	break;
	case 400:	l_pKeywords = new GLSL::Keywords< 400 >;	break;
	case 410:	l_pKeywords = new GLSL::Keywords< 410 >;	break;
	case 420:	l_pKeywords = new GLSL::Keywords< 420 >;	break;
	case 430:	l_pKeywords = new GLSL::Keywords< 430 >;	break;
	default:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
	}

	String l_strVersion		= l_pKeywords->GetVersion();
	String l_strIn			= l_pKeywords->GetIn();
	String l_strOut			= l_pKeywords->GetOut();

	String l_strPixelInMatrices						= l_pVariables->GetPixelInMatrices();
	String l_strPixelScene							= l_pConstants->Scene();
	String l_strPixelPass							= l_pConstants->Pass();
	String l_strFragOutput							= l_pKeywords->GetPixelOut();
	String l_strPixelOutput							= l_pKeywords->GetPixelOutputName();
	String l_strPixelLights							= g_pixelShaderSource.GetPixelLights();
	String l_strPixelMainDeclarations				= g_pixelShaderSource.GetPixelMainDeclarations();
	String l_strPixelMainLightsLoop					= g_pixelShaderSource.GetPixelMainLightsLoop();
	String l_strPixelMainLightsLoopEnd				= g_pixelShaderSource.GetPixelMainLightsLoopEnd();
	String l_strPixelMainEnd						= g_pixelShaderSource.GetPixelMainEnd();
	String l_strPixelMainLightsLoopAfterLightDir	= g_pixelShaderSource.GetPixelMainLightsLoopAfterLightDir();
	String l_strPixelMtxModelView					= l_pVariables->GetPixelMtxModelView();

	str_utils::replace( l_strPixelInMatrices, cuT( "in" ), l_strIn );

	l_strReturn += l_strVersion;
	l_strReturn += l_strPixelInMatrices;
	l_strReturn += l_strPixelScene;
	l_strReturn += l_strPixelPass;
	l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_vertex;\n"		);
	l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_normal;\n"		);
	l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_tangent;\n"		);
	l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_bitangent;\n"	);
	l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_texture;\n"		);
	l_strReturn += l_pKeywords->GetLayout( 0 ) + l_strFragOutput;
//	l_strReturn += cuT( "uniform	sampler2D 	c3d_sLights;\n"		);
	l_strReturn += cuT( "uniform	sampler1D 	c3d_sLights;\n"		);

	l_strPixelMainDeclarations += cuT( "	" ) + l_strPixelOutput + cuT( " = vec4( 0.0, 0.0, 0.0, 0.0 );\n" );

	if( p_uiFlags != 0 )
	{
		if( (p_uiFlags & eTEXTURE_CHANNEL_COLOUR) == eTEXTURE_CHANNEL_COLOUR )
		{
			l_strReturn									+= cuT( "uniform sampler2D 	c3d_mapColour;\n" );
			l_strPixelMainDeclarations					+= cuT( "	vec4	l_v4MapColour		= texture2D( c3d_mapColour, vtx_texture.xy );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Ambient		*= l_v4MapColour;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_AMBIENT) == eTEXTURE_CHANNEL_AMBIENT )
		{
			l_strReturn									+= cuT( "uniform sampler2D 	c3d_mapAmbient;\n" );
			l_strPixelMainDeclarations					+= cuT( "	vec4	l_v4MapAmbient		= texture2D( c3d_mapAmbient, vtx_texture.xy );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Ambient		*= l_v4MapAmbient;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE) == eTEXTURE_CHANNEL_DIFFUSE )
		{
			l_strReturn									+= cuT( "uniform sampler2D 	c3d_mapDiffuse;\n" );
			l_strPixelMainDeclarations					+= cuT( "	vec4	l_v4MapDiffuse		= texture2D( c3d_mapDiffuse, vtx_texture.xy );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_v4Diffuse		*= l_v4MapDiffuse;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_NORMAL) == eTEXTURE_CHANNEL_NORMAL )
		{
			l_strReturn									+= cuT( "uniform sampler2D 	c3d_mapNormal;\n" );
			l_strPixelMainDeclarations					+= cuT( "	float 	l_fSqrLength;\n" );
			l_strPixelMainDeclarations					+= cuT( "	vec4	l_v4MapNormal		= texture2D( c3d_mapNormal, vtx_texture.xy );\n" );
			l_strPixelMainDeclarations					+= cuT( "	float	l_fInvRadius		= 0.02;\n" );
			l_strPixelMainDeclarations					+= cuT( "	l_v3Normal					= normalize( l_v4MapNormal.xyz * 2.0 - 1.0 );\n" );
			l_strPixelMainLightsLoop					+= cuT( "		Bump( vtx_tangent, vtx_bitangent, vtx_normal, l_v3LightDir, l_fAttenuation );\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_OPACITY) == eTEXTURE_CHANNEL_OPACITY )
		{
			l_strReturn									+= cuT( "uniform sampler2D 	c3d_mapOpacity;\n" );
			l_strPixelMainDeclarations					+= cuT( "	vec4	l_v4MapOpacity		= texture2D( c3d_mapOpacity, vtx_texture.xy );\n" );
			l_strPixelMainLightsLoopEnd					+= cuT( "	l_fAlpha		= l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_SPECULAR) == eTEXTURE_CHANNEL_SPECULAR )
		{
			l_strReturn									+= cuT( "uniform sampler2D 	c3d_mapSpecular;\n" );
			l_strPixelMainDeclarations					+= cuT( "	vec4	l_v4MapSpecular		= texture2D( c3d_mapSpecular, vtx_texture.xy );\n" );
			l_strPixelMainLightsLoopAfterLightDir		+= cuT( "	l_v4TmpSpecular = l_fAttenuation * l_light.m_v4Specular * l_v4MapSpecular * l_fSpecular;\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_HEIGHT) == eTEXTURE_CHANNEL_HEIGHT )
		{
			l_strReturn									+= cuT( "uniform sampler2D 	c3d_mapHeight;\n" );
			l_strPixelMainDeclarations					+= cuT( "	vec4	l_v4MapHeight		= texture2D( c3d_mapHeight, vtx_texture.xy );\n" );
		}
		if( (p_uiFlags & eTEXTURE_CHANNEL_GLOSS) == eTEXTURE_CHANNEL_GLOSS )
		{
			l_strReturn									+= cuT( "uniform sampler2D 	c3d_mapGloss;\n" );
			l_strPixelMainDeclarations					+= cuT( "	vec4	l_v4MapGloss		= texture2D( c3d_mapGloss, vtx_texture.xy );\n" );
		}
	}

	l_strPixelMainLightsLoopEnd	+= cuT( "	" ) + l_strPixelOutput + cuT( " = vec4( (l_v4Ambient + l_v4Diffuse + l_v4Specular).xyz, l_fAlpha );\n" );

	l_strReturn += l_strPixelLights + l_strPixelMainDeclarations + l_strPixelMainLightsLoop + l_strPixelMainLightsLoopAfterLightDir + l_strPixelMainLightsLoopEnd + l_strPixelMainEnd;
	str_utils::replace( l_strReturn, cuT( "<pxlin_mtxModelView>" ), l_strPixelMtxModelView );
//	Logger::LogDebug( l_strReturn );

	GLSL::ConstantsBase::Replace( l_strReturn );

	delete l_pKeywords;
	return l_strReturn;
}

String GlShaderProgram :: DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass )
{
	String	l_strReturn;
	::GLSL::VariablesBase * l_pVariables = NULL;
	GLSL::ConstantsBase * l_pConstants = NULL;
	GLSL::KeywordsBase * l_pKeywords = NULL;

	if( m_gl.HasUbo() )
	{
		l_pVariables = &::GLSL::variablesUbo;
		l_pConstants = &GLSL::constantsUbo;
	}
	else
	{
		l_pVariables = &::GLSL::variablesStd;
		l_pConstants = &GLSL::constantsStd;
	}

	switch( m_gl.GetGlslVersion() )
	{
	case 110:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
	case 120:	l_pKeywords = new GLSL::Keywords< 120 >;	break;
	case 130:	l_pKeywords = new GLSL::Keywords< 130 >;	break;
	case 140:	l_pKeywords = new GLSL::Keywords< 140 >;	break;
	case 150:	l_pKeywords = new GLSL::Keywords< 150 >;	break;
	case 330:	l_pKeywords = new GLSL::Keywords< 330 >;	break;
	case 400:	l_pKeywords = new GLSL::Keywords< 400 >;	break;
	case 410:	l_pKeywords = new GLSL::Keywords< 410 >;	break;
	case 420:	l_pKeywords = new GLSL::Keywords< 420 >;	break;
	case 430:	l_pKeywords = new GLSL::Keywords< 430 >;	break;
	default:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
	}

	String l_strVersion		= l_pKeywords->GetVersion();
	String l_strAttribute0	= l_pKeywords->GetAttribute( 0 );
	String l_strAttribute1	= l_pKeywords->GetAttribute( 1 );
	String l_strAttribute2	= l_pKeywords->GetAttribute( 2 );
	String l_strAttribute3	= l_pKeywords->GetAttribute( 3 );
	String l_strAttribute4	= l_pKeywords->GetAttribute( 4 );
	String l_strAttribute5	= l_pKeywords->GetAttribute( 5 );
	String l_strIn			= l_pKeywords->GetIn();
	String l_strOut			= l_pKeywords->GetOut();

	if( p_bLightPass )
	{
		String l_strVertexInMatrices	= l_pConstants->Matrices();
		String l_strVertexOutMatrices	= l_pVariables->GetVertexOutMatrices();
		String l_strVertexMatrixCopy	= l_pVariables->GetVertexMatrixCopy();
		str_utils::replace( l_strVertexOutMatrices, cuT( "out" ), l_strOut );

		l_strReturn += l_strVersion;
		l_strReturn += l_strAttribute0	+ cuT( "	<vec4>	vertex;\n"					);
		l_strReturn += l_strAttribute1	+ cuT( "	<vec2>	texture;\n"					);
		l_strReturn += l_strVertexInMatrices;
		l_strReturn += l_strOut			+ cuT( "	<vec2>	vtx_texture;\n"				);
		l_strReturn += l_strVertexOutMatrices;
		l_strReturn += cuT( "void main( void )\n"										);
		l_strReturn += cuT( "{\n"														);
		l_strReturn +=			l_strVertexMatrixCopy;
		l_strReturn += cuT( "	gl_Position = c3d_mtxProjectionModelView * vertex;\n"	);
		l_strReturn += cuT( "	vtx_texture = texture;\n"								);
		l_strReturn += cuT( "}\n"														);
	}
	else
	{
		String l_strVertexInMatrices	= l_pConstants->Matrices();
		String l_strVertexOutMatrices	= l_pVariables->GetVertexOutMatrices();
		String l_strVertexMatrixCopy	= l_pVariables->GetVertexMatrixCopy();
		str_utils::replace( l_strVertexOutMatrices, cuT( "out" ), l_strOut );
		l_strReturn += l_strVersion;
		l_strReturn += l_strAttribute0	+ cuT( "	<vec4> 	vertex;\n"			);
		l_strReturn += l_strAttribute1	+ cuT( "	<vec3> 	normal;\n"			);
		l_strReturn += l_strAttribute2	+ cuT( "	<vec3>	tangent;\n"			);
		l_strReturn += l_strAttribute3	+ cuT( "	<vec3>	bitangent;\n"		);
		l_strReturn += l_strAttribute4	+ cuT( "	<vec3> 	texture;\n"			);

		if( (p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION) == ePROGRAM_FLAG_INSTANCIATION )
		{
			l_strReturn += l_strAttribute5	+ cuT( "	<mat4> 	transform;\n"	);
		}

		l_strReturn += l_strVertexInMatrices;
		l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_vertex;\n"		);
		l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_normal;\n"		);
		l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_tangent;\n"		);
		l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_bitangent;\n"	);
		l_strReturn += l_strOut			+ cuT( "	<vec3> 	vtx_texture;\n"		);

		l_strReturn += l_strVertexOutMatrices;
		l_strReturn += cuT( "void main()\n"																			);
		l_strReturn += cuT( "{\n"																					);
		l_strReturn +=			l_strVertexMatrixCopy;
		l_strReturn += cuT( "	vtx_texture 	= texture;\n"														);

		if( (p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION) == ePROGRAM_FLAG_INSTANCIATION )
		{
			l_strReturn += cuT( "	<mat4> l_mtxMV	= c3d_mtxView * transform;\n"										);
			l_strReturn += cuT( "	<mat4> l_mtxN	= transpose( inverse( l_mtxMV ) );\n"								);
			l_strReturn += cuT( "	vtx_normal 		= normalize( (l_mtxN * <vec4>( normal, 0.0 )).xyz );\n"				);
			l_strReturn += cuT( "	vtx_tangent 	= normalize( (l_mtxN * <vec4>( tangent, 0.0 )).xyz );\n"			);
			l_strReturn += cuT( "	vtx_bitangent	= normalize( (l_mtxN * <vec4>( bitangent, 0.0 )).xyz );\n"			);
			l_strReturn += cuT( "	<vec4> l_v4Vtx	= l_mtxMV * vertex;\n"												);
			l_strReturn += cuT( "	gl_Position 	= c3d_mtxProjection * l_mtxMV * vertex;\n"							);
		}
		else
		{
			l_strReturn += cuT( "	vtx_normal 		= normalize( (c3d_mtxNormal * <vec4>( normal, 0.0 )).xyz );\n"		);
			l_strReturn += cuT( "	vtx_tangent 	= normalize( (c3d_mtxNormal * <vec4>( tangent, 0.0 )).xyz );\n"		);
			l_strReturn += cuT( "	vtx_bitangent	= normalize( (c3d_mtxNormal * <vec4>( bitangent, 0.0 )).xyz );\n"	);
			l_strReturn += cuT( "	<vec4> l_v4Vtx	= c3d_mtxModelView * vertex;\n"										);
			l_strReturn += cuT( "	gl_Position 	= c3d_mtxProjectionModelView * vertex;\n"							);
		}

		l_strReturn += cuT( "}\n"																					);
	}

	GLSL::ConstantsBase::Replace( l_strReturn );

	delete l_pKeywords;
	return l_strReturn;
}

String GlShaderProgram :: DoGetDeferredPixelShaderSource( uint32_t p_uiFlags )
{
	String	l_strReturn;
	::GLSL::VariablesBase * l_pVariables = NULL;
	GLSL::ConstantsBase * l_pConstants = NULL;
	GLSL::KeywordsBase * l_pKeywords = NULL;

	if( m_gl.HasUbo() )
	{
		l_pVariables = &::GLSL::variablesUbo;
		l_pConstants = &GLSL::constantsUbo;
	}
	else
	{
		l_pVariables = &::GLSL::variablesStd;
		l_pConstants = &GLSL::constantsStd;
	}

	switch( m_gl.GetGlslVersion() )
	{
	case 110:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
	case 120:	l_pKeywords = new GLSL::Keywords< 120 >;	break;
	case 130:	l_pKeywords = new GLSL::Keywords< 130 >;	break;
	case 140:	l_pKeywords = new GLSL::Keywords< 140 >;	break;
	case 150:	l_pKeywords = new GLSL::Keywords< 150 >;	break;
	case 330:	l_pKeywords = new GLSL::Keywords< 330 >;	break;
	case 400:	l_pKeywords = new GLSL::Keywords< 400 >;	break;
	case 410:	l_pKeywords = new GLSL::Keywords< 410 >;	break;
	case 420:	l_pKeywords = new GLSL::Keywords< 420 >;	break;
	case 430:	l_pKeywords = new GLSL::Keywords< 430 >;	break;
	default:	l_pKeywords = new GLSL::Keywords< 110 >;	break;
	}

	String l_strVersion		= l_pKeywords->GetVersion();
	String l_strIn			= l_pKeywords->GetIn();
	String l_strOut			= l_pKeywords->GetOut();

	if( (p_uiFlags & eTEXTURE_CHANNEL_LGHTPASS) == eTEXTURE_CHANNEL_LGHTPASS )
	{
		String l_strPixelInMatrices		= l_pVariables->GetPixelInMatrices();
		String l_strPixelScene			= l_pConstants->Scene();
		String l_strPixelPass			= l_pConstants->Pass();
		String l_strPixelMtxModelView	= l_pVariables->GetPixelMtxModelView();

		str_utils::replace( l_strPixelInMatrices, cuT( "in" ), l_strIn );

		l_strReturn += l_strVersion;
		l_strReturn += l_strPixelInMatrices;
		l_strReturn += l_strPixelScene;
		l_strReturn += l_strPixelPass;
		l_strReturn += l_pKeywords->GetPixelOut();
		l_strReturn += cuT( "uniform	sampler1D 	c3d_sLights;\n"		);
		l_strReturn += g_deferredShaderSource.GetLSPixelProgram();
		str_utils::replace( l_strReturn, cuT( "<pxlin_mtxModelView>" ), l_strPixelMtxModelView );
	}
	else
	{
		String l_strPixelDeclarations					= g_deferredShaderSource.GetGSPixelDeclarations();
		String l_strPixelMainDeclarations				= g_deferredShaderSource.GetGSPixelMainDeclarations();
		String l_strPixelMainLightsLoopAfterLightDir	= g_deferredShaderSource.GetGSPixelMainLightsLoopAfterLightDir();
		String l_strPixelMainLightsLoopEnd;
		String l_strPixelMainEnd						= g_deferredShaderSource.GetGSPixelMainEnd();
		String l_strPixelInMatrices						= l_pConstants->Matrices();
		String l_strPixelScene							= l_pConstants->Scene();
		String l_strPixelPass							= l_pConstants->Pass();

		str_utils::replace( l_strPixelInMatrices, cuT( "in" ), l_strIn );

		l_strReturn += l_strVersion;
		l_strReturn += l_strPixelInMatrices;
		l_strReturn += l_strPixelScene;
		l_strReturn += l_strPixelPass;
		l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_vertex;\n"		);
		l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_normal;\n"		);
		l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_tangent;\n"		);
		l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_bitangent;\n"	);
		l_strReturn += l_strIn	+ cuT( "	<vec3>	vtx_texture;\n"		);
		l_strReturn += l_strPixelDeclarations;
		l_strReturn += l_pKeywords->GetGSOutPositionDecl();
		l_strReturn += l_pKeywords->GetGSOutDiffuseDecl();
		l_strReturn += l_pKeywords->GetGSOutNormalDecl();
		l_strReturn += l_pKeywords->GetGSOutTangentDecl();
		l_strReturn += l_pKeywords->GetGSOutBitangentDecl();
		l_strReturn += l_pKeywords->GetGSOutSpecularDecl();

		if( p_uiFlags != 0 )
		{
			if( (p_uiFlags & eTEXTURE_CHANNEL_COLOUR) == eTEXTURE_CHANNEL_COLOUR )
			{
				l_strReturn						+= cuT( "uniform sampler2D 	c3d_mapColour;\n" );
				l_strPixelMainDeclarations		+= cuT( "	vec4	l_v4MapColour		= texture2D( c3d_mapColour, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd		+= cuT( "	l_v4Diffuse		*= l_v4MapColour;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_AMBIENT) == eTEXTURE_CHANNEL_AMBIENT )
			{
				l_strReturn						+= cuT( "uniform sampler2D 	c3d_mapAmbient;\n" );
				l_strPixelMainDeclarations		+= cuT( "	vec4	l_v4MapAmbient		= texture2D( c3d_mapAmbient, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd		+= cuT( "	l_v4Diffuse		*= l_v4MapAmbient;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE) == eTEXTURE_CHANNEL_DIFFUSE )
			{
				l_strReturn						+= cuT( "uniform sampler2D 	c3d_mapDiffuse;\n" );
				l_strPixelMainDeclarations		+= cuT( "	vec4	l_v4MapDiffuse		= texture2D( c3d_mapDiffuse, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd		+= cuT( "	l_v4Diffuse		*= l_v4MapDiffuse;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_NORMAL) == eTEXTURE_CHANNEL_NORMAL )
			{
				l_strReturn						+= cuT( "uniform sampler2D 	c3d_mapNormal;\n" );
				l_strPixelMainDeclarations		+= cuT( "	vec4	l_v4MapNormal		= texture2D( c3d_mapNormal, vtx_texture.xy );\n" );
				l_strPixelMainDeclarations		+= cuT( "	l_v4Normal					+= vec4( normalize( (l_v4MapNormal.xyz * 2.0 - 1.0) ), 0 );\n" );
//				l_strPixelMainDeclarations		+= cuT( "	l_v4Tangent 				-= vec4( l_v4Normal.xyz * dot( l_v4Tangent.xyz, l_v4Normal.xyz ), 0 );\n" );
//				l_strPixelMainDeclarations		+= cuT( "	l_v4Bitangent 				= vec4( cross( l_v4Normal.xyz, l_v4Tangent.xyz ), 1 );\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_OPACITY) == eTEXTURE_CHANNEL_OPACITY )
			{
				l_strReturn						+= cuT( "uniform sampler2D 	c3d_mapOpacity;\n" );
				l_strPixelMainDeclarations		+= cuT( "	vec4	l_v4MapOpacity		= texture2D( c3d_mapOpacity, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd		+= cuT( "	l_fAlpha		= l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_SPECULAR) == eTEXTURE_CHANNEL_SPECULAR )
			{
				l_strReturn						+= cuT( "uniform sampler2D 	c3d_mapSpecular;\n" );
				l_strPixelMainDeclarations		+= cuT( "	vec4	l_v4MapSpecular		= texture2D( c3d_mapSpecular, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd		+= cuT( "	l_v4Specular.xyz	*= l_v4MapSpecular.xyz;\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_HEIGHT) == eTEXTURE_CHANNEL_HEIGHT )
			{
				l_strReturn						+= cuT( "uniform sampler2D 	c3d_mapHeight;\n" );
				l_strPixelMainDeclarations		+= cuT( "	vec4	l_v4MapHeight		= texture2D( c3d_mapHeight, vtx_texture.xy );\n" );
			}
			if( (p_uiFlags & eTEXTURE_CHANNEL_GLOSS) == eTEXTURE_CHANNEL_GLOSS )
			{
				l_strReturn						+= cuT( "uniform sampler2D 	c3d_mapGloss;\n" );
				l_strPixelMainDeclarations		+= cuT( "	vec4	l_v4MapGloss		= texture2D( c3d_mapGloss, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd		+= cuT( "	l_v4Specular.w	*= l_v4MapGloss.x;\n" );
			}
		}

		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutPositionName() + cuT( " = vec4( l_v4Position.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutDiffuseName() + cuT( " = vec4( l_v4Diffuse.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutNormalName() + cuT( " = vec4( l_v4Normal.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutTangentName() + cuT( " = vec4( l_v4Tangent.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutBitangentName() + cuT( " = vec4( l_v4Bitangent.xyz, 1 );\n" );
		l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutSpecularName() + cuT( " = vec4( l_v4Specular );\n" );

		l_strReturn += l_strPixelMainDeclarations + l_strPixelMainLightsLoopAfterLightDir + l_strPixelMainLightsLoopEnd + l_strPixelMainEnd;
//		Logger::LogDebug( l_strReturn );
	}

	GLSL::ConstantsBase::Replace( l_strReturn );

	delete l_pKeywords;
	return l_strReturn;
}

ShaderObjectBaseSPtr GlShaderProgram :: DoCreateObject( eSHADER_TYPE p_eType)
{
	ShaderObjectBaseSPtr l_pReturn = std::make_shared< GlShaderObject >( m_gl, this, p_eType );
	return l_pReturn;
}

std::shared_ptr< OneTextureFrameVariable > GlShaderProgram :: DoCreateTextureVariable( int p_iNbOcc )
{
	return std::make_shared< GlOneFrameVariable< TextureBaseRPtr > >( m_gl, p_iNbOcc, this );
}

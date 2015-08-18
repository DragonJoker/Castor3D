#include "GlShaderProgram.hpp"
#include "GlShaderObject.hpp"
#include "GlBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "GlOneFrameVariable.hpp"
#include "GlFrameVariableBuffer.hpp"
#include "OpenGl.hpp"
#include "GlPixelShaderSource.hpp"
#include "GlDeferredShaderSource.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlShaderProgram::GlShaderProgram( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
		:	ShaderProgramBase( p_pRenderSystem, eSHADER_LANGUAGE_GLSL )
		,	m_programObject( 0 )
		,	m_gl( p_gl )
	{
		CreateObject( eSHADER_TYPE_VERTEX );
		CreateObject( eSHADER_TYPE_PIXEL );
	}

	GlShaderProgram::~GlShaderProgram()
	{
		ShaderProgramBase::Cleanup();

		if ( m_pRenderSystem->UseShaders() )
		{
			if ( m_programObject )
			{
				m_gl.DeleteProgram( m_programObject );
				m_programObject = 0;
			}
		}
	}

	void GlShaderProgram::Initialise()
	{
		if ( m_eStatus != ePROGRAM_STATUS_LINKED )
		{
			if ( m_pRenderSystem->UseShaders() && !m_programObject )
			{
				m_programObject = m_gl.CreateProgram();
			}

			ShaderProgramBase::Initialise();
		}
	}

	bool GlShaderProgram::Link()
	{
		bool        l_bReturn       = false;
		uint32_t    l_uiNbAttached  = 0;
		int         l_iLinked       = 0;

		if ( m_pRenderSystem->UseShaders() && m_eStatus != ePROGRAM_STATUS_ERROR )
		{
			l_bReturn = true;

			for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
			{
				if ( m_pShaders[i] && m_pShaders[i]->GetStatus() == eSHADER_STATUS_COMPILED )
				{
					std::static_pointer_cast< GlShaderObject >( m_pShaders[i] )->AttachTo( this );
					l_uiNbAttached++;
				}
			}

			Logger::LogDebug( cuT( "GlShaderProgram::Link - Programs attached : %d" ), l_uiNbAttached );
			l_bReturn &= m_gl.LinkProgram( m_programObject );
			l_bReturn &= m_gl.GetProgramiv( m_programObject, eGL_SHADER_STATUS_LINK, &l_iLinked );
			Logger::LogDebug( cuT( "GlShaderProgram::Link - Program link status : %i" ), l_iLinked );
			RetrieveLinkerLog( m_linkerLog );

			if ( m_linkerLog.size() > 0 )
			{
				Logger::LogDebug( cuT( "GlShaderProgram::Link - Log : " ) + m_linkerLog );
			}

			if ( l_iLinked && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
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

	void GlShaderProgram::RetrieveLinkerLog( String & strLog )
	{
		if ( !m_pRenderSystem->UseShaders() )
		{
			strLog = m_gl.GetGlslErrorString( 0 );
		}
		else
		{
			int l_iLength = 0;
			int l_iLength2 = 0;

			if ( m_programObject == 0 )
			{
				strLog = m_gl.GetGlslErrorString( 2 );
			}
			else
			{
				m_gl.GetProgramiv( m_programObject, eGL_SHADER_STATUS_INFO_LOG_LENGTH , &l_iLength );

				if ( l_iLength > 1 )
				{
					char * l_pTmp = new char[l_iLength];
					m_gl.GetProgramInfoLog( m_programObject, l_iLength, &l_iLength2, l_pTmp );
					strLog = str_utils::from_str( l_pTmp );
					delete [] l_pTmp;
				}
			}
		}
	}

	void GlShaderProgram::Bind( uint8_t p_byIndex, uint8_t p_byCount )
	{
		if ( m_pRenderSystem->UseShaders() && m_programObject != 0 && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			m_gl.UseProgram( m_programObject );
			m_pRenderSystem->GetPipeline()->UpdateFunctions( this );
			ShaderProgramBase::Bind( p_byIndex, p_byCount );
		}
	}

	void GlShaderProgram::Unbind()
	{
		if ( m_pRenderSystem->UseShaders() && m_programObject != 0 && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			uint32_t l_index = 0;

			for ( auto l_variableBuffer: m_listFrameVariableBuffers )
			{
				l_variableBuffer->Unbind( l_index++ );
			}

			ShaderProgramBase::Unbind();
			m_gl.UseProgram( 0 );
		}
	}

	int GlShaderProgram::GetAttributeLocation( String const & p_strName )const
	{
		int l_iReturn = eGL_INVALID_INDEX;

		if ( m_programObject != eGL_INVALID_INDEX && m_gl.IsProgram( m_programObject ) )
		{
			l_iReturn = m_gl.GetAttribLocation( m_programObject, str_utils::to_str( p_strName ).c_str() );
		}

		return l_iReturn;
	}

	String GlShaderProgram::DoGetVertexShaderSource( uint32_t p_uiProgramFlags )
	{
		String	l_strReturn;
		GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
		GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
		std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );

		String l_strVersion = l_pKeywords->GetVersion();
		String l_strAttribute0 = l_pKeywords->GetAttribute( 0 );
		String l_strAttribute1 = l_pKeywords->GetAttribute( 1 );
		String l_strAttribute2 = l_pKeywords->GetAttribute( 2 );
		String l_strAttribute3 = l_pKeywords->GetAttribute( 3 );
		String l_strAttribute4 = l_pKeywords->GetAttribute( 4 );
		String l_strAttribute5 = l_pKeywords->GetAttribute( 5 );
		String l_strAttribute6 = l_pKeywords->GetAttribute( 6 );
		String l_strAttribute7 = l_pKeywords->GetAttribute( 7 );
		String l_strIn = l_pKeywords->GetIn();
		String l_strOut = l_pKeywords->GetOut();
		String l_strVertexInMatrices = l_pConstants->Matrices();
		String l_strVertexOutMatrices = l_pVariables->GetVertexOutMatrices();
		String l_strVertexMatrixCopy = l_pVariables->GetVertexMatrixCopy();
		str_utils::replace( l_strVertexOutMatrices, cuT( "out" ), l_strOut );
		l_strReturn += l_strVersion;
		l_strReturn += l_strAttribute0 + cuT( " <vec4> vertex;\n" );
		l_strReturn += l_strAttribute1 + cuT( " <vec3> normal;\n" );
		l_strReturn += l_strAttribute2 + cuT( " <vec3> tangent;\n" );
		l_strReturn += l_strAttribute3 + cuT( " <vec3> bitangent;\n" );
		l_strReturn += l_strAttribute4 + cuT( " <vec3> texture;\n" );

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
		{
			l_strReturn += l_strAttribute5 + cuT( " ivec4 bone_ids;\n" );
			l_strReturn += l_strAttribute6 + cuT( " <vec4> weights;\n" );
		}

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
		{
			l_strReturn += l_strAttribute7 + cuT( " <mat4> transform;\n" );
		}

		l_strReturn += l_strVertexInMatrices;
		l_strReturn += l_strOut + cuT( " <vec3> vtx_vertex;\n" );
		l_strReturn += l_strOut + cuT( " <vec3> vtx_normal;\n" );
		l_strReturn += l_strOut + cuT( " <vec3> vtx_tangent;\n" );
		l_strReturn += l_strOut + cuT( " <vec3> vtx_bitangent;\n" );
		l_strReturn += l_strOut + cuT( " <vec3> vtx_texture;\n" );
		l_strReturn += l_strVertexOutMatrices;
		l_strReturn += cuT( "void main()\n" );
		l_strReturn += cuT( "{\n" );
		l_strReturn += l_strVertexMatrixCopy;
		l_strReturn += cuT( "	vtx_texture = texture;\n" );
		l_strReturn += cuT( "	<vec4> l_v4Vertex = vec4( vertex.xyz, 1.0 );\n" );
		l_strReturn += cuT( "	<vec4> l_v4Normal = vec4( normal, 0.0 );\n" );
		l_strReturn += cuT( "	<vec4> l_v4Tangent = vec4( tangent, 0.0 );\n" );
		l_strReturn += cuT( "	<vec4> l_v4Bitangent = vec4( bitangent, 0.0 );\n" );

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
		{
			l_strReturn += cuT( "	<mat4> l_mtxBoneTransform = c3d_mtxBones[bone_ids[0]] * weights[0];\n" );
			l_strReturn += cuT( "	l_mtxBoneTransform += c3d_mtxBones[bone_ids[1]] * weights[1];\n" );
			l_strReturn += cuT( "	l_mtxBoneTransform += c3d_mtxBones[bone_ids[2]] * weights[2];\n" );
			l_strReturn += cuT( "	l_mtxBoneTransform += c3d_mtxBones[bone_ids[3]] * weights[3];\n" );
			//l_strReturn += cuT( "	l_mtxBoneTransform = transpose( l_mtxBoneTransform );" );
			l_strReturn += cuT( "	l_v4Vertex = l_mtxBoneTransform * l_v4Vertex;\n" );
			l_strReturn += cuT( "	l_v4Normal = l_mtxBoneTransform * l_v4Normal;\n" );
			l_strReturn += cuT( "	l_v4Tangent = l_mtxBoneTransform * l_v4Tangent;\n" );
			l_strReturn += cuT( "	l_v4Bitangent = l_mtxBoneTransform * l_v4Bitangent;\n" );
		}

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
		{
			l_strReturn += cuT( "	<mat4> l_mtxMV = c3d_mtxView * transform;\n" );
			l_strReturn += cuT( "	<mat4> l_mtxN = transpose( inverse( l_mtxMV ) );\n" );
			l_strReturn += cuT( "	l_v4Vertex = l_mtxMV * l_v4Vertex;\n" );
			l_strReturn += cuT( "	l_v4Normal = l_mtxN * l_v4Normal;\n" );
			l_strReturn += cuT( "	l_v4Tangent = l_mtxN * l_v4Tangent;\n" );
			l_strReturn += cuT( "	l_v4Bitangent = l_mtxN * l_v4Bitangent;\n" );
		}
		else
		{
			l_strReturn += cuT( "	l_v4Vertex = c3d_mtxModelView * l_v4Vertex;\n" );
			l_strReturn += cuT( "	l_v4Normal = c3d_mtxNormal * l_v4Normal;\n" );
			l_strReturn += cuT( "	l_v4Tangent = c3d_mtxNormal * l_v4Tangent;\n" );
			l_strReturn += cuT( "	l_v4Bitangent = c3d_mtxNormal * l_v4Bitangent;\n" );
		}

		l_strReturn += cuT( "	vtx_vertex = l_v4Vertex.xyz;\n" );
		l_strReturn += cuT( "	vtx_normal = normalize( l_v4Normal.xyz );\n" );
		l_strReturn += cuT( "	vtx_tangent = normalize( l_v4Tangent.xyz );\n" );
		l_strReturn += cuT( "	vtx_bitangent = normalize( l_v4Bitangent.xyz );\n" );
		l_strReturn += cuT( "	<vec4> position = c3d_mtxProjection * l_v4Vertex;\n" );
		l_strReturn += cuT( "	gl_Position = vec4( position.x, position.y, position.z, position.w );\n" );
		l_strReturn += cuT( "}\n" );
		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetLayout() );
		GLSL::ConstantsBase::Replace( l_strReturn );
		return l_strReturn;
	}

	String GlShaderProgram::DoGetPixelShaderSource( uint32_t p_uiFlags )
	{
		String	l_strReturn;
		GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
		GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
		std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );

		String l_strVersion = l_pKeywords->GetVersion();
		String l_strIn = l_pKeywords->GetIn();
		String l_strOut = l_pKeywords->GetOut();
		String l_strTexture1D = l_pKeywords->GetTexture1D();
		String l_strTexture2D = l_pKeywords->GetTexture2D();
		String l_strTexture3D = l_pKeywords->GetTexture3D();
		String l_strPixelInMatrices = l_pVariables->GetPixelInMatrices();
		String l_strPixelScene = l_pConstants->Scene();
		String l_strPixelPass = l_pConstants->Pass();
		String l_strFragOutput = l_pKeywords->GetPixelOut();
		String l_strPixelOutput = l_pKeywords->GetPixelOutputName();
		String l_strPixelLights = pixelShaderSource.GetPixelLights();
		String l_strPixelMainDeclarations = pixelShaderSource.GetPixelMainDeclarations();
		String l_strPixelMainLightsLoop = pixelShaderSource.GetPixelMainLightsLoop();
		String l_strPixelMainLightsLoopEnd = pixelShaderSource.GetPixelMainLightsLoopEnd();
		String l_strPixelMainEnd = pixelShaderSource.GetPixelMainEnd();
		String l_strPixelMainLightsLoopAfterLightDir = pixelShaderSource.GetPixelMainLightsLoopAfterLightDir();
		String l_strPixelMtxModelView = l_pVariables->GetPixelMtxModelView();
		str_utils::replace( l_strPixelInMatrices, cuT( "in" ), l_strIn );
		l_strReturn += l_strVersion;
		l_strReturn += l_strPixelInMatrices;
		l_strReturn += l_strPixelScene;
		l_strReturn += l_strPixelPass;
		l_strReturn += l_strIn + cuT( " <vec3> vtx_vertex;\n" );
		l_strReturn += l_strIn + cuT( " <vec3> vtx_normal;\n" );
		l_strReturn += l_strIn + cuT( " <vec3> vtx_tangent;\n" );
		l_strReturn += l_strIn + cuT( " <vec3> vtx_bitangent;\n" );
		l_strReturn += l_strIn + cuT( " <vec3> vtx_texture;\n" );
		l_strReturn += l_pKeywords->GetLayout( 0 ) + l_strFragOutput;
		l_strReturn += cuT( "uniform sampler1D c3d_sLights;\n" );
		l_strPixelMainDeclarations += cuT( "	" ) + l_strPixelOutput + cuT( " = vec4( 0.0, 0.0, 0.0, 0.0 );\n" );

		if ( p_uiFlags != 0 )
		{
			if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapColour;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapColour = <texture2D>( c3d_mapColour, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_v4Ambient *= l_v4MapColour;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapAmbient;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapAmbient = <texture2D>( c3d_mapAmbient, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_v4Ambient *= l_v4MapAmbient;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapDiffuse;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapDiffuse = <texture2D>( c3d_mapDiffuse, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapDiffuse;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapNormal;\n" );
				l_strPixelMainDeclarations += cuT( "	float l_fSqrLength;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapNormal = <texture2D>( c3d_mapNormal, vtx_texture.xy );\n" );
				l_strPixelMainDeclarations += cuT( "	float l_fInvRadius = 0.02;\n" );
				l_strPixelMainDeclarations += cuT( "	l_v3Normal = normalize( l_v4MapNormal.xyz * 2.0 - 1.0 );\n" );
				l_strPixelMainLightsLoop += cuT( "		Bump( vtx_tangent, vtx_bitangent, vtx_normal, l_v3LightDir, l_fAttenuation );\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapOpacity;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapOpacity = <texture2D>( c3d_mapOpacity, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopEnd += cuT( "	l_fAlpha = l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapSpecular;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapSpecular = <texture2D>( c3d_mapSpecular, vtx_texture.xy );\n" );
				l_strPixelMainLightsLoopAfterLightDir += cuT( "		l_v4TmpSpecular = l_fAttenuation * l_light.m_v4Specular * l_v4MapSpecular * l_fSpecular;\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapHeight;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapHeight = <texture2D>( c3d_mapHeight, vtx_texture.xy );\n" );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
			{
				l_strReturn += cuT( "uniform sampler2D c3d_mapGloss;\n" );
				l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapGloss = <texture2D>( c3d_mapGloss, vtx_texture.xy );\n" );
				l_strPixelMainDeclarations += cuT( "	l_fShininess = l_v4MapGloss.r;\n" );
			}
		}
		
		//l_strPixelMainLightsLoopEnd	+= cuT( "	" ) + l_strPixelOutput + cuT( " = vec4( ( l_v4Emissive + l_v4Ambient + l_v4Diffuse + l_v4Specular ).xyz, l_fAlpha );\n" );
		l_strPixelMainLightsLoopEnd	+= cuT( "	" ) + l_strPixelOutput + cuT( " = vec4( l_v4Diffuse/*( l_v4Emissive + l_v4Ambient + l_v4Diffuse + l_v4Specular )*/.xyz, l_fAlpha );\n" );
		l_strReturn += l_strPixelLights + l_strPixelMainDeclarations + l_strPixelMainLightsLoop + l_strPixelMainLightsLoopAfterLightDir + l_strPixelMainLightsLoopEnd + l_strPixelMainEnd;
		str_utils::replace( l_strReturn, cuT( "<pxlin_mtxModelView>" ), l_strPixelMtxModelView );
		str_utils::replace( l_strReturn, cuT( "<texture1D>" ), l_strTexture1D );
		str_utils::replace( l_strReturn, cuT( "<texture2D>" ), l_strTexture2D );
		str_utils::replace( l_strReturn, cuT( "<texture3D>" ), l_strTexture3D );
		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetLayout() );
		GLSL::ConstantsBase::Replace( l_strReturn );
		return l_strReturn;
	}

	String GlShaderProgram::DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass )
	{
		String	l_strReturn;
		GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
		GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
		std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );

		String l_strVersion = l_pKeywords->GetVersion();
		String l_strAttribute0 = l_pKeywords->GetAttribute( 0 );
		String l_strAttribute1 = l_pKeywords->GetAttribute( 1 );
		String l_strAttribute2 = l_pKeywords->GetAttribute( 2 );
		String l_strAttribute3 = l_pKeywords->GetAttribute( 3 );
		String l_strAttribute4 = l_pKeywords->GetAttribute( 4 );
		String l_strAttribute5 = l_pKeywords->GetAttribute( 5 );
		String l_strIn = l_pKeywords->GetIn();
		String l_strOut = l_pKeywords->GetOut();

		if ( p_bLightPass )
		{
			String l_strVertexInMatrices = l_pConstants->Matrices();
			String l_strVertexOutMatrices = l_pVariables->GetVertexOutMatrices();
			String l_strVertexMatrixCopy = l_pVariables->GetVertexMatrixCopy();
			str_utils::replace( l_strVertexOutMatrices, cuT( "out" ), l_strOut );
			l_strReturn += l_strVersion;
			l_strReturn += l_strAttribute0 + cuT( " <vec4> vertex;\n" );
			l_strReturn += l_strAttribute1 + cuT( " <vec2> texture;\n" );
			l_strReturn += l_strVertexInMatrices;
			l_strReturn += l_strOut + cuT( " <vec2> vtx_texture;\n" );
			l_strReturn += l_strVertexOutMatrices;
			l_strReturn += cuT( "void main( void )\n" );
			l_strReturn += cuT( "{\n" );
			l_strReturn += l_strVertexMatrixCopy;
			l_strReturn += cuT( "	<vec4> position = c3d_mtxProjectionModelView * vertex;\n" );
			l_strReturn += cuT( "	gl_Position = vec4( position.x, position.y, position.z, position.w );\n" );
			l_strReturn += cuT( "	vtx_texture = texture;\n" );
			l_strReturn += cuT( "}\n" );
		}
		else
		{
			String l_strVertexInMatrices = l_pConstants->Matrices();
			String l_strVertexOutMatrices = l_pVariables->GetVertexOutMatrices();
			String l_strVertexMatrixCopy = l_pVariables->GetVertexMatrixCopy();
			str_utils::replace( l_strVertexOutMatrices, cuT( "out" ), l_strOut );
			l_strReturn += l_strVersion;
			l_strReturn += l_strAttribute0 + cuT( " <vec4> vertex;\n" );
			l_strReturn += l_strAttribute1 + cuT( " <vec3> normal;\n" );
			l_strReturn += l_strAttribute2 + cuT( " <vec3> tangent;\n" );
			l_strReturn += l_strAttribute3 + cuT( " <vec3> bitangent;\n" );
			l_strReturn += l_strAttribute4 + cuT( " <vec3> texture;\n" );

			if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
			{
				l_strReturn += l_strAttribute5	+ cuT( "    <mat4>   transform;\n" );
			}

			l_strReturn += l_strVertexInMatrices;
			l_strReturn += l_strOut + cuT( " <vec3> vtx_vertex;\n" );
			l_strReturn += l_strOut + cuT( " <vec3> vtx_normal;\n" );
			l_strReturn += l_strOut + cuT( " <vec3> vtx_tangent;\n" );
			l_strReturn += l_strOut + cuT( " <vec3> vtx_bitangent;\n" );
			l_strReturn += l_strOut + cuT( " <vec3> vtx_texture;\n" );
			l_strReturn += l_strVertexOutMatrices;
			l_strReturn += cuT( "void main()\n" );
			l_strReturn += cuT( "{\n" );
			l_strReturn += l_strVertexMatrixCopy;
			l_strReturn += cuT( "	vtx_texture = texture;\n" );

			if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
			{
				l_strReturn += cuT( "	<mat4> l_mtxMV = c3d_mtxView * transform;\n" );
				l_strReturn += cuT( "	<mat4> l_mtxN = transpose( inverse( l_mtxMV ) );\n" );
				l_strReturn += cuT( "	vtx_normal = normalize( (l_mtxN * <vec4>( normal, 0.0 )).xyz );\n" );
				l_strReturn += cuT( "	vtx_tangent = normalize( (l_mtxN * <vec4>( tangent, 0.0 )).xyz );\n" );
				l_strReturn += cuT( "	vtx_bitangent = normalize( (l_mtxN * <vec4>( bitangent, 0.0 )).xyz );\n" );
				l_strReturn += cuT( "	<vec4> l_v4Vtx = l_mtxMV * vertex;\n" );
				l_strReturn += cuT( "	gl_Position = c3d_mtxProjection * l_mtxMV * vertex;\n" );
			}
			else
			{
				l_strReturn += cuT( "	vtx_normal = normalize( (c3d_mtxNormal * <vec4>( normal, 0.0 )).xyz );\n" );
				l_strReturn += cuT( "	vtx_tangent = normalize( (c3d_mtxNormal * <vec4>( tangent, 0.0 )).xyz );\n" );
				l_strReturn += cuT( "	vtx_bitangent = normalize( (c3d_mtxNormal * <vec4>( bitangent, 0.0 )).xyz );\n" );
				l_strReturn += cuT( "	<vec4> l_v4Vtx = c3d_mtxModelView * vertex;\n" );
				l_strReturn += cuT( "	gl_Position = c3d_mtxProjectionModelView * vertex;\n" );
			}

			l_strReturn += cuT( "}\n" );
		}

		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetLayout() );
		GLSL::ConstantsBase::Replace( l_strReturn );
		return l_strReturn;
	}

	String GlShaderProgram::DoGetDeferredPixelShaderSource( uint32_t p_uiFlags )
	{
		String	l_strReturn;
		GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
		GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
		std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );

		String l_strVersion = l_pKeywords->GetVersion();
		String l_strIn = l_pKeywords->GetIn();
		String l_strOut = l_pKeywords->GetOut();
		String l_strTexture1D = l_pKeywords->GetTexture1D();
		String l_strTexture2D = l_pKeywords->GetTexture2D();
		String l_strTexture3D = l_pKeywords->GetTexture3D();

		if ( ( p_uiFlags & eTEXTURE_CHANNEL_LGHTPASS ) == eTEXTURE_CHANNEL_LGHTPASS )
		{
			String l_strPixelInMatrices = l_pVariables->GetPixelInMatrices();
			String l_strPixelScene = l_pConstants->Scene();
			String l_strPixelPass = l_pConstants->Pass();
			String l_strPixelMtxModelView = l_pVariables->GetPixelMtxModelView();
			str_utils::replace( l_strPixelInMatrices, cuT( "in" ), l_strIn );
			l_strReturn += l_strVersion;
			l_strReturn += l_strPixelInMatrices;
			l_strReturn += l_strPixelScene;
			l_strReturn += l_strPixelPass;
			l_strReturn += l_pKeywords->GetPixelOut();
			l_strReturn += cuT( "uniform sampler1D c3d_sLights;\n" );
			l_strReturn += deferredShaderSource.GetLSPixelProgram();
			str_utils::replace( l_strReturn, cuT( "<pxlin_mtxModelView>" ), l_strPixelMtxModelView );
		}
		else
		{
			String l_strPixelDeclarations = deferredShaderSource.GetGSPixelDeclarations();
			String l_strPixelMainDeclarations = deferredShaderSource.GetGSPixelMainDeclarations();
			String l_strPixelMainLightsLoopAfterLightDir = deferredShaderSource.GetGSPixelMainLightsLoopAfterLightDir();
			String l_strPixelMainLightsLoopEnd;
			String l_strPixelMainEnd = deferredShaderSource.GetGSPixelMainEnd();
			String l_strPixelInMatrices = l_pConstants->Matrices();
			String l_strPixelScene = l_pConstants->Scene();
			String l_strPixelPass = l_pConstants->Pass();
			str_utils::replace( l_strPixelInMatrices, cuT( "in" ), l_strIn );
			l_strReturn += l_strVersion;
			l_strReturn += l_strPixelInMatrices;
			l_strReturn += l_strPixelScene;
			l_strReturn += l_strPixelPass;
			l_strReturn += l_strIn + cuT( " <vec3> vtx_vertex;\n" );
			l_strReturn += l_strIn + cuT( " <vec3> vtx_normal;\n" );
			l_strReturn += l_strIn + cuT( " <vec3> vtx_tangent;\n" );
			l_strReturn += l_strIn + cuT( " <vec3> vtx_bitangent;\n" );
			l_strReturn += l_strIn + cuT( " <vec3> vtx_texture;\n" );
			l_strReturn += l_strPixelDeclarations;
			l_strReturn += l_pKeywords->GetGSOutPositionDecl();
			l_strReturn += l_pKeywords->GetGSOutDiffuseDecl();
			l_strReturn += l_pKeywords->GetGSOutNormalDecl();
			l_strReturn += l_pKeywords->GetGSOutTangentDecl();
			l_strReturn += l_pKeywords->GetGSOutBitangentDecl();
			l_strReturn += l_pKeywords->GetGSOutSpecularDecl();
			l_strReturn += l_pKeywords->GetGSOutEmissiveDecl();

			if ( p_uiFlags != 0 )
			{
				if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					l_strReturn += cuT( "uniform sampler2D c3d_mapColour;\n" );
					l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapColour = <texture2D>( c3d_mapColour, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapColour;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
				{
					l_strReturn += cuT( "uniform sampler2D c3d_mapAmbient;\n" );
					l_strPixelMainDeclarations += cuT( "	vec4    l_v4MapAmbient = <texture2D>( c3d_mapAmbient, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapAmbient;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
				{
					l_strReturn += cuT( "uniform sampler2D c3d_mapDiffuse;\n" );
					l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapDiffuse = <texture2D>( c3d_mapDiffuse, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Diffuse *= l_v4MapDiffuse;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					l_strReturn += cuT( "uniform sampler2D c3d_mapNormal;\n" );
					l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapNormal = <texture2D>( c3d_mapNormal, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
					l_strPixelMainDeclarations += cuT( "	l_v4Normal += vec4( normalize( (l_v4MapNormal.xyz * 2.0 - 1.0) ), 0 );\n" );
					l_strPixelMainDeclarations += cuT( "	l_v4Tangent -= vec4( l_v4Normal.xyz * dot( l_v4Tangent.xyz, l_v4Normal.xyz ), 0 );\n" );
					l_strPixelMainDeclarations += cuT( "	l_v4Bitangent = vec4( cross( l_v4Normal.xyz, l_v4Tangent.xyz ), 1 );\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					l_strReturn += cuT( "uniform sampler2D c3d_mapOpacity;\n" );
					l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapOpacity = <texture2D>( c3d_mapOpacity, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_fAlpha = l_v4MapOpacity.r * c3d_fMatOpacity;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					l_strReturn += cuT( "uniform sampler2D c3d_mapSpecular;\n" );
					l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapSpecular = <texture2D>( c3d_mapSpecular, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Specular.xyz *= l_v4MapSpecular.xyz;\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
				{
					l_strReturn += cuT( "uniform sampler2D c3d_mapHeight;\n" );
					l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapHeight = <texture2D>( c3d_mapHeight, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
				{
					l_strReturn += cuT( "uniform sampler2D c3d_mapGloss;\n" );
					l_strPixelMainDeclarations += cuT( "	vec4 l_v4MapGloss = <texture2D>( c3d_mapGloss, vec2( vtx_texture.x, vtx_texture.y ) );\n" );
					l_strPixelMainLightsLoopEnd += cuT( "	l_v4Specular.w *= l_v4MapGloss.x;\n" );
				}
			}

			l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutPositionName() + cuT( " = vec4( l_v4Position.xyz, 1 );\n" );
			l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutDiffuseName() + cuT( " = vec4( l_v4Diffuse.xyz, 1 );\n" );
			l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutNormalName() + cuT( " = vec4( l_v4Normal.xyz, 1 );\n" );
			l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutTangentName() + cuT( " = vec4( l_v4Tangent.xyz, 1 );\n" );
			l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutBitangentName() + cuT( " = vec4( l_v4Bitangent.xyz, 1 );\n" );
			l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutSpecularName() + cuT( " = vec4( l_v4Specular );\n" );
			l_strPixelMainLightsLoopEnd += cuT( "	" ) + l_pKeywords->GetGSOutEmissiveName() + cuT( " = vec4( l_v4Emissive );\n" );
			l_strReturn += l_strPixelMainDeclarations + l_strPixelMainLightsLoopAfterLightDir + l_strPixelMainLightsLoopEnd + l_strPixelMainEnd;
			//		Logger::LogDebug( l_strReturn );
		}

		str_utils::replace( l_strReturn, cuT( "<texture1D>" ), l_strTexture1D );
		str_utils::replace( l_strReturn, cuT( "<texture2D>" ), l_strTexture2D );
		str_utils::replace( l_strReturn, cuT( "<texture3D>" ), l_strTexture3D );
		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetLayout() );
		GLSL::ConstantsBase::Replace( l_strReturn );
		return l_strReturn;
	}

	ShaderObjectBaseSPtr GlShaderProgram::DoCreateObject( eSHADER_TYPE p_eType )
	{
		ShaderObjectBaseSPtr l_pReturn = std::make_shared< GlShaderObject >( m_gl, this, p_eType );
		return l_pReturn;
	}

	std::shared_ptr< OneTextureFrameVariable > GlShaderProgram::DoCreateTextureVariable( int p_iNbOcc )
	{
		return std::make_shared< GlOneFrameVariable< TextureBaseRPtr > >( m_gl, p_iNbOcc, this );
	}
}

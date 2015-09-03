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
		bool l_bReturn = false;
		uint32_t l_uiNbAttached  = 0;
		int l_iLinked = 0;

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

			Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << l_uiNbAttached );
			l_bReturn &= m_gl.LinkProgram( m_programObject );
			l_bReturn &= m_gl.GetProgramiv( m_programObject, eGL_SHADER_STATUS_LINK, &l_iLinked );
			Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Program link status : " ) << l_iLinked );
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
		using namespace GLSL;

		GlslWriter l_writer( m_gl, eSHADER_TYPE_VERTEX );
		l_writer << Version() << Endl();
		// Vertex inputs
		ATTRIBUTE( l_writer, Vec4, vertex );
		ATTRIBUTE( l_writer, Vec3, normal );
		ATTRIBUTE( l_writer, Vec3, tangent );
		ATTRIBUTE( l_writer, Vec3, bitangent );
		ATTRIBUTE( l_writer, Vec3, texture );
		IVec4 bone_ids;
		Vec4 weights;
		Mat4 transform;

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
		{
			bone_ids = l_writer.GetAttribute< IVec4 >( cuT( "bone_ids" ) );
			weights = l_writer.GetAttribute< Vec4 >( cuT( "weights" ) );
		}

		if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
		{
			transform = l_writer.GetAttribute< Mat4 >( cuT( "transform" ) );
		}

		UBO_MATRIX( l_writer );

		// Outputs
		OUT( l_writer, Vec3, vtx_vertex );
		OUT( l_writer, Vec3, vtx_normal );
		OUT( l_writer, Vec3, vtx_tangent );
		OUT( l_writer, Vec3, vtx_bitangent );
		OUT( l_writer, Vec3, vtx_texture );
		l_writer << Legacy_MatrixOut();

		std::function< void() > l_main = [&]()
		{
			l_writer << Legacy_MatrixCopy();
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Vertex, vec4( vertex.xyz(), 1.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Normal, vec4( normal, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Tangent, vec4( tangent, 0.0 ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Bitangent, vec4( bitangent, 0.0 ) );

			if ( ( p_uiProgramFlags & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
			{
				LOCALE( l_writer, Mat4, l_mtxBoneTransform );
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 0 )]] * weights[Int( 0 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 1 )]] * weights[Int( 1 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 2 )]] * weights[Int( 2 )];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids[Int( 3 )]] * weights[Int( 3 )];
				//l_mtxBoneTransform = transpose( l_mtxBoneTransform );
				l_v4Vertex = l_mtxBoneTransform * l_v4Vertex;
				l_v4Normal = l_mtxBoneTransform * l_v4Normal;
				l_v4Tangent = l_mtxBoneTransform * l_v4Tangent;
				l_v4Bitangent = l_mtxBoneTransform * l_v4Bitangent;
			}

			if ( ( p_uiProgramFlags & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
			{
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxMV, c3d_mtxView * transform );
				LOCALE_ASSIGN( l_writer, Mat4, l_mtxN, transpose( inverse( l_mtxMV ) ) );
				l_v4Vertex = l_mtxMV * l_v4Vertex;
				l_v4Normal = l_mtxN * l_v4Normal;
				l_v4Tangent = l_mtxN * l_v4Tangent;
				l_v4Bitangent = l_mtxN * l_v4Bitangent;
			}
			else
			{
				l_v4Vertex = c3d_mtxModelView * l_v4Vertex;
				l_v4Normal = c3d_mtxNormal * l_v4Normal;
				l_v4Tangent = c3d_mtxNormal * l_v4Tangent;
				l_v4Bitangent = c3d_mtxNormal * l_v4Bitangent;
			}

			vtx_texture = texture;
			vtx_vertex = l_v4Vertex.xyz();
			vtx_normal = normalize( l_v4Normal.xyz() );
			vtx_tangent = normalize( l_v4Tangent.xyz() );
			vtx_bitangent = normalize( l_v4Bitangent.xyz() );
			BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * l_v4Vertex;
		};

		l_writer.Implement_Function< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String GlShaderProgram::DoGetPixelShaderSource( uint32_t p_uiFlags )
	{
		using namespace GLSL;

		GlslWriter l_writer( m_gl, eSHADER_TYPE_PIXEL );
		l_writer << Version() << Endl();

		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Pixel inputs
		IN( l_writer, Vec3, vtx_vertex );
		IN( l_writer, Vec3, vtx_normal );
		IN( l_writer, Vec3, vtx_tangent );
		IN( l_writer, Vec3, vtx_bitangent );
		IN( l_writer, Vec3, vtx_texture );

		LAYOUT( l_writer, Vec4, pxl_v4FragColor );
		UNIFORM( l_writer, Sampler1D, c3d_sLights );
		Sampler2D c3d_mapColour;
		Sampler2D c3d_mapAmbient;
		Sampler2D c3d_mapDiffuse;
		Sampler2D c3d_mapNormal;
		Sampler2D c3d_mapOpacity;
		Sampler2D c3d_mapSpecular;
		Sampler2D c3d_mapHeight;
		Sampler2D c3d_mapGloss;

		if ( p_uiFlags != 0 )
		{
			if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
			{
				c3d_mapColour = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapColour" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
			{
				c3d_mapAmbient = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapAmbient" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
			{
				c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapDiffuse" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
			{
				c3d_mapNormal = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapNormal" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			{
				c3d_mapOpacity = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapOpacity" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
			{
				c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapSpecular" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_HEIGHT ) == eTEXTURE_CHANNEL_HEIGHT )
			{
				c3d_mapHeight = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapHeight" ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
			{
				c3d_mapGloss = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapGloss" ) );
			}
		}

		Lighting< BlinnPhongLightingModel > l_lighting;
		l_lighting.Declare_Light( l_writer );
		l_lighting.Declare_GetLight( l_writer );
		l_lighting.Declare_ComputeLightDirection( l_writer );
		l_lighting.Declare_ComputeFresnel( l_writer );
		
		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( &l_writer, 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( 0.0f ), 0, 0 ) );
			//LOCALE_ASSIGN( l_writer, Vec3, l_v3EyeVec, normalize( vec3( vtx_vertex.x(), vtx_vertex.y(), vtx_vertex.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3EyeVec, normalize( c3d_v3CameraPosition - vec3( vtx_vertex.x(), vtx_vertex.y(), vtx_vertex.z() ) ) );
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );
			LOCALE_ASSIGN( l_writer, Float, l_fShininess, c3d_fMatShininess );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.xyz() );
			pxl_v4FragColor = vec4( Float( &l_writer, 0.0f ), 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapSpecular( &l_writer, cuT( "l_v3MapSpecular" ) );

			if ( p_uiFlags != 0 )
			{
				if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapNormal, texture2D( c3d_mapNormal, vtx_texture.xy() ).xyz() );
					l_v3Normal = normalize( l_v3MapNormal.xyz() * Float( 2.0f ) - Float( 1.0f ) );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapSpecular, texture2D( c3d_mapSpecular, vtx_texture.xy() ).xyz() );
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_GLOSS ) == eTEXTURE_CHANNEL_GLOSS )
				{
					l_fShininess = texture2D( c3d_mapGloss, vtx_texture.xy() ).r();
				}
			}

			FOR( l_writer, Int, i, 0, cuT( "i < c3d_iLightsCount" ), cuT( "++i" ) )
			{
				LOCALE_ASSIGN( l_writer, Light, l_light, l_lighting.GetLight( i ) );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4LightDir, l_lighting.ComputeLightDirection( l_light, vtx_vertex, c3d_mtxModelView ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3LightDir, l_v4LightDir.xyz() );
				LOCALE_ASSIGN( l_writer, Float, l_fAttenuation, l_v4LightDir.w() );

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_NORMAL ) == eTEXTURE_CHANNEL_NORMAL )
				{
					l_lighting.Bump( vtx_tangent, vtx_bitangent, vtx_normal, l_v3LightDir, l_fAttenuation );
				}

				LOCALE_ASSIGN( l_writer, Float, l_fLambert, max( dot( l_v3Normal, l_v3LightDir ), 0.0 ) );
				//l_fLambert = dot( l_v3Normal, -l_v3LightDir );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3MatSpecular, c3d_v4MatSpecular.xyz() );
				LOCALE_ASSIGN( l_writer, Float, l_fFresnel, l_lighting.ComputeFresnel( l_fLambert, l_v3LightDir, l_v3Normal, l_v3EyeVec, l_fShininess, l_v3MatSpecular ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpAmbient, ( l_light.m_v4Ambient().xyz() * l_light.m_v4Ambient().w() * c3d_v4MatAmbient.xyz() ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpDiffuse, ( l_light.m_v4Diffuse().xyz() * l_light.m_v4Diffuse().w() * c3d_v4MatDiffuse.xyz() * l_fLambert ) / l_fAttenuation );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3TmpSpecular, ( l_light.m_v4Specular().xyz() * l_light.m_v4Specular().w() * l_v3MatSpecular * l_fFresnel ) / l_fAttenuation );

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_SPECULAR ) == eTEXTURE_CHANNEL_SPECULAR )
				{
					l_v3TmpSpecular = l_fAttenuation * l_light.m_v4Specular().xyz() * l_v3MapSpecular * l_v3TmpSpecular;
				}

				l_v3Ambient += l_v3TmpAmbient;
				l_v3Diffuse += l_v3TmpDiffuse;
				l_v3Specular += l_v3TmpSpecular;
			}
			ROF

			if ( p_uiFlags != 0 )
			{
				if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					l_v3Ambient *= texture2D( c3d_mapColour, vtx_texture.xy() ).xyz();
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_AMBIENT ) == eTEXTURE_CHANNEL_AMBIENT )
				{
					l_v3Ambient *= texture2D( c3d_mapAmbient, vtx_texture.xy() ).xyz();
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_DIFFUSE ) == eTEXTURE_CHANNEL_DIFFUSE )
				{
					l_v3Diffuse *= texture2D( c3d_mapDiffuse, vtx_texture.xy() ).xyz();
				}

				if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					l_fAlpha = texture2D( c3d_mapOpacity, vtx_texture.xy() ).r() * c3d_fMatOpacity;
				}
			}

			//pxl_v4FragColor = vec4( l_v3Emissive + l_v3Ambient + l_v3Diffuse + l_v3Specular, l_fAlpha );
			pxl_v4FragColor = vec4( l_v3Diffuse, l_fAlpha );
		};
		l_writer.Implement_Function< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
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

		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetStdLayout( 140 ) );
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
		str_utils::replace( l_strReturn, cuT( "<layout>" ), l_pKeywords->GetStdLayout( 140 ) );
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

#include "GlShaderProgram.hpp"

#include "GlShaderObject.hpp"
#include "GlShaderSource.hpp"
#include "GlBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "GlOneFrameVariable.hpp"
#include "GlFrameVariableBuffer.hpp"
#include "OpenGl.hpp"

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
	}

	void GlShaderProgram::Cleanup()
	{
		ShaderProgramBase::Cleanup();

		if ( m_renderSystem->UseShaders() )
		{
			if ( m_programObject )
			{
				glUntrack( m_gl, this );
				m_gl.DeleteProgram( m_programObject );
				m_programObject = 0;
			}
		}
	}

	void GlShaderProgram::Initialise()
	{
		if ( m_eStatus != ePROGRAM_STATUS_LINKED )
		{
			if ( m_renderSystem->UseShaders() && !m_programObject )
			{
				m_programObject = m_gl.CreateProgram();
				glTrack( m_gl, GlShaderProgram, this );
			}

			ShaderProgramBase::Initialise();
		}
	}

	bool GlShaderProgram::Link()
	{
		bool l_return = false;
		int l_iLinked = 0;

		if ( m_renderSystem->UseShaders() && m_eStatus != ePROGRAM_STATUS_ERROR )
		{
			l_return = true;
			Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << uint32_t( m_activeShaders.size() ) );
			l_return &= m_gl.LinkProgram( m_programObject );
			l_return &= m_gl.GetProgramiv( m_programObject, eGL_SHADER_STATUS_LINK, &l_iLinked );
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

			l_return = m_eStatus == ePROGRAM_STATUS_LINKED;
		}

		return l_return;
	}

	void GlShaderProgram::RetrieveLinkerLog( String & strLog )
	{
		if ( !m_renderSystem->UseShaders() )
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
					strLog = string::string_cast< xchar >( l_pTmp );
					delete [] l_pTmp;
				}
			}
		}
	}

	void GlShaderProgram::Bind( uint8_t p_byIndex, uint8_t p_byCount )
	{
		if ( m_renderSystem->UseShaders() && m_programObject != 0 && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			m_gl.UseProgram( m_programObject );
			ShaderProgramBase::Bind( p_byIndex, p_byCount );
		}
	}

	void GlShaderProgram::Unbind()
	{
		if ( m_renderSystem->UseShaders() && m_programObject != 0 && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			uint32_t l_index = 0;

			for ( auto l_variableBuffer : m_listFrameVariableBuffers )
			{
				l_variableBuffer->Unbind( l_index++ );
			}

			ShaderProgramBase::Unbind();
			m_gl.UseProgram( 0 );
		}
	}

	int GlShaderProgram::GetAttributeLocation( String const & p_name )const
	{
		int l_iReturn = eGL_INVALID_INDEX;

		if ( m_programObject != eGL_INVALID_INDEX && m_gl.IsProgram( m_programObject ) )
		{
			l_iReturn = m_gl.GetAttribLocation( m_programObject, string::string_cast< char >( p_name ).c_str() );
		}

		return l_iReturn;
	}

	ShaderObjectBaseSPtr GlShaderProgram::DoCreateObject( eSHADER_TYPE p_type )
	{
		ShaderObjectBaseSPtr l_pReturn = std::make_shared< GlShaderObject >( m_gl, this, p_type );
		return l_pReturn;
	}

	std::shared_ptr< OneTextureFrameVariable > GlShaderProgram::DoCreateTextureVariable( int p_iNbOcc )
	{
		return std::make_shared< GlOneFrameVariable< TextureBaseRPtr > >( m_gl, p_iNbOcc, this );
	}

	String GlShaderProgram::DoGetVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();
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

}

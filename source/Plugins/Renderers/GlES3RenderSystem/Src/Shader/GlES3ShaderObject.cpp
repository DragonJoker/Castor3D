#include "Shader/GlES3ShaderObject.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"
#include "Shader/GlES3ShaderProgram.hpp"

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3ShaderObject::GlES3ShaderObject( OpenGlES3 & p_gl, GlES3ShaderProgram * p_parent, ShaderType p_type )
		: ShaderObject( p_parent, p_type )
		, Object( p_gl,
				  "GlES3ShaderObject",
				  std::bind( &OpenGlES3::CreateShader, std::ref( p_gl ), p_gl.Get( p_type ) ),
				  std::bind( &OpenGlES3::DeleteShader, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGlES3::IsShader, std::ref( p_gl ), std::placeholders::_1 )
				)
		, m_shaderProgram( nullptr )
	{
	}

	GlES3ShaderObject::~GlES3ShaderObject()
	{
	}

	bool GlES3ShaderObject::Create()
	{
		return ObjectType::Create();
	}

	void GlES3ShaderObject::Destroy()
	{
		Detach();

		if ( m_status == ShaderStatus::eCompiled )
		{
			ObjectType::Destroy();
			m_status = ShaderStatus::eNotCompiled;
		}
	}

	bool GlES3ShaderObject::Compile()
	{
		bool l_return = false;
		String l_loadedSource;

		for ( size_t i = size_t( ShaderModel::eModel5 ); i >= size_t( ShaderModel::eModel1 ) && l_loadedSource.empty() && i < m_arraySources.size(); i-- )
		{
			if ( m_parent->GetRenderSystem()->GetGpuInformations().CheckSupport( ShaderModel( i ) ) )
			{
				l_loadedSource = m_arraySources[i];

				if ( !l_loadedSource.empty() )
				{
					m_eShaderModel = ShaderModel( i );
				}
			}
		}

		if ( m_status != ShaderStatus::eError && !l_loadedSource.empty() && l_loadedSource != m_loadedSource )
		{
			l_return = true;
			m_loadedSource = l_loadedSource;

			if ( m_parent->GetRenderSystem()->GetGpuInformations().HasShaderType( m_type ) )
			{
				m_status = ShaderStatus::eNotCompiled;
				int l_compiled = 0;
				int l_iLength = int( m_loadedSource.size() );
				std::string l_tmp = string::string_cast< char >( m_loadedSource );
				std::vector< char > l_pszTmp( m_loadedSource.size() + 1 );
				char * l_buffer = l_pszTmp.data();
#if defined( _MSC_VER )
				strncpy_s( l_buffer, m_loadedSource.size() + 1, l_tmp.c_str(), l_tmp.size() );
#else
				strncpy( l_buffer, l_tmp.c_str(), l_tmp.size() );
#endif
				GetOpenGlES3().ShaderSource( GetGlES3Name(), 1, const_cast< const char ** >( &l_buffer ), &l_iLength );
				GetOpenGlES3().CompileShader( GetGlES3Name() );
				GetOpenGlES3().GetShaderiv( GetGlES3Name(), GlES3ShaderStatus::eCompile, &l_compiled );
				Logger::LogDebug( StringStream() << cuT( "GlES3ShaderObject:: Compile - Shader compilation status : " ) << l_compiled );

				if ( l_compiled )
				{
					m_status = ShaderStatus::eCompiled;
				}
				else
				{
					m_status = ShaderStatus::eError;
				}

				l_return = DoCheckErrors();
			}
			else
			{
				Logger::LogError( "GlES3ShaderObject::Compile - Shader type not supported by currently loaded API." );
			}
		}
		else if ( m_loadedSource.empty() )
		{
			Logger::LogError( "GlES3ShaderObject::Compile - No shader source." );
		}
		else
		{
			Logger::LogWarning( "GlES3ShaderObject::Compile - Shader is already compiled." );
		}

		return l_return;
	}

	void GlES3ShaderObject::Detach()
	{
		if ( m_status == ShaderStatus::eCompiled && m_shaderProgram && m_parent->GetRenderSystem()->GetGpuInformations().HasShaderType( m_type ) )
		{
			GetOpenGlES3().DetachShader( m_shaderProgram->GetGlES3Name(), GetGlES3Name() );
			m_shaderProgram = nullptr;
		}
	}

	void GlES3ShaderObject::AttachTo( ShaderProgram & p_program )
	{
		Detach();

		if ( m_status == ShaderStatus::eCompiled && m_parent->GetRenderSystem()->GetGpuInformations().HasShaderType( m_type ) )
		{
			m_shaderProgram = &static_cast< GlES3ShaderProgram & >( p_program );
			GetOpenGlES3().AttachShader( m_shaderProgram->GetGlES3Name(), GetGlES3Name() );
		}
	}

	String GlES3ShaderObject::DoRetrieveCompilerLog()
	{
		String l_log;
		int l_infologLength = 0;
		int l_charsWritten = 0;
		GetOpenGlES3().GetShaderiv( GetGlES3Name(), GlES3ShaderStatus::eInfoLogLength, &l_infologLength );

		if ( l_infologLength > 0 )
		{
			std::vector< char > l_infoLog( l_infologLength + 1 );
			GetOpenGlES3().GetShaderInfoLog( GetGlES3Name(), l_infologLength, &l_charsWritten, l_infoLog.data() );
			l_log = string::string_cast< xchar >( l_infoLog.data() );
		}

		if ( !l_log.empty() )
		{
			l_log = l_log.substr( 0, l_log.size() - 1 );
		}

		return l_log;
	}
}

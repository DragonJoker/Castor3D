#include "PrecompiledHeader.h"

#include "GLSLProgram.h"
#include "GLSLObject.h"
#include "GLUniformVariable.h"
#include "GLRenderSystem.h"
#include "GLContext.h"

using namespace Castor3D;

GLShaderProgram :: GLShaderProgram()
	:	ShaderProgram(),
		m_programObject( 0)
{
}

GLShaderProgram :: GLShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile),
		m_programObject( 0)
{
}

GLShaderProgram :: ~GLShaderProgram()
{
	if (GLRenderSystem::UseShaders())
	{
		Cleanup();

		if (m_programObject != 0)
		{
			glDeleteProgram( m_programObject);
			CheckGLError( "GLShaderProgram :: ~GLShaderProgram - glDeleteShader");
		}
	}
}

void GLShaderProgram :: Cleanup()
{
	m_pVertexShader.reset();
	m_pFragmentShader.reset();
	m_pGeometryShader.reset();
}

void GLShaderProgram :: Initialise()
{
	if ( ! m_isLinked)
	{
		if (GLRenderSystem::UseShaders())
		{
			m_programObject = glCreateProgram();
		}

		ShaderProgram::Initialise();
	}
}

bool GLShaderProgram :: Link()
{
	bool l_bReturn = false;

	if (GLRenderSystem::UseShaders() && ! m_bError)
	{
		if (m_usesGeometryShader)
		{
			glProgramParameteriEXT( m_programObject, GL_GEOMETRY_INPUT_TYPE_EXT, m_nInputPrimitiveType);
			glProgramParameteriEXT( m_programObject, GL_GEOMETRY_OUTPUT_TYPE_EXT, m_nOutputPrimitiveType);
			glProgramParameteriEXT( m_programObject, GL_GEOMETRY_VERTICES_OUT_EXT, m_nVerticesOut);
		}

		if (m_isLinked)  // already linked, detach everything first
		{
			Log::LogMessage( "GLShaderProgram :: Link - **warning** Object is already linked, trying to link again");

			GLShaderObjectPtr( m_pVertexShader)->Detach();
			GLShaderObjectPtr( m_pFragmentShader)->Detach();

			if ( ! m_pGeometryShader.null())
			{
				GLShaderObjectPtr( m_pGeometryShader)->Detach();
			}
		}

		GLShaderObjectPtr( m_pVertexShader)->AttachTo( this);
		GLShaderObjectPtr( m_pFragmentShader)->AttachTo( this);

		if ( ! m_pGeometryShader.null())
		{
			GLShaderObjectPtr( m_pGeometryShader)->AttachTo( this);
		}

		GLint linked = 0;
		glLinkProgram( m_programObject);
		CheckGLError( "GLShaderProgram :: Link - glLinkProgram");
		glGetProgramiv( m_programObject, GL_LINK_STATUS, & linked);
		CheckGLError( "GLShaderProgram :: Link - glGetProgramiv");

		if (linked)
		{
			m_isLinked = true;
		}
		else
		{
			m_bError = true;
			String l_strLog;
			RetrieveLinkerLog( l_strLog);
			Log::LogMessage( "GLShaderProgram :: Link - **linker error**");
		}

		l_bReturn = m_isLinked;
	}

	return l_bReturn;
}

void GLShaderProgram :: RetrieveLinkerLog( String & strLog)
{
	if ( ! GLRenderSystem::UseShaders())
	{
		strLog = GLRenderSystem::GetGLSLString( 0);
		return;
	}

	GLint l_iLength = 0;
	GLsizei l_uiLength = 0;

	if (m_programObject == 0)
	{
		strLog = GLRenderSystem::GetGLSLString( 2);
		return;
	}

	glGetProgramiv(m_programObject, GL_INFO_LOG_LENGTH , & l_iLength);
	CheckGLError( "GLShaderProgram :: GetLinkerLog - glGetProgramiv");

	if (l_iLength > 1)
	{
		char * l_pTmp = new char[l_iLength];
		glGetProgramInfoLog(m_programObject, l_iLength, & l_uiLength, l_pTmp);
		CheckGLError( "GLShaderProgram :: GetLinkerLog - glGetProgramInfoLog");
		m_linkerLog.assign( l_pTmp);
		delete [] l_pTmp;
	}

	strLog = m_linkerLog;
}

void GLShaderProgram :: Begin()
{
	if ( ! GLRenderSystem::UseShaders() || m_programObject == 0 || ! m_enabled || ! m_isLinked || m_bError)
	{
		return;
	}

	glUseProgram( m_programObject);
	CheckGLError( "GLShaderProgram :: End - glUseProgram");

	ApplyAllVariables();
}

void GLShaderProgram :: ApplyAllVariables()
{
	for (GLUniformVariablePtrStrMap::iterator l_it = m_mapGLUniformVariables.begin() ; l_it != m_mapGLUniformVariables.end() ; ++l_it)
	{
		l_it->second->Apply();
	}
}

void GLShaderProgram :: End()
{
	if ( ! GLRenderSystem::UseShaders() || ! m_enabled)
	{
		return;
	}

	glUseProgram( 0);
	CheckGLError( "GLShaderProgram :: End - glUseProgram");
}

GLUniformVariablePtr GLShaderProgram :: _createGLUniformVariable( UniformVariablePtr p_pUniformVariable)
{
	GLUniformVariablePtr l_pReturn;

	switch (p_pUniformVariable->GetType())
	{
	case UniformVariable::eOne:
		if (p_pUniformVariable->GetSubType() == typeid( float))
		{
			l_pReturn = new GLFloatUniformVariable( (OneUniformVariable<float> *)p_pUniformVariable.get(), & m_programObject);
		}
		else
		{
			l_pReturn = new GLIntUniformVariable( (OneUniformVariable<int> *)p_pUniformVariable.get(), & m_programObject);
		}
		break;
		
	case UniformVariable::eVec2:
		if (p_pUniformVariable->GetSubType() == typeid( float))
		{
			l_pReturn = new GLFloatsUniformVariable<2>( (PointUniformVariable<float, 2> *)p_pUniformVariable.get(), & m_programObject);
		}
		else
		{
			l_pReturn = new GLIntsUniformVariable<2>( (PointUniformVariable<int, 2> *)p_pUniformVariable.get(), & m_programObject);
		}
		break;
		
	case UniformVariable::eVec3:
		if (p_pUniformVariable->GetSubType() == typeid( float))
		{
			l_pReturn = new GLFloatsUniformVariable<3>( (PointUniformVariable<float, 3> *)p_pUniformVariable.get(), & m_programObject);
		}
		else
		{
			l_pReturn = new GLIntsUniformVariable<3>( (PointUniformVariable<int, 3> *)p_pUniformVariable.get(), & m_programObject);
		}
		break;
		
	case UniformVariable::eVec4:
		if (p_pUniformVariable->GetSubType() == typeid( float))
		{
			l_pReturn = new GLFloatsUniformVariable<4>( (PointUniformVariable<float, 4> *)p_pUniformVariable.get(), & m_programObject);
		}
		else
		{
			l_pReturn = new GLIntsUniformVariable<4>( (PointUniformVariable<int, 4> *)p_pUniformVariable.get(), & m_programObject);
		}
		break;
		
	case UniformVariable::eMat2:
		l_pReturn = new GLMatrixUniformVariable<2>( (MatrixUniformVariable<float, 2> *)p_pUniformVariable.get(), & m_programObject);
		break;
		
	case UniformVariable::eMat3:
		l_pReturn = new GLMatrixUniformVariable<3>( (MatrixUniformVariable<float, 3> *)p_pUniformVariable.get(), & m_programObject);
		break;
		
	case UniformVariable::eMat4:
		l_pReturn = new GLMatrixUniformVariable<4>( (MatrixUniformVariable<float, 4> *)p_pUniformVariable.get(), & m_programObject);
		break;
	}

	return l_pReturn;
}

void GLShaderProgram :: AddUniformVariable( UniformVariablePtr p_pUniformVariable)
{
	ShaderProgram::AddUniformVariable( p_pUniformVariable);

	if ( ! p_pUniformVariable.null())
	{
		GLUniformVariablePtr l_pGLUniformVariable = _createGLUniformVariable( p_pUniformVariable);

		if (m_mapGLUniformVariables.find( p_pUniformVariable->GetName()) == m_mapGLUniformVariables.end())
		{
			m_mapGLUniformVariables.insert( GLUniformVariablePtrStrMap::value_type( p_pUniformVariable->GetName(), l_pGLUniformVariable));
		}
	}
}

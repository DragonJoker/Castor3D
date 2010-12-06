#include "PrecompiledHeader.h"

#include "OpenGLCommon/GLShaderProgram.h"
#include "OpenGLCommon/GLShaderObject.h"
#include "OpenGLCommon/GLFrameVariable.h"

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
	m_mapGLFrameVariables.clear();
	Cleanup();

	if (RenderSystem::UseShaders())
	{

		if (m_programObject != 0)
		{
			glDeleteProgram( m_programObject);
			CheckGLError( CU_T( "GLShaderProgram :: ~GLShaderProgram - glDeleteShader"));
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
		if (RenderSystem::UseShaders())
		{
			m_programObject = glCreateProgram();
		}

		ShaderProgram::Initialise();
	}
}

bool GLShaderProgram :: Link()
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError)
	{
		if (m_usesGeometryShader)
		{
			glProgramParameteriEXT( m_programObject, GL_GEOMETRY_INPUT_TYPE_EXT, m_nInputPrimitiveType);
			glProgramParameteriEXT( m_programObject, GL_GEOMETRY_OUTPUT_TYPE_EXT, m_nOutputPrimitiveType);
			glProgramParameteriEXT( m_programObject, GL_GEOMETRY_VERTICES_OUT_EXT, m_nVerticesOut);
		}

		if (m_isLinked)  // already linked, detach everything first
		{
			Logger::LogWarning( CU_T( "GLShaderProgram :: Link - Object is already linked, trying to link again"));

			static_pointer_cast<GLShaderObject>( m_pVertexShader)->Detach();
			static_pointer_cast<GLShaderObject>( m_pFragmentShader)->Detach();

			if ( ! m_pGeometryShader.null())
			{
				static_pointer_cast<GLShaderObject>( m_pGeometryShader)->Detach();
			}
		}

		static_pointer_cast<GLShaderObject>( m_pVertexShader)->AttachTo( this);
		static_pointer_cast<GLShaderObject>( m_pFragmentShader)->AttachTo( this);

		if ( ! m_pGeometryShader.null())
		{
			static_pointer_cast<GLShaderObject>( m_pGeometryShader)->AttachTo( this);
		}

		GLint linked = 0;
		glLinkProgram( m_programObject);
		CheckGLError( CU_T( "GLShaderProgram :: Link - glLinkProgram"));
		glGetProgramiv( m_programObject, GL_LINK_STATUS, & linked);
		CheckGLError( CU_T( "GLShaderProgram :: Link - glGetProgramiv"));
		glGetProgramiv( m_programObject, GL_VALIDATE_STATUS, & linked);

		RetrieveLinkerLog( m_linkerLog);

		if (m_linkerLog.size() > 0)
		{
			Logger::LogMessage( m_linkerLog);
		}

		if (linked && m_linkerLog.find( "ERROR") == String::npos)
		{
			m_isLinked = true;
		}
		else
		{
			m_bError = true;
		}

		l_bReturn = m_isLinked;
	}

	return l_bReturn;
}

void GLShaderProgram :: RetrieveLinkerLog( String & strLog)
{
	if ( ! RenderSystem::UseShaders())
	{
		strLog = GetGLSLErrorString( 0);
		return;
	}

	GLint l_iLength = 0;
	GLsizei l_uiLength = 0;

	if (m_programObject == 0)
	{
		strLog = GetGLSLErrorString( 2);
		return;
	}

	glGetProgramiv(m_programObject, GL_INFO_LOG_LENGTH , & l_iLength);
	CheckGLError( CU_T( "GLShaderProgram :: GetLinkerLog - glGetProgramiv"));

	if (l_iLength > 1)
	{
		char * l_pTmp = new char[l_iLength];
		glGetProgramInfoLog( m_programObject, l_iLength, & l_uiLength, l_pTmp);
		CheckGLError( CU_T( "GLShaderProgram :: GetLinkerLog - glGetProgramInfoLog"));
		strLog = l_pTmp;
		delete [] l_pTmp;
	}
}

void GLShaderProgram :: Begin()
{
	if ( ! RenderSystem::UseShaders() || m_programObject == 0 || ! m_enabled || ! m_isLinked || m_bError)
	{
		return;
	}

	glUseProgram( m_programObject);
	CheckGLError( CU_T( "GLShaderProgram :: End - glUseProgram"));

	ApplyAllVariables();
}

void GLShaderProgram :: ApplyAllVariables()
{
	for (GLFrameVariablePtrStrMap::iterator l_it = m_mapGLFrameVariables.begin() ; l_it != m_mapGLFrameVariables.end() ; ++l_it)
	{
		l_it->second->Apply();
	}
}

void GLShaderProgram :: End()
{
	if ( ! RenderSystem::UseShaders() || ! m_enabled)
	{
		return;
	}

	glUseProgram( 0);
	CheckGLError( CU_T( "GLShaderProgram :: End - glUseProgram"));
}

GLFrameVariablePtr GLShaderProgram :: GetGLFrameVariable( const String & p_strName)
{
	GLFrameVariablePtr l_pReturn;

	if (m_mapGLFrameVariables.find( p_strName) != m_mapGLFrameVariables.end())
	{
		l_pReturn = m_mapGLFrameVariables.find( p_strName)->second;
	}

	return l_pReturn;
}

void GLShaderProgram :: AddFrameVariable( FrameVariablePtr p_pVariable)
{
	if ( ! p_pVariable.null() && m_mapGLFrameVariables.find( p_pVariable->GetName()) == m_mapGLFrameVariables.end())
	{
		switch (p_pVariable->GetType())
		{
		case FrameVariable::eOne:
			if (p_pVariable->GetSubType() == typeid( int))
			{
				AddFrameVariable<int>( static_pointer_cast< OneFrameVariable<int> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int))
			{
				AddFrameVariable<unsigned int>( static_pointer_cast< OneFrameVariable<unsigned int> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< OneFrameVariable<float> >( p_pVariable));
			}
			break;

		case FrameVariable::eVec1:
			if (p_pVariable->GetSubType() == typeid( int))
			{
				AddFrameVariable<int>( static_pointer_cast< PointFrameVariable<int, 1> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int))
			{
				AddFrameVariable<unsigned int>( static_pointer_cast< PointFrameVariable<unsigned int, 1> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< PointFrameVariable<float, 1> >( p_pVariable));
			}
			break;

		case FrameVariable::eVec2:
			if (p_pVariable->GetSubType() == typeid( int))
			{
				AddFrameVariable<int>( static_pointer_cast< PointFrameVariable<int, 2> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int))
			{
				AddFrameVariable<unsigned int>( static_pointer_cast< PointFrameVariable<unsigned int, 2> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< PointFrameVariable<float, 2> >( p_pVariable));
			}
			break;

		case FrameVariable::eVec3:
			if (p_pVariable->GetSubType() == typeid( int))
			{
				AddFrameVariable<int>( static_pointer_cast< PointFrameVariable<int, 3> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int))
			{
				AddFrameVariable<unsigned int>( static_pointer_cast< PointFrameVariable<unsigned int, 3> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< PointFrameVariable<float, 3> >( p_pVariable));
			}
			break;

		case FrameVariable::eVec4:
			if (p_pVariable->GetSubType() == typeid( int))
			{
				AddFrameVariable<int>( static_pointer_cast< PointFrameVariable<int, 4> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int))
			{
				AddFrameVariable<unsigned int>( static_pointer_cast< PointFrameVariable<unsigned int, 4> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< PointFrameVariable<float, 4> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat2x2:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 2, 2> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat2x3:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 2, 3> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat2x4:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 2, 4> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat3x2:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 3, 2> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat3x3:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 3, 3> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat3x4:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 3, 4> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat4x2:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 4, 2> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat4x3:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 4, 3> >( p_pVariable));
			}
			break;

		case FrameVariable::eMat4x4:
			if (p_pVariable->GetSubType() == typeid( float))
			{
				AddFrameVariable<float>( static_pointer_cast< MatrixFrameVariable<float, 4, 4> >( p_pVariable));
			}
			break;
		}
	}
}
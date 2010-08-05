#include "PrecompiledHeader.h"

#include "GLSLProgram.h"
#include "GLSLObject.h"
#include "GLUniformVariable.h"
#include "GLRenderSystem.h"
#include "GLContext.h"

using namespace Castor3D;

GLShaderProgram :: GLShaderProgram()
	:	ShaderProgram(),
		m_programObject( 0),
		m_linkerLog( NULL)
{
}

GLShaderProgram :: GLShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile),
		m_programObject( 0),
		m_linkerLog( NULL)
{
}

GLShaderProgram :: ~GLShaderProgram()
{
	if (m_linkerLog != NULL)
	{
		free(m_linkerLog);
	}

	if (GLRenderSystem::UseGLSL())
	{
		Cleanup();

//		glDeleteShader( m_programObject);
		CheckGLError( "GLShaderProgram :: ~GLShaderProgram - glDeleteShader");
	}
}

void GLShaderProgram :: _deleteShaderObject( GLShaderObject *& p_ppObject)
{
	delete p_ppObject;
	p_ppObject = NULL;
}

void GLShaderProgram :: Cleanup()
{
	_deleteShaderObject( reinterpret_cast <GLShaderObject *&>( m_pVertexShader));
	_deleteShaderObject( reinterpret_cast <GLShaderObject *&>( m_pFragmentShader));
	_deleteShaderObject( reinterpret_cast <GLShaderObject *&>( m_pGeometryShader));
}

void GLShaderProgram :: Initialise()
{
	if (m_isLinked)
	{
		return;
	}

	if ( ! GLRenderSystem::UseGLSL())
	{
		Log::LogMessage( "**ERROR: OpenGL Shading Language is NOT available!");
	}
	else
	{
		m_programObject = glCreateProgram();
	}

	ShaderProgram::Initialise();
}

bool GLShaderProgram :: Link()
{
	if ( ! GLRenderSystem::UseGLSL() || m_bError)
	{
		return false;
	}

	if (m_usesGeometryShader)
	{
		glProgramParameteriEXT(m_programObject, GL_GEOMETRY_INPUT_TYPE_EXT, m_nInputPrimitiveType);
		glProgramParameteriEXT(m_programObject, GL_GEOMETRY_OUTPUT_TYPE_EXT, m_nOutputPrimitiveType);
		glProgramParameteriEXT(m_programObject, GL_GEOMETRY_VERTICES_OUT_EXT, m_nVerticesOut);
	}

	if (m_isLinked)  // already linked, detach everything first
	{
		Log::LogMessage( "**warning** Object is already linked, trying to link again");

		reinterpret_cast <GLShaderObject *>( m_pVertexShader)->Detach();
		reinterpret_cast <GLShaderObject *>( m_pFragmentShader)->Detach();

		if (m_pGeometryShader != NULL)
		{
			reinterpret_cast <GLShaderObject *>( m_pGeometryShader)->Detach();
		}
	}

	reinterpret_cast <GLShaderObject *>( m_pVertexShader)->AttachTo( this);
	reinterpret_cast <GLShaderObject *>( m_pFragmentShader)->AttachTo( this);

	if (m_pGeometryShader != NULL)
	{
		reinterpret_cast <GLShaderObject *>( m_pGeometryShader)->AttachTo( this);
	}

	GLint linked = 0;
	glLinkProgram( m_programObject);
	CheckGLError( "GLShaderProgram :: Link - glLinkProgram");
	glGetProgramiv( m_programObject, GL_LINK_STATUS, & linked);
	CheckGLError( "GLShaderProgram :: Link - glGetProgramiv");

	if (linked)
	{
		m_iTangentAttribLocation = 12;
		glBindAttribLocation( m_programObject, m_iTangentAttribLocation, "tangent");
		CheckGLError( "GLShaderProgram :: Link - glBindAttribLocation");
		m_iBitangentAttribLocation = 13;
		glBindAttribLocation( m_programObject, m_iBitangentAttribLocation, "binormal");
		CheckGLError( "GLShaderProgram :: Link - glBindAttribLocation");

		m_isLinked = true;
	}
	else
	{
		m_bError = true;
		String l_strLog;
		RetrieveLinkerLog( l_strLog);
		Log::LogMessage( "**linker error**");
	}

	return m_isLinked;
}

void GLShaderProgram :: RetrieveLinkerLog( String & strLog)
{
	if ( ! GLRenderSystem::UseGLSL())
	{
		strLog = GLRenderSystem::GetGLSLString( 0);
		return;
	}

	GLint blen = 0;
	GLsizei slen = 0;

	if (m_programObject == 0)
	{
		strLog = GLRenderSystem::GetGLSLString( 2);
		return;
	}

	glGetProgramiv(m_programObject, GL_INFO_LOG_LENGTH , &blen);
	CheckGLError( "GLShaderProgram :: GetLinkerLog - glGetProgramiv");

	if (blen > 1)
	{
		if (m_linkerLog!=0) 
		{   
			free(m_linkerLog);
			m_linkerLog =0;
		}
		if ((m_linkerLog = (GLcharARB*)malloc(blen)) == NULL) 
		{
			Log::LogMessage( "ERROR: Could not allocate compiler_log buffer");
			strLog = GLRenderSystem::GetGLSLString( 3);
			return;
		}

		glGetProgramInfoLog(m_programObject, blen, &slen, m_linkerLog);
		CheckGLError( "GLShaderProgram :: GetLinkerLog - glGetProgramInfoLog");

	}
	if (m_linkerLog!=0)
	{
		strLog = m_linkerLog;
	}
	else
	{
		strLog = GLRenderSystem::GetGLSLString( 5);
	}
}

void GLShaderProgram :: Begin()
{
	if ( ! GLRenderSystem::UseGLSL() || m_programObject == 0 || ! m_enabled || ! m_isLinked || m_bError)
	{
		return;
	}

	glUseProgram( m_programObject);
	CheckGLError( "GLShaderProgram :: End - glUseProgram");

	map::cycle( m_mapGLUniformVariables, & GLUniformVariable::Apply);
}

void GLShaderProgram :: ApplyAllVariables()
{
	map::cycle( m_mapGLUniformVariables, & GLUniformVariable::Apply);
}

void GLShaderProgram :: End()
{
	if ( ! GLRenderSystem::UseGLSL() || ! m_enabled)
	{
		return;
	}

	glUseProgram( 0);
	CheckGLError( "GLShaderProgram :: End - glUseProgram");
}

GLUniformVariable * GLShaderProgram :: _createGLUniformVariable( UniformVariable * p_pUniformVariable)
{
	GLUniformVariable * l_pReturn = NULL;

	switch (p_pUniformVariable->GetType())
	{
	case UniformVariable::eOne:
		if (p_pUniformVariable->GetSubType() == typeid( float))
		{
			l_pReturn = new GLOneFloatUniformVariable( (OneUniformVariable<float> *)p_pUniformVariable, & m_programObject);
		}
		else
		{
			l_pReturn = new GLOneIntUniformVariable( (OneUniformVariable<int> *)p_pUniformVariable, & m_programObject);
		}
		break;
		
	case UniformVariable::eVec2:
		if (p_pUniformVariable->GetSubType() == typeid( float))
		{
			l_pReturn = new GLTwoFloatsUniformVariable( (Point2DUniformVariable<float> *)p_pUniformVariable, & m_programObject);
		}
		else
		{
			l_pReturn = new GLTwoIntsUniformVariable( (Point2DUniformVariable<int> *)p_pUniformVariable, & m_programObject);
		}
		break;
		
	case UniformVariable::eVec3:
		if (p_pUniformVariable->GetSubType() == typeid( float))
		{
			l_pReturn = new GLThreeFloatsUniformVariable( (Point3DUniformVariable<float> *)p_pUniformVariable, & m_programObject);
		}
		else
		{
			l_pReturn = new GLThreeIntsUniformVariable( (Point3DUniformVariable<int> *)p_pUniformVariable, & m_programObject);
		}
		break;
		
	case UniformVariable::eVec4:
		if (p_pUniformVariable->GetSubType() == typeid( float))
		{
			l_pReturn = new GLFourFloatsUniformVariable( (Point4DUniformVariable<float> *)p_pUniformVariable, & m_programObject);
		}
		else
		{
			l_pReturn = new GLFourIntsUniformVariable( (Point4DUniformVariable<int> *)p_pUniformVariable, & m_programObject);
		}
		break;
		
	case UniformVariable::eMat2:
		l_pReturn = new GLMatrix2fUniformVariable( (Matrix2UniformVariable<float> *)p_pUniformVariable, & m_programObject);
		break;
		
	case UniformVariable::eMat3:
		l_pReturn = new GLMatrix3fUniformVariable( (Matrix3UniformVariable<float> *)p_pUniformVariable, & m_programObject);
		break;
		
	case UniformVariable::eMat4:
		l_pReturn = new GLMatrix4fUniformVariable( (Matrix4UniformVariable<float> *)p_pUniformVariable, & m_programObject);
		break;
	}

	return l_pReturn;
}

void GLShaderProgram :: AddUniformVariable( UniformVariable * p_pUniformVariable)
{
	ShaderProgram::AddUniformVariable( p_pUniformVariable);

	if (p_pUniformVariable != NULL)
	{
		GLUniformVariable * l_pGLUniformVariable = _createGLUniformVariable( p_pUniformVariable);

		if (m_mapGLUniformVariables.find( p_pUniformVariable->GetName()) == m_mapGLUniformVariables.end())
		{
			m_mapGLUniformVariables.insert( std::map <String, GLUniformVariable *>::value_type( p_pUniformVariable->GetName(), l_pGLUniformVariable));
		}
	}
}

bool GLShaderProgram :: SetUniform1ui( const GLcharARB* varname, GLuint v0, GLint index)
{ 
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return false;  // can't find variable / invalid index

	glUniform1uiEXT(loc, v0);

	return true;
}

bool GLShaderProgram :: SetUniform2ui( const GLcharARB* varname, GLuint v0, GLuint v1, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return false;  // can't find variable / invalid index

	glUniform2uiEXT(loc, v0, v1);


	return true;
}

bool GLShaderProgram :: SetUniform3ui( const GLcharARB* varname, GLuint v0, GLuint v1, GLuint v2, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return false;  // can't find variable / invalid index

	glUniform3uiEXT(loc, v0, v1, v2);

	return true;
}

bool GLShaderProgram :: SetUniform4ui( const GLcharARB* varname, GLuint v0, GLuint v1, GLuint v2, GLuint v3, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return false;  // can't find variable / invalid index

	glUniform4uiEXT(loc, v0, v1, v2, v3);

	return true;
}

bool GLShaderProgram :: SetUniform1uiv( const GLcharARB* varname, GLsizei count, GLuint *value, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return false;  // can't find variable / invalid index

	glUniform1uivEXT(loc, count, value);

	return true;
}

bool GLShaderProgram :: SetUniform2uiv( const GLcharARB* varname, GLsizei count, GLuint *value, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return false;  // can't find variable / invalid index

	glUniform2uivEXT(loc, count, value);

	return true;
}

bool GLShaderProgram :: SetUniform3uiv( const GLcharARB* varname, GLsizei count, GLuint *value, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return false;  // can't find variable / invalid index

	glUniform3uivEXT(loc, count, value);

	return true;
}

bool GLShaderProgram :: SetUniform4uiv( const GLcharARB* varname, GLsizei count, GLuint *value, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return false;  // can't find variable / invalid index

	glUniform4uivEXT(loc, count, value);

	return true;
}

GLint GLShaderProgram :: GetUniformLocation(const GLcharARB *name)
{
	GLint loc;

	loc = glGetUniformLocation(m_programObject, name);
	if (loc == -1) 
	{
		Log::LogMessage( "Error: can't find uniform variable \"%s\"", name);
	}
	CheckGLError( "GLShaderProgram :: GetUniformLocation - glGetUniformLocation");
	return loc;
}

void GLShaderProgram :: GetUniformfv(const GLcharARB* varname, GLfloat* values, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return;  // can't find variable / invalid index

	glGetUniformfv(m_programObject, loc, values);

}

void GLShaderProgram :: GetUniformiv(const GLcharARB* varname, GLint* values, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return;  // can't find variable / invalid index

	glGetUniformiv(m_programObject, loc, values);

}

void GLShaderProgram :: GetUniformuiv(const GLcharARB* varname, GLuint* values, GLint index)
{
	if (!GLRenderSystem::UseGLSL()) return;

	GLint loc;
	if (varname)
		loc = GetUniformLocation(varname);
	else
		loc = index;

	if (loc==-1) 
		return;  // can't find variable / invalid index

	glGetUniformuivEXT(m_programObject, loc, values);

}

void  GLShaderProgram :: BindAttribLocation(GLint index, GLchar* name)
{
	glBindAttribLocation(m_programObject, index, name);
}

bool GLShaderProgram :: SetVertexAttrib1f(GLuint index, GLfloat v0)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib1f(index, v0);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib2f(index, v0, v1);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib3f(index, v0, v1, v2);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib4f(index, v0, v1, v2, v3);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib1d(GLuint index, GLdouble v0)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib1d(index, v0);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib2d(GLuint index, GLdouble v0, GLdouble v1)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib2d(index, v0, v1);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib3d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib3d(index, v0, v1, v2);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib4d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib4d(index, v0, v1, v2, v3);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib1s(GLuint index, GLshort v0)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib1s(index, v0);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib2s(GLuint index, GLshort v0, GLshort v1)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib2s(index, v0, v1);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib3s(GLuint index, GLshort v0, GLshort v1, GLshort v2)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib3s(index, v0, v1, v2);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib4s(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib4s(index, v0, v1, v2, v3);

	return true;
}

bool GLShaderProgram :: SetVertexAttribNormalizedByte(GLuint index, GLbyte v0, GLbyte v1, GLbyte v2, GLbyte v3)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!m_enabled) return true;

	glVertexAttrib4Nub(index, v0, v1, v2, v3);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib1i(GLuint index, GLint v0)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	glVertexAttribI1iEXT(index, v0);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib2i(GLuint index, GLint v0, GLint v1)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	glVertexAttribI2iEXT(index, v0, v1);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib3i(GLuint index, GLint v0, GLint v1, GLint v2)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	glVertexAttribI3iEXT(index, v0, v1, v2);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib4i(GLuint index, GLint v0, GLint v1, GLint v2, GLint v3)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	glVertexAttribI4iEXT(index, v0, v1, v2, v3);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib1ui(GLuint index, GLuint v0)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	glVertexAttribI1uiEXT(index, v0);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib2ui(GLuint index, GLuint v0, GLuint v1)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	glVertexAttribI2uiEXT(index, v0, v1);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib3ui(GLuint index, GLuint v0, GLuint v1, GLuint v2)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	glVertexAttribI3uiEXT(index, v0, v1, v2);

	return true;
}

bool GLShaderProgram :: SetVertexAttrib4ui(GLuint index, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
	if (!GLRenderSystem::UseGLSL()) return false; // GLSL not available
	if (!GLRenderSystem::HasGPUShader4()) return false;
	if (!m_enabled) return true;

	glVertexAttribI4uiEXT(index, v0, v1, v2, v3);

	return true;
}

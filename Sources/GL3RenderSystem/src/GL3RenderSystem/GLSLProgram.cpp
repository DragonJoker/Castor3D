#include "PrecompiledHeader.h"

#include "GLSLProgram.h"
#include "GLSLObject.h"
#include "GLUniformVariable.h"
#include "GLRenderSystem.h"
#include "GLContext.h"
#include "GLBuffer.h"
#include "GLLightRenderer.h"

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
	if ( ! GLRenderSystem::UseShaders() || m_bError)
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

	glBindFragDataLocation( m_programObject, 1, "FragColor");
	CheckGLError( "GLShaderProgram :: Link - glBindFragDataLocation");

	GLint linked = 0;
	glLinkProgram( m_programObject);
	CheckGLError( "GLShaderProgram :: Link - glLinkProgram");
	glGetProgramiv( m_programObject, GL_LINK_STATUS, & linked);
	CheckGLError( "GLShaderProgram :: Link - glGetProgramiv");

	if (linked)
	{
		m_isLinked = true;

		m_materialShader.pUniformBuffer = new GLUniformBufferObject( "in_Material", this);
		m_materialShader.pUniformBuffer->Initialise();
//		m_materialShader.pUniformBuffer->InitialiseBuffer( 17, 0);
		m_materialShader.pUniformBuffer->AddVariable( "in_MatAmbient",		(unsigned char *)m_materialShader.vAmbient.ptr(), 16);
		m_materialShader.pUniformBuffer->AddVariable( "in_MatDiffuse",		(unsigned char *)m_materialShader.vDiffuse.ptr(), 16);
		m_materialShader.pUniformBuffer->AddVariable( "in_MatEmissive",		(unsigned char *)m_materialShader.vEmissive.ptr(), 16);
		m_materialShader.pUniformBuffer->AddVariable( "in_MatSpecular",		(unsigned char *)m_materialShader.vSpecular.ptr(), 16);
		m_materialShader.pUniformBuffer->AddVariable( "in_MatShininess",	(unsigned char *)& m_materialShader.rShininess, 4);
		m_mapUniformBuffers.insert( GLUniformBufferObjectPtrStrMap::value_type( "in_Material", m_materialShader.pUniformBuffer));

		String l_strBase = "in_Lights";
		m_lightsShader.pUniformBuffer = new GLUniformBufferObject( l_strBase, this);
		m_lightsShader.pUniformBuffer->Initialise();
//		m_lightsShader.pUniformBuffer->InitialiseBuffer( 37 * 8, 0);
		m_lightsShader.pAmbients = m_lightsShader.pUniformBuffer->AddVariable( l_strBase + "Ambient",			(unsigned char *)m_lightsShader.vAmbients,		16 * 8);
		m_lightsShader.pDiffuses = m_lightsShader.pUniformBuffer->AddVariable( l_strBase + "Diffuse",			(unsigned char *)m_lightsShader.vDiffuses,		16 * 8);
		m_lightsShader.pSpeculars = m_lightsShader.pUniformBuffer->AddVariable( l_strBase + "Specular",			(unsigned char *)m_lightsShader.vSpeculars,		16 * 8);
		m_lightsShader.pPositions = m_lightsShader.pUniformBuffer->AddVariable( l_strBase + "Position",			(unsigned char *)m_lightsShader.vPositions,		16 * 8);
		m_lightsShader.pAttenuations = m_lightsShader.pUniformBuffer->AddVariable( l_strBase + "Attenuation",	(unsigned char *)m_lightsShader.vAttenuations,	12 * 8);
		m_lightsShader.pOrientations = m_lightsShader.pUniformBuffer->AddVariable( l_strBase + "Orientation",	(unsigned char *)m_lightsShader.mOrientations,	64 * 8);
		m_lightsShader.pExponents = m_lightsShader.pUniformBuffer->AddVariable( l_strBase + "Exponent",			(unsigned char *)& m_lightsShader.rExponents,	4 * 8);
		m_lightsShader.pCutOffs = m_lightsShader.pUniformBuffer->AddVariable( l_strBase + "CutOff",				(unsigned char *)& m_lightsShader.rCutOffs,		4 * 8);
		m_mapUniformBuffers.insert( GLUniformBufferObjectPtrStrMap::value_type( l_strBase, m_lightsShader.pUniformBuffer));
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

	GLRenderSystem::GetSingletonPtr()->SetCurrentShaderProgram( this);

	glUseProgram( m_programObject);
	CheckGLError( "GLShaderProgram :: Begin - glUseProgram ");

	ApplyAllVariables();
}

void GLShaderProgram :: ApplyAllVariables()
{
	for (GLUniformVariablePtrStrMap::iterator l_it = m_mapGLUniformVariables.begin() ; l_it != m_mapGLUniformVariables.end() ; ++l_it)
	{
		l_it->second->Apply();
	}
	for (GLUniformBufferObjectPtrStrMap::iterator l_it = m_mapUniformBuffers.begin() ; l_it != m_mapUniformBuffers.end() ; ++l_it)
	{
		l_it->second->Activate();
	}
}

void GLShaderProgram :: End()
{
	if ( ! GLRenderSystem::UseShaders() || ! m_enabled)
	{
		return;
	}
	for (GLUniformBufferObjectPtrStrMap::iterator l_it = m_mapUniformBuffers.begin() ; l_it != m_mapUniformBuffers.end() ; ++l_it)
	{
		l_it->second->Deactivate();
	}

	glUseProgram( 0);
	CheckGLError( "GLShaderProgram :: End - glUseProgram");
}

GLUniformBufferObjectPtr GLShaderProgram :: GetUniformBuffer( const String & p_strName)
{
	GLUniformBufferObjectPtr l_pReturn;

	if (m_mapUniformBuffers.find( p_strName) != m_mapUniformBuffers.end())
	{
		l_pReturn = m_mapUniformBuffers.find( p_strName)->second;
	}

	return l_pReturn;
}

GLUniformVariablePtr GLShaderProgram :: _createGLUniformVariable( UniformVariablePtr p_pUniformVariable)
{
	GLUniformVariable * l_pReturn = NULL;

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

void GLShaderProgram :: SetLightValues( int p_iIndex, GLLightRenderer * p_pLightRenderer)
{
	if (p_iIndex >= 0 && p_iIndex < 8)
	{
		m_lightsShader.vAmbients[p_iIndex] = p_pLightRenderer->GetAmbient();
		m_lightsShader.vDiffuses[p_iIndex] = p_pLightRenderer->GetDiffuse();
		m_lightsShader.vSpeculars[p_iIndex] = p_pLightRenderer->GetSpecular();
		m_lightsShader.vPositions[p_iIndex] = p_pLightRenderer->GetPosition();
		m_lightsShader.vAttenuations[p_iIndex] = p_pLightRenderer->GetAttenuation();
		m_lightsShader.mOrientations[p_iIndex] = p_pLightRenderer->GetOrientation();
		m_lightsShader.rExponents[p_iIndex] = p_pLightRenderer->GetExponent();
		m_lightsShader.rCutOffs[p_iIndex] = p_pLightRenderer->GetCutOff();

		m_lightsShader.pAmbients->SetIndexValue( m_lightsShader.vAmbients[p_iIndex].const_ptr(), p_iIndex);
		m_lightsShader.pDiffuses->SetIndexValue( m_lightsShader.vDiffuses[p_iIndex].const_ptr(), p_iIndex);
		m_lightsShader.pSpeculars->SetIndexValue( m_lightsShader.vSpeculars[p_iIndex].const_ptr(), p_iIndex);
		m_lightsShader.pPositions->SetIndexValue( m_lightsShader.vPositions[p_iIndex].const_ptr(), p_iIndex);
		m_lightsShader.pAttenuations->SetIndexValue( m_lightsShader.vAttenuations[p_iIndex].const_ptr(), p_iIndex);
		m_lightsShader.pOrientations->SetIndexValue( m_lightsShader.mOrientations[p_iIndex].m_matrix, p_iIndex);
		m_lightsShader.pExponents->SetIndexValue( & m_lightsShader.rExponents[p_iIndex], p_iIndex);
		m_lightsShader.pCutOffs->SetIndexValue( & m_lightsShader.rCutOffs[p_iIndex], p_iIndex);
	}
}

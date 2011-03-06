#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlShaderProgram.h"
#include "OpenGlCommon/GlShaderObject.h"
#include "OpenGlCommon/GlFrameVariable.h"

using namespace Castor3D;

GlShaderProgram :: GlShaderProgram()
	:	GlslShaderProgram(),
		m_programObject( 0)
{
}

GlShaderProgram :: GlShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	GlslShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile),
		m_programObject( 0)
{
}

GlShaderProgram :: ~GlShaderProgram()
{
	m_mapGlFrameVariables.clear();
	Cleanup();

	if (RenderSystem::UseShaders())
	{

		if (m_programObject != 0)
		{
			CheckGlError( glDeleteProgram( m_programObject), CU_T( "GlShaderProgram :: ~GlShaderProgram - glDeleteShader"));
		}
	}
}

void GlShaderProgram :: Cleanup()
{
	GlslShaderProgram::Cleanup();
}

void GlShaderProgram :: Initialise()
{
	if ( ! m_isLinked)
	{
		if (RenderSystem::UseShaders())
		{
			m_programObject = glCreateProgram();
		}

		GlslShaderProgram::Initialise();
	}
}

bool GlShaderProgram :: Link()
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError)
	{
		if (m_isLinked)  // already linked, detach everything first
		{
			Logger::LogWarning( CU_T( "GlShaderProgram :: Link - Object is already linked, trying to link again"));

			static_pointer_cast<GlShaderObject>( m_pShaders[eVertexShader])->Detach();
			static_pointer_cast<GlShaderObject>( m_pShaders[ePixelShader])->Detach();

			if (m_pShaders[eGeometryShader] != NULL)
			{
				static_pointer_cast<GlShaderObject>( m_pShaders[eGeometryShader])->Detach();
			}
		}

		static_pointer_cast<GlShaderObject>( m_pShaders[eVertexShader])->AttachTo( this);
		static_pointer_cast<GlShaderObject>( m_pShaders[ePixelShader])->AttachTo( this);

		if (m_pShaders[eGeometryShader] != NULL)
		{
			static_pointer_cast<GlShaderObject>( m_pShaders[eGeometryShader])->AttachTo( this);
		}

		GLint linked = 0;
		CheckGlError( glLinkProgram( m_programObject), CU_T( "GlShaderProgram :: Link - glLinkProgram"));
		CheckGlError( glGetProgramiv( m_programObject, GL_LINK_STATUS, & linked), CU_T( "GlShaderProgram :: Link - glGetProgramiv( GL_LINK_STATUS)"));
		CheckGlError( glGetProgramiv( m_programObject, GL_VALIDATE_STATUS, & linked), CU_T( "GlShaderProgram :: Link - glGetProgramiv( GL_VALIDATE_STATUS)"));

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

void GlShaderProgram :: RetrieveLinkerLog( String & strLog)
{
	if ( ! RenderSystem::UseShaders())
	{
		strLog = GlEnum::GetGlslErrorString( 0);
		return;
	}

	GLint l_iLength = 0;
	GLsizei l_uiLength = 0;

	if (m_programObject == 0)
	{
		strLog = GlEnum::GetGlslErrorString( 2);
		return;
	}

	CheckGlError( glGetProgramiv(m_programObject, GL_INFO_LOG_LENGTH , & l_iLength), CU_T( "GlShaderProgram :: GetLinkerLog - glGetProgramiv"));

	if (l_iLength > 1)
	{
		char * l_pTmp = new char[l_iLength];
		CheckGlError( glGetProgramInfoLog( m_programObject, l_iLength, & l_uiLength, l_pTmp), CU_T( "GlShaderProgram :: GetLinkerLog - glGetProgramInfoLog"));
		strLog = l_pTmp;
		delete [] l_pTmp;
	}
}

void GlShaderProgram :: Begin()
{
	if ( ! RenderSystem::UseShaders() || m_programObject == 0 || ! m_enabled || ! m_isLinked || m_bError)
	{
		return;
	}

	CheckGlError( glUseProgram( m_programObject), CU_T( "GlShaderProgram :: End - glUseProgram"));
	ApplyAllVariables();
}

void GlShaderProgram :: ApplyAllVariables()
{
	for (GlFrameVariablePtrStrMap::iterator l_it = m_mapGlFrameVariables.begin() ; l_it != m_mapGlFrameVariables.end() ; ++l_it)
	{
		l_it->second->Apply();
	}
}

void GlShaderProgram :: End()
{
	if ( ! RenderSystem::UseShaders() || ! m_enabled)
	{
		return;
	}

	CheckGlError( glUseProgram( 0), CU_T( "GlShaderProgram :: End - glUseProgram"));
}

GlFrameVariablePtr GlShaderProgram :: GetGlFrameVariable( const String & p_strName)
{
	GlFrameVariablePtr l_pReturn;

	if (m_mapGlFrameVariables.find( p_strName) != m_mapGlFrameVariables.end())
	{
		l_pReturn = m_mapGlFrameVariables.find( p_strName)->second;
	}

	return l_pReturn;
}

void GlShaderProgram :: AddFrameVariable( FrameVariablePtr p_pVariable, GlslShaderObjectPtr p_pPobject)
{
	if (p_pVariable != NULL && m_mapGlFrameVariables.find( p_pVariable->GetName()) == m_mapGlFrameVariables.end())
	{
		switch (p_pVariable->GetType())
		{
		case FrameVariable::eOne:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				AddFrameVariable<int>( static_pointer_cast< OneFrameVariable<int> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int).name())
			{
				AddFrameVariable<unsigned int>( static_pointer_cast< OneFrameVariable<unsigned int> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float>( static_pointer_cast< OneFrameVariable<float> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eVec1:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				AddFrameVariable<int, 1>( static_pointer_cast< PointFrameVariable<int, 1> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int).name())
			{
				AddFrameVariable<unsigned int, 1>( static_pointer_cast< PointFrameVariable<unsigned int, 1> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 1>( static_pointer_cast< PointFrameVariable<float, 1> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eVec2:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				AddFrameVariable<int, 2>( static_pointer_cast< PointFrameVariable<int, 2> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int).name())
			{
				AddFrameVariable<unsigned int, 2>( static_pointer_cast< PointFrameVariable<unsigned int, 2> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 2>( static_pointer_cast< PointFrameVariable<float, 2> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eVec3:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				AddFrameVariable<int, 3>( static_pointer_cast< PointFrameVariable<int, 3> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int).name())
			{
				AddFrameVariable<unsigned int, 3>( static_pointer_cast< PointFrameVariable<unsigned int, 3> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 3>( static_pointer_cast< PointFrameVariable<float, 3> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eVec4:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				AddFrameVariable<int, 4>( static_pointer_cast< PointFrameVariable<int, 4> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( unsigned int).name())
			{
				AddFrameVariable<unsigned int, 4>( static_pointer_cast< PointFrameVariable<unsigned int, 4> >( p_pVariable), p_pPobject);
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 4>( static_pointer_cast< PointFrameVariable<float, 4> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat2x2:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 2, 2>( static_pointer_cast< MatrixFrameVariable<float, 2, 2> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat2x3:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 2, 3>( static_pointer_cast< MatrixFrameVariable<float, 2, 3> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat2x4:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 2, 4>( static_pointer_cast< MatrixFrameVariable<float, 2, 4> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat3x2:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 3, 2>( static_pointer_cast< MatrixFrameVariable<float, 3, 2> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat3x3:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 3, 3>( static_pointer_cast< MatrixFrameVariable<float, 3, 3> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat3x4:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 3, 4>( static_pointer_cast< MatrixFrameVariable<float, 3, 4> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat4x2:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 4, 2>( static_pointer_cast< MatrixFrameVariable<float, 4, 2> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat4x3:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 4, 3>( static_pointer_cast< MatrixFrameVariable<float, 4, 3> >( p_pVariable), p_pPobject);
			}
			break;

		case FrameVariable::eMat4x4:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				AddFrameVariable<float, 4, 4>( static_pointer_cast< MatrixFrameVariable<float, 4, 4> >( p_pVariable), p_pPobject);
			}
			break;
		}
	}
}

#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/CgGLShaderProgram.h"
#include "OpenGLCommon/CgGLShaderObject.h"
#include "OpenGLCommon/CgGLFrameVariable.h"
#include "OpenGLCommon/GLRenderSystem.h"

using namespace Castor3D;

CgGLShaderProgram :: CgGLShaderProgram()
	:	CgShaderProgram()
{
}

CgGLShaderProgram :: CgGLShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	CgShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile)
{
}

CgGLShaderProgram :: ~CgGLShaderProgram()
{
	m_mapCgGLFrameVariables.clear();
	Cleanup();

	if (RenderSystem::UseShaders())
	{
		if (m_cgContext != NULL)
		{
			cgDestroyContext( m_cgContext);
			CheckCgError( CU_T( "GLShaderProgram :: ~GLShaderProgram - cgDestroyContext"));
		}
	}
}

void CgGLShaderProgram :: Cleanup()
{
	CgShaderProgram::Cleanup();
}

void CgGLShaderProgram :: Initialise()
{
	if ( ! m_isLinked)
	{
		if (RenderSystem::UseShaders())
		{
			if ( ! m_isLinked && ! m_bError)
			{
				Cleanup();

				for (int i = eVertexShader ; i < eNbShaderTypes ; i++)
				{
					if ( ! m_pShaders[i] == NULL && ! m_pShaders[i]->GetSource().empty())
					{
						m_pShaders[i]->CreateProgram();
					}
				}

				m_cgContext = cgCreateContext();
				CASTOR_ASSERT( m_cgContext != NULL && cgIsContext( m_cgContext));

				m_usesGeometryShader = ! m_pShaders[eGeometryShader] == NULL && ! m_pShaders[eGeometryShader]->GetSource().empty();

				for (int i = eVertexShader ; i < eNbShaderTypes ; i++)
				{
					if ( ! m_pShaders[i] == NULL && ! m_pShaders[i]->GetSource().empty())
					{
						if ( ! m_pShaders[i]->Compile())
						{
							Logger::LogError( CU_T( "ShaderProgram :: LoadfromMemory - ***COMPILER ERROR (Shader %i)"), i);
							Cleanup();
							return;
						}
					}
				}

				if ( ! Link())
				{
					Logger::LogError( CU_T( "ShaderProgram :: LoadfromMemory - **LINKER ERROR"));
					Cleanup();
					return;
				}

				Logger::LogMessage( CU_T( "ShaderProgram :: LoadfromMemory - ***Program Linked successfully"));
			}
		}
	}
}

bool CgGLShaderProgram :: Link()
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError && m_cgContext != NULL)
	{
		if ( ! m_isLinked)
		{
			SmartPtr< CgOneFrameVariable<float> >::Shared l_fUniform;
			SmartPtr< CgPointFrameVariable<float, 4> >::Shared l_pt4Uniform;
			SmartPtr< CgPointFrameVariable<float, 3> >::Shared l_pt3Uniform;
			SmartPtr< CgMatrixFrameVariable<float, 4, 4> >::Shared l_mtxUniform;

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 1));
			l_pt4Uniform->SetName( CU_T( "in_AmbientLight"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pAmbientLight = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_AmbientLight")));

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 8));
			l_pt4Uniform->SetName( CU_T( "in_LightsAmbient"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pAmbients = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_LightsAmbient")));

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 8));
			l_pt4Uniform->SetName( CU_T( "in_LightsDiffuse"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pDiffuses = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_LightsDiffuse")));

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 8));
			l_pt4Uniform->SetName( CU_T( "in_LightsSpecular"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pSpeculars = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_LightsSpecular")));

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 8));
			l_pt4Uniform->SetName( CU_T( "in_LightsPosition"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pPositions = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_LightsPosition")));

			l_mtxUniform = SmartPtr< CgMatrixFrameVariable<float, 4, 4> >::Shared( new CgMatrixFrameVariable<float, 4, 4>( NULL, 8));
			l_mtxUniform->SetName( CU_T( "in_LightsOrientation"));
			AddFrameVariable( l_mtxUniform, GetProgram( eVertexShader));
			m_pOrientations = static_pointer_cast< CgGLMatrixFrameVariable<float, 4, 4> >( GetCgGLFrameVariable( CU_T( "in_LightsOrientation")));

			l_pt3Uniform = SmartPtr< CgPointFrameVariable<float, 3> >::Shared( new CgPointFrameVariable<float, 3>( NULL, 8));
			l_pt3Uniform->SetName( CU_T( "in_LightsAttenuation"));
			AddFrameVariable( l_pt3Uniform, GetProgram( eVertexShader));
			m_pAttenuations = static_pointer_cast< CgGLPointFrameVariable<float, 3> >( GetCgGLFrameVariable( CU_T( "in_LightsAttenuation")));

			l_fUniform = SmartPtr< CgOneFrameVariable<float> >::Shared( new CgOneFrameVariable<float>( NULL, 8));
			l_fUniform->SetName( CU_T( "in_LightsExponent"));
			AddFrameVariable( l_fUniform, GetProgram( eVertexShader));
			m_pExponents = static_pointer_cast< CgGLOneFrameVariable<float> >( GetCgGLFrameVariable( CU_T( "in_LightsExponent")));

			l_fUniform = SmartPtr< CgOneFrameVariable<float> >::Shared( new CgOneFrameVariable<float>( NULL, 8));
			l_fUniform->SetName( CU_T( "in_LightsCutOff"));
			AddFrameVariable( l_fUniform, GetProgram( eVertexShader));
			m_pCutOffs = static_pointer_cast< CgGLOneFrameVariable<float> >( GetCgGLFrameVariable( CU_T( "in_LightsCutOff")));

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 1));
			l_pt4Uniform->SetName( CU_T( "in_MatAmbient"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pAmbient = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_MatAmbient")));

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 1));
			l_pt4Uniform->SetName( CU_T( "in_MatDiffuse"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pDiffuse = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_MatDiffuse")));

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 1));
			l_pt4Uniform->SetName( CU_T( "in_MatEmissive"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pEmissive = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_MatEmissive")));

			l_pt4Uniform = SmartPtr< CgPointFrameVariable<float, 4> >::Shared( new CgPointFrameVariable<float, 4>( NULL, 1));
			l_pt4Uniform->SetName( CU_T( "in_MatSpecular"));
			AddFrameVariable( l_pt4Uniform, GetProgram( eVertexShader));
			m_pSpecular = static_pointer_cast< CgGLPointFrameVariable<float, 4> >( GetCgGLFrameVariable( CU_T( "in_MatSpecular")));

			l_fUniform = SmartPtr< CgOneFrameVariable<float> >::Shared( new CgOneFrameVariable<float>( NULL, 1));
			l_fUniform->SetName( CU_T( "in_MatShininess"));
			AddFrameVariable( l_fUniform, GetProgram( eVertexShader));
			m_pShininess = static_pointer_cast< CgGLOneFrameVariable<float> >( GetCgGLFrameVariable( CU_T( "in_MatShininess")));
		}

		l_bReturn = m_isLinked;
	}

	return l_bReturn;
}

void CgGLShaderProgram :: Begin()
{
	if ( ! RenderSystem::UseShaders() || ! m_enabled || ! m_isLinked || m_bError)
	{
		return;
	}

	GLRenderSystem::GetSingletonPtr()->SetCurrentShaderProgram( this);

	static_pointer_cast<CgGLShaderObject>( m_pShaders[eVertexShader])->Bind();
	static_pointer_cast<CgGLShaderObject>( m_pShaders[ePixelShader])->Bind();

	if ( ! m_pShaders[eGeometryShader] == NULL)
	{
		static_pointer_cast<CgGLShaderObject>( m_pShaders[eGeometryShader])->Bind();
	}

	ApplyAllVariables();
}

void CgGLShaderProgram :: ApplyAllVariables()
{
	for (CgGLFrameVariablePtrStrMap::iterator l_it = m_mapCgGLFrameVariables.begin() ; l_it != m_mapCgGLFrameVariables.end() ; ++l_it)
	{
		l_it->second->Apply();
	}
}

void CgGLShaderProgram :: End()
{
	if ( ! RenderSystem::UseShaders() || ! m_enabled)
	{
		return;
	}

	static_pointer_cast<CgGLShaderObject>( m_pShaders[eVertexShader])->Unbind();
	static_pointer_cast<CgGLShaderObject>( m_pShaders[ePixelShader])->Unbind();

	if ( ! m_pShaders[eGeometryShader] == NULL)
	{
		static_pointer_cast<CgGLShaderObject>( m_pShaders[eGeometryShader])->Unbind();
	}
}

CgGLFrameVariablePtr CgGLShaderProgram :: GetCgGLFrameVariable( const String & p_strName)
{
	CgGLFrameVariablePtr l_pReturn;

	if (m_mapCgGLFrameVariables.find( p_strName) != m_mapCgGLFrameVariables.end())
	{
		l_pReturn = m_mapCgGLFrameVariables.find( p_strName)->second;
	}

	return l_pReturn;
}

void CgGLShaderProgram :: AddFrameVariable( CgFrameVariablePtr p_pVariable, CgShaderObjectPtr p_pObject)
{
	if ( ! p_pVariable == NULL && m_mapCgGLFrameVariables.find( p_pVariable->GetName()) == m_mapCgGLFrameVariables.end())
	{
		switch (p_pVariable->GetType())
		{
		case FrameVariable::eOne:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double>( static_pointer_cast< CgOneFrameVariable<double> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float>( static_pointer_cast< CgOneFrameVariable<float> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eVec1:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 1>( static_pointer_cast< CgPointFrameVariable<double, 1> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 1>( static_pointer_cast< CgPointFrameVariable<float, 1> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eVec2:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 2>( static_pointer_cast< CgPointFrameVariable<double, 2> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 2>( static_pointer_cast< CgPointFrameVariable<float, 2> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eVec3:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 3>( static_pointer_cast< CgPointFrameVariable<double, 3> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 3>( static_pointer_cast< CgPointFrameVariable<float, 3> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eVec4:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 4>( static_pointer_cast< CgPointFrameVariable<double, 4> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 4>( static_pointer_cast< CgPointFrameVariable<float, 4> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat2x2:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 2, 2>( static_pointer_cast< CgMatrixFrameVariable<double, 2, 2> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 2, 2>( static_pointer_cast< CgMatrixFrameVariable<float, 2, 2> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat2x3:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 2, 3>( static_pointer_cast< CgMatrixFrameVariable<double, 2, 3> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 2, 3>( static_pointer_cast< CgMatrixFrameVariable<float, 2, 3> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat2x4:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 2, 4>( static_pointer_cast< CgMatrixFrameVariable<double, 2, 4> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 2, 4>( static_pointer_cast< CgMatrixFrameVariable<float, 2, 4> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat3x2:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 3, 2>( static_pointer_cast< CgMatrixFrameVariable<double, 3, 2> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 3, 2>( static_pointer_cast< CgMatrixFrameVariable<float, 3, 2> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat3x3:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 3, 3>( static_pointer_cast< CgMatrixFrameVariable<double, 3, 3> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 3, 3>( static_pointer_cast< CgMatrixFrameVariable<float, 3, 3> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat3x4:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 3, 4>( static_pointer_cast< CgMatrixFrameVariable<double, 3, 4> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 3, 4>( static_pointer_cast< CgMatrixFrameVariable<float, 3, 4> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat4x2:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 4, 2>( static_pointer_cast< CgMatrixFrameVariable<double, 4, 2> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 4, 2>( static_pointer_cast< CgMatrixFrameVariable<float, 4, 2> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat4x3:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 4, 3>( static_pointer_cast< CgMatrixFrameVariable<double, 4, 3> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 4, 3>( static_pointer_cast< CgMatrixFrameVariable<float, 4, 3> >( p_pVariable), p_pObject);
			}
			break;

		case FrameVariable::eMat4x4:
			if (p_pVariable->GetSubType() == typeid( double))
			{
				AddFrameVariable<double, 4, 4>( static_pointer_cast< CgMatrixFrameVariable<double, 4, 4> >( p_pVariable), p_pObject);
			}
			else
			{
				AddFrameVariable<float, 4, 4>( static_pointer_cast< CgMatrixFrameVariable<float, 4, 4> >( p_pVariable), p_pObject);
			}
			break;
		}
	}
}

int CgGLShaderProgram :: AssignLight()
{
	int l_iReturn = -1;

	if (m_setFreeLights.size() > 0)
	{
		l_iReturn = (* m_setFreeLights.begin());
		m_setFreeLights.erase( m_setFreeLights.begin());

	}

	return l_iReturn;
}

void CgGLShaderProgram :: FreeLight( int p_iIndex)
{
	if (p_iIndex != -1)
	{
		if (m_setFreeLights.find( p_iIndex) == m_setFreeLights.end())
		{
			m_setFreeLights.insert( std::set <int>::value_type( p_iIndex));
		}
	}
	else
	{
		Logger::LogError( "coin");
	}
}

void CgGLShaderProgram :: SetAmbientLight( const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pAmbientLight->SetValue( p_crColour, 0);
	}
}

void CgGLShaderProgram :: SetLightAmbient( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pAmbients->SetValue( p_crColour, p_iIndex);
	}
}

void CgGLShaderProgram :: SetLightDiffuse( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pDiffuses->SetValue( p_crColour, p_iIndex);
	}
}

void CgGLShaderProgram :: SetLightSpecular( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pSpeculars->SetValue( p_crColour, p_iIndex);
	}
}

void CgGLShaderProgram :: SetLightPosition( int p_iIndex, const Point4f & p_ptPosition)
{
	if (m_isLinked)
	{
		m_pPositions->SetValue( p_ptPosition, p_iIndex);
	}
}

void CgGLShaderProgram :: SetLightOrientation( int p_iIndex, const Matrix4x4r & p_mtxOrientation)
{
	if (m_isLinked)
	{
		m_pOrientations->SetValue( Matrix<float, 4, 4>( p_mtxOrientation), p_iIndex);
	}
}

void CgGLShaderProgram :: SetLightAttenuation( int p_iIndex, const Point3f & p_ptAtt)
{
	if (m_isLinked)
	{
		m_pAttenuations->SetValue( p_ptAtt, p_iIndex);
	}
}

void CgGLShaderProgram :: SetLightExponent( int p_iIndex, float p_fExp)
{
	if (m_isLinked)
	{
		m_pExponents->SetValue( p_fExp, p_iIndex);
	}
}

void CgGLShaderProgram :: SetLightCutOff( int p_iIndex, float p_fCut)
{
	if (m_isLinked)
	{
		m_pCutOffs->SetValue( p_fCut, p_iIndex);
	}
}

void CgGLShaderProgram :: SetMaterialAmbient( const Point4f & p_crColour)
{
	if (m_isLinked && m_pAmbient->GetValue() != p_crColour)
	{
		m_pAmbient->SetValue( p_crColour);
	}
}

void CgGLShaderProgram :: SetMaterialDiffuse( const Point4f & p_crColour)
{
	if (m_isLinked && m_pDiffuse->GetValue() != p_crColour)
	{
		m_pDiffuse->SetValue( p_crColour);
	}
}

void CgGLShaderProgram :: SetMaterialSpecular( const Point4f & p_crColour)
{
	if (m_isLinked && m_pSpecular->GetValue() != p_crColour)
	{
		m_pSpecular->SetValue( p_crColour);
	}
}

void CgGLShaderProgram :: SetMaterialEmissive( const Point4f & p_crColour)
{
	if (m_isLinked && m_pEmissive->GetValue() != p_crColour)
	{
		m_pEmissive->SetValue( p_crColour);
	}
}

void CgGLShaderProgram :: SetMaterialShininess( float p_fShine)
{
	if (m_isLinked && m_pShininess->GetValue() != p_fShine)
	{
		m_pShininess->SetValue( p_fShine);
	}
}
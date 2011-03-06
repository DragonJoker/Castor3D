#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9ShaderProgram.h"
#include "Dx9RenderSystem/Dx9ShaderObject.h"
#include "Dx9RenderSystem/Dx9FrameVariable.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"

using namespace Castor3D;

Dx9ShaderProgram :: Dx9ShaderProgram()
	:	HlslShaderProgram()
{
	m_setFreeLights.insert( std::set<int>::value_type( 0));
	m_setFreeLights.insert( std::set<int>::value_type( 1));
	m_setFreeLights.insert( std::set<int>::value_type( 2));
	m_setFreeLights.insert( std::set<int>::value_type( 3));
	m_setFreeLights.insert( std::set<int>::value_type( 4));
	m_setFreeLights.insert( std::set<int>::value_type( 5));
	m_setFreeLights.insert( std::set<int>::value_type( 6));
	m_setFreeLights.insert( std::set<int>::value_type( 7));
}

Dx9ShaderProgram :: Dx9ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	HlslShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile)
{
	m_setFreeLights.insert( std::set<int>::value_type( 0));
	m_setFreeLights.insert( std::set<int>::value_type( 1));
	m_setFreeLights.insert( std::set<int>::value_type( 2));
	m_setFreeLights.insert( std::set<int>::value_type( 3));
	m_setFreeLights.insert( std::set<int>::value_type( 4));
	m_setFreeLights.insert( std::set<int>::value_type( 5));
	m_setFreeLights.insert( std::set<int>::value_type( 6));
	m_setFreeLights.insert( std::set<int>::value_type( 7));
}

Dx9ShaderProgram :: ~Dx9ShaderProgram()
{
	Cleanup();
}

void Dx9ShaderProgram :: Cleanup()
{
	HlslShaderProgram::Cleanup();
}

void Dx9ShaderProgram :: Initialise()
{
	if ( ! m_isLinked)
	{
		HlslShaderProgram::Initialise();
	}
}

bool Dx9ShaderProgram :: Link()
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError)
	{
		m_isLinked = true;
		l_bReturn = m_isLinked;
	}

	if (l_bReturn)
	{
		shared_ptr< OneFrameVariable<float> > l_fUniform;
		shared_ptr< PointFrameVariable<float, 4> > l_pt4Uniform;
		shared_ptr< PointFrameVariable<float, 3> > l_pt3Uniform;
		shared_ptr< MatrixFrameVariable<float, 4, 4> > l_mtxUniform;

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_AmbientLight"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pAmbientLight = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_AmbientLight")));

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 8));
		l_pt4Uniform->SetName( CU_T( "in_LightsAmbient"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pAmbients = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_LightsAmbient")));

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 8));
		l_pt4Uniform->SetName( CU_T( "in_LightsDiffuse"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pDiffuses = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_LightsDiffuse")));

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 8));
		l_pt4Uniform->SetName( CU_T( "in_LightsSpecular"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pSpeculars = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_LightsSpecular")));

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 8));
		l_pt4Uniform->SetName( CU_T( "in_LightsPosition"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pPositions = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_LightsPosition")));

		l_mtxUniform = shared_ptr< MatrixFrameVariable<float, 4, 4> >( new MatrixFrameVariable<float, 4, 4>( 8));
		l_mtxUniform->SetName( CU_T( "in_LightsOrientation"));
		AddFrameVariable( l_mtxUniform, m_pShaders[eVertexShader]);
		m_pOrientations = static_pointer_cast< Dx9MatrixFrameVariable<float, 4, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_LightsOrientation")));

		l_pt3Uniform = shared_ptr< PointFrameVariable<float, 3> >( new PointFrameVariable<float, 3>( 8));
		l_pt3Uniform->SetName( CU_T( "in_LightsAttenuation"));
		AddFrameVariable( l_pt3Uniform, m_pShaders[eVertexShader]);
		m_pAttenuations = static_pointer_cast< Dx9PointFrameVariable<float, 3> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_LightsAttenuation")));

		l_fUniform = shared_ptr< OneFrameVariable<float> >( new OneFrameVariable<float>( 8));
		l_fUniform->SetName( CU_T( "in_LightsExponent"));
		AddFrameVariable( l_fUniform, m_pShaders[eVertexShader]);
		m_pExponents = static_pointer_cast< Dx9OneFrameVariable<float> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_LightsExponent")));

		l_fUniform = shared_ptr< OneFrameVariable<float> >( new OneFrameVariable<float>( 8));
		l_fUniform->SetName( CU_T( "in_LightsCutOff"));
		AddFrameVariable( l_fUniform, m_pShaders[eVertexShader]);
		m_pCutOffs = static_pointer_cast< Dx9OneFrameVariable<float> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_LightsCutOff")));

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_MatAmbient"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pAmbient = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_MatAmbient")));

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_MatDiffuse"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pDiffuse = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_MatDiffuse")));

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_MatEmissive"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pEmissive = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_MatEmissive")));

		l_pt4Uniform = shared_ptr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_MatSpecular"));
		AddFrameVariable( l_pt4Uniform, m_pShaders[eVertexShader]);
		m_pSpecular = static_pointer_cast< Dx9PointFrameVariable<float, 4> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_MatSpecular")));

		l_fUniform = shared_ptr< OneFrameVariable<float> >( new OneFrameVariable<float>( 1));
		l_fUniform->SetName( CU_T( "in_MatShininess"));
		AddFrameVariable( l_fUniform, m_pShaders[eVertexShader]);
		m_pShininess = static_pointer_cast< Dx9OneFrameVariable<float> >( static_pointer_cast<Dx9ShaderObject>( m_pShaders[eVertexShader])->GetD3dFrameVariable( CU_T( "in_MatShininess")));
	}

	return l_bReturn;
}

void Dx9ShaderProgram :: RetrieveLinkerLog( String & strLog)
{
	if ( ! RenderSystem::UseShaders())
	{
		strLog = D3dEnum::GetHlslErrorString( 0);
		return;
	}
}

void Dx9ShaderProgram :: Begin()
{
	if ( ! RenderSystem::UseShaders() || ! m_enabled || m_bError)
	{
		return;
	}

	Dx9RenderSystem::GetSingletonPtr()->SetCurrentShaderProgram( this);

	for (size_t i = 0 ; i < eNbShaderTypes ; i++)
	{
		if (m_pShaders[i] != NULL)
		{
			static_pointer_cast<Dx9ShaderObject>( m_pShaders[i])->Begin();
		}
	}
}

void Dx9ShaderProgram :: ApplyAllVariables()
{
}

void Dx9ShaderProgram :: End()
{
	if ( ! RenderSystem::UseShaders() || ! m_enabled)
	{
		return;
	}

	for (size_t i = 0 ; i < eNbShaderTypes ; i++)
	{
		if (m_pShaders[i] != NULL)
		{
			static_pointer_cast<Dx9ShaderObject>( m_pShaders[i])->End();
		}
	}
}

void Dx9ShaderProgram :: AddFrameVariable( FrameVariablePtr p_pVariable, HlslShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);

	if (p_pVariable != NULL)
	{
		switch (p_pVariable->GetType())
		{
		case FrameVariable::eOne:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				l_pObject->AddFrameVariable<int>( static_pointer_cast< OneFrameVariable<int> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				l_pObject->AddFrameVariable<float>( static_pointer_cast< OneFrameVariable<float> >( p_pVariable));
			}
			else
			{
				CASTOR_EXCEPTION( "Unsupported variable subtype");
			}
			break;

		case FrameVariable::eVec1:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				l_pObject->AddFrameVariable<int, 1>( static_pointer_cast< PointFrameVariable<int, 1> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				l_pObject->AddFrameVariable<float, 1>( static_pointer_cast< PointFrameVariable<float, 1> >( p_pVariable));
			}
			else
			{
				CASTOR_EXCEPTION( "Unsupported variable subtype");
			}
			break;

		case FrameVariable::eVec2:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				l_pObject->AddFrameVariable<int, 2>( static_pointer_cast< PointFrameVariable<int, 2> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				l_pObject->AddFrameVariable<float, 2>( static_pointer_cast< PointFrameVariable<float, 2> >( p_pVariable));
			}
			else
			{
				CASTOR_EXCEPTION( "Unsupported variable subtype");
			}
			break;

		case FrameVariable::eVec3:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				l_pObject->AddFrameVariable<int, 3>( static_pointer_cast< PointFrameVariable<int, 3> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				l_pObject->AddFrameVariable<float, 3>( static_pointer_cast< PointFrameVariable<float, 3> >( p_pVariable));
			}
			else
			{
				CASTOR_EXCEPTION( "Unsupported variable subtype");
			}
			break;

		case FrameVariable::eVec4:
			if (p_pVariable->GetSubType() == typeid( int).name())
			{
				l_pObject->AddFrameVariable<int, 4>( static_pointer_cast< PointFrameVariable<int, 4> >( p_pVariable));
			}
			else if (p_pVariable->GetSubType() == typeid( float).name())
			{
				l_pObject->AddFrameVariable<float, 4>( static_pointer_cast< PointFrameVariable<float, 4> >( p_pVariable));
			}
			else
			{
				CASTOR_EXCEPTION( "Unsupported variable subtype");
			}
			break;

		case FrameVariable::eMat4x4:
			if (p_pVariable->GetSubType() == typeid( float).name())
			{
				l_pObject->AddFrameVariable<float, 4, 4>( static_pointer_cast< MatrixFrameVariable<float, 4, 4> >( p_pVariable));
			}
			else
			{
				CASTOR_EXCEPTION( "Unsupported variable subtype");
			}
			break;

		default:
			CASTOR_EXCEPTION( "Unsupported variable type");
		}
	}
}

int Dx9ShaderProgram :: AssignLight()
{
	int l_iReturn = GL_INVALID_INDEX;

	if (m_setFreeLights.size() > 0)
	{
		l_iReturn = (* m_setFreeLights.begin());
		m_setFreeLights.erase( m_setFreeLights.begin());

	}

	return l_iReturn;
}

void Dx9ShaderProgram :: FreeLight( int p_iIndex)
{
	if (p_iIndex != GL_INVALID_INDEX)
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

void Dx9ShaderProgram :: SetAmbientLight( const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pAmbientLight->SetValue( p_crColour, 0);
	}
}

void Dx9ShaderProgram :: SetLightAmbient( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pAmbients->SetValue( p_crColour, p_iIndex);
	}
}

void Dx9ShaderProgram :: SetLightDiffuse( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pDiffuses->SetValue( p_crColour, p_iIndex);
	}
}

void Dx9ShaderProgram :: SetLightSpecular( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pSpeculars->SetValue( p_crColour, p_iIndex);
	}
}

void Dx9ShaderProgram :: SetLightPosition( int p_iIndex, const Point4f & p_ptPosition)
{
	if (m_isLinked)
	{
		m_pPositions->SetValue( p_ptPosition, p_iIndex);
	}
}

void Dx9ShaderProgram :: SetLightOrientation( int p_iIndex, const Matrix4x4r & p_mtxOrientation)
{
	if (m_isLinked)
	{
		m_pOrientations->SetValue( Matrix<float, 4, 4>( p_mtxOrientation), p_iIndex);
	}
}

void Dx9ShaderProgram :: SetLightAttenuation( int p_iIndex, const Point3f & p_ptAtt)
{
	if (m_isLinked)
	{
		m_pAttenuations->SetValue( p_ptAtt, p_iIndex);
	}
}

void Dx9ShaderProgram :: SetLightExponent( int p_iIndex, float p_fExp)
{
	if (m_isLinked)
	{
		m_pExponents->SetValue( p_fExp, p_iIndex);
	}
}

void Dx9ShaderProgram :: SetLightCutOff( int p_iIndex, float p_fCut)
{
	if (m_isLinked)
	{
		m_pCutOffs->SetValue( p_fCut, p_iIndex);
	}
}

void Dx9ShaderProgram :: SetMaterialAmbient( const Point4f & p_crColour)
{
	if (m_isLinked && m_pAmbient->GetValue() != p_crColour)
	{
		m_pAmbient->SetValue( p_crColour);
	}
}

void Dx9ShaderProgram :: SetMaterialDiffuse( const Point4f & p_crColour)
{
	if (m_isLinked && m_pDiffuse->GetValue() != p_crColour)
	{
		m_pDiffuse->SetValue( p_crColour);
	}
}

void Dx9ShaderProgram :: SetMaterialSpecular( const Point4f & p_crColour)
{
	if (m_isLinked && m_pSpecular->GetValue() != p_crColour)
	{
		m_pSpecular->SetValue( p_crColour);
	}
}

void Dx9ShaderProgram :: SetMaterialEmissive( const Point4f & p_crColour)
{
	if (m_isLinked && m_pEmissive->GetValue() != p_crColour)
	{
		m_pEmissive->SetValue( p_crColour);
	}
}

void Dx9ShaderProgram :: SetMaterialShininess( float p_fShine)
{
	if (m_isLinked && m_pShininess->GetValue() != p_fShine)
	{
		m_pShininess->SetValue( p_fShine);
	}
}
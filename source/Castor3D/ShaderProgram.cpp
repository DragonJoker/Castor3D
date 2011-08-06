#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/ShaderObject.hpp"

using namespace Castor3D;

//*************************************************************************************************

struct VariableApply
{
	template <class T> void operator()(T & p) const
	{
		CASTOR_TRACK;
		p->Apply();
	}
};

//*************************************************************************************************

ShaderProgramBase :: ShaderProgramBase( eSHADER_LANGUAGE p_eType, const StringArray & p_shaderFiles)
	:	m_bLinked( false)
	,	m_bError( false)
	,	m_bEnabled( true)
	,	m_eType( p_eType)
{
	CASTOR_TRACK;
	m_setFreeLights.insert( std::set<int>::value_type( 0));
	m_setFreeLights.insert( std::set<int>::value_type( 1));
	m_setFreeLights.insert( std::set<int>::value_type( 2));
	m_setFreeLights.insert( std::set<int>::value_type( 3));
	m_setFreeLights.insert( std::set<int>::value_type( 4));
	m_setFreeLights.insert( std::set<int>::value_type( 5));
	m_setFreeLights.insert( std::set<int>::value_type( 6));
	m_setFreeLights.insert( std::set<int>::value_type( 7));

	for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT && i < static_cast< int>( p_shaderFiles.size()) ; i++)
	{
		if ( ! p_shaderFiles[i].empty())
		{
			CreateObject( eSHADER_TYPE( i));
			SetProgramFile( p_shaderFiles[i], eSHADER_TYPE( i));
		}
	}
}

ShaderProgramBase :: ShaderProgramBase( eSHADER_LANGUAGE p_eType)
	:	m_bLinked( false)
	,	m_bError( false)
	,	m_bEnabled( true)
	,	m_eType( p_eType)
{
	CASTOR_TRACK;
	m_setFreeLights.insert( std::set<int>::value_type( 0));
	m_setFreeLights.insert( std::set<int>::value_type( 1));
	m_setFreeLights.insert( std::set<int>::value_type( 2));
	m_setFreeLights.insert( std::set<int>::value_type( 3));
	m_setFreeLights.insert( std::set<int>::value_type( 4));
	m_setFreeLights.insert( std::set<int>::value_type( 5));
	m_setFreeLights.insert( std::set<int>::value_type( 6));
	m_setFreeLights.insert( std::set<int>::value_type( 7));
}

ShaderProgramBase :: ~ShaderProgramBase()
{
	CASTOR_TRACK;
	Cleanup();
}

void ShaderProgramBase :: Cleanup()
{
	CASTOR_TRACK;
	for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
	{
		if (m_pShaders[i])
		{
			m_pShaders[i]->DestroyProgram();
		}
	}

//	m_listFrameVariables.clear();
	m_pathFile.clear();
}

void ShaderProgramBase :: Initialise()
{
	CASTOR_TRACK;
	if ( ! m_bLinked && ! m_bError)
	{
		if (RenderSystem::UseShaders())
		{
			Cleanup();

			for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
			{
				if (m_pShaders[i] && ! m_pShaders[i]->GetSource().empty())
				{
					m_pShaders[i]->CreateProgram();
				}
			}

			for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
			{
				if (m_pShaders[i] && ! m_pShaders[i]->GetSource().empty())
				{
					if ( ! m_pShaders[i]->Compile())
					{
						Logger::LogError( cuT( "ShaderProgram :: LoadfromMemory - ***COMPILER ERROR"));
						Cleanup();
						return;
					}
				}
			}

			if ( ! Link())
			{
				Logger::LogError( cuT( "ShaderProgram :: LoadfromMemory - ***LINKER ERROR"));
				Cleanup();
				return;
			}

			Logger::LogMessage( cuT( "ShaderProgram :: LoadfromMemory - ***Program Linked successfully"));
		}
	}
}

void ShaderProgramBase :: CreateObject( eSHADER_TYPE p_eType)
{
	CASTOR_TRACK;
	CASTOR_ASSERT( p_eType > eSHADER_TYPE_NONE && p_eType < eSHADER_TYPE_COUNT);

	if ( ! m_pShaders[p_eType])
	{
		m_pShaders[p_eType] = _createObject( p_eType);

		if ( ! m_pathFile.empty())
		{
			m_pShaders[p_eType]->SetFile( m_pathFile);
		}
	}
}

bool ShaderProgramBase :: Link()
{
	CASTOR_TRACK;
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError)
	{
		if ( ! m_bLinked)
		{
			m_pAmbientLight = CreateFrameVariable<Point4fFrameVariable>( 1, eSHADER_TYPE_PIXEL);
			m_pAmbientLight->SetName( cuT( "in_AmbientLight"));

			m_pAmbients = CreateFrameVariable<Point4fFrameVariable>( 8, eSHADER_TYPE_PIXEL);
			m_pAmbients->SetName( cuT( "in_LightsAmbient"));

			m_pDiffuses = CreateFrameVariable<Point4fFrameVariable>( 8, eSHADER_TYPE_PIXEL);
			m_pDiffuses->SetName( cuT( "in_LightsDiffuse"));

			m_pSpeculars = CreateFrameVariable<Point4fFrameVariable>( 8, eSHADER_TYPE_PIXEL);
			m_pSpeculars->SetName( cuT( "in_LightsSpecular"));

			m_pPositions = CreateFrameVariable<Point4fFrameVariable>( 8, eSHADER_TYPE_PIXEL);
			m_pPositions->SetName( cuT( "in_LightsPosition"));

			m_pOrientations = CreateFrameVariable<Matrix4x4fFrameVariable>( 8, eSHADER_TYPE_PIXEL);
			m_pOrientations->SetName( cuT( "in_LightsOrientation"));

			m_pAttenuations = CreateFrameVariable<Point3fFrameVariable>( 8, eSHADER_TYPE_PIXEL);
			m_pAttenuations->SetName( cuT( "in_LightsAttenuation"));

			m_pExponents = CreateFrameVariable<OneFloatFrameVariable>( 8, eSHADER_TYPE_PIXEL);
			m_pExponents->SetName( cuT( "in_LightsExponent"));

			m_pCutOffs = CreateFrameVariable<OneFloatFrameVariable>( 8, eSHADER_TYPE_PIXEL);
			m_pCutOffs->SetName( cuT( "in_LightsCutOff"));


			m_pAmbient = CreateFrameVariable<Point4fFrameVariable>( 1, eSHADER_TYPE_PIXEL);
			m_pAmbient->SetName( cuT( "in_MatAmbient"));

			m_pDiffuse = CreateFrameVariable<Point4fFrameVariable>( 1, eSHADER_TYPE_PIXEL);
			m_pDiffuse->SetName( cuT( "in_MatDiffuse"));

			m_pEmissive = CreateFrameVariable<Point4fFrameVariable>( 1, eSHADER_TYPE_PIXEL);
			m_pEmissive->SetName( cuT( "in_MatEmissive"));

			m_pSpecular = CreateFrameVariable<Point4fFrameVariable>( 1, eSHADER_TYPE_PIXEL);
			m_pSpecular->SetName( cuT( "in_MatSpecular"));

			m_pShininess = CreateFrameVariable<OneFloatFrameVariable>( 1, eSHADER_TYPE_PIXEL);
			m_pShininess->SetName( cuT( "in_MatShininess"));

			m_bLinked = true;
		}

		l_bReturn = m_bLinked;
	}

	return l_bReturn;
}

void ShaderProgramBase :: Begin()
{
	CASTOR_TRACK;

	if ( ! RenderSystem::UseShaders() || ! m_bEnabled || ! m_bLinked || m_bError)
	{
		return;
	}

	RenderSystem::GetSingletonPtr()->SetCurrentShaderProgram( this);

	for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
	{
		if (m_pShaders[i])
		{
			m_pShaders[i]->Bind();
		}
	}

	ApplyAllVariables();
}

void ShaderProgramBase :: End()
{
	CASTOR_TRACK;

	if ( ! RenderSystem::UseShaders() || ! m_bEnabled)
	{
		return;
	}

	for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
	{
		if (m_pShaders[i])
		{
			m_pShaders[i]->Unbind();
		}
	}
}

void ShaderProgramBase :: SetProgramFile( String const & p_strFile, eSHADER_TYPE p_eType)
{
	CASTOR_TRACK;
	CASTOR_ASSERT( p_eType > eSHADER_TYPE_NONE && p_eType < eSHADER_TYPE_COUNT);

	if (RenderSystem::HasShaderType( p_eType) && m_pShaders[p_eType])
	{
		m_pShaders[p_eType]->SetFile( p_strFile);
		ResetToCompile();
	}
}

ShaderObjectPtr ShaderProgramBase :: GetProgram( eSHADER_TYPE p_eType)
{
	CASTOR_TRACK;
	CASTOR_ASSERT( p_eType > eSHADER_TYPE_NONE && p_eType < eSHADER_TYPE_COUNT);
	return m_pShaders[p_eType];
}

void ShaderProgramBase :: SetFile( Path const & p_path)
{
	CASTOR_TRACK;
	m_pathFile = p_path;

	for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
	{
		if (m_pShaders[i] && RenderSystem::HasShaderType( eSHADER_TYPE( i)))
		{
			m_pShaders[i]->SetFile( p_path);
		}
	}

	ResetToCompile();
}

void ShaderProgramBase :: SetProgramText( String const & p_strFile, eSHADER_TYPE p_eType)
{
	CASTOR_TRACK;
	CASTOR_ASSERT( p_eType > eSHADER_TYPE_NONE && p_eType < eSHADER_TYPE_COUNT);

	if (RenderSystem::HasShaderType( p_eType) && m_pShaders[p_eType])
	{
		m_pShaders[p_eType]->SetFile( cuEmptyString);
		m_pShaders[p_eType]->SetSource( p_strFile);
		ResetToCompile();
	}
}

int ShaderProgramBase :: AssignLight()
{
	CASTOR_TRACK;
	int l_iReturn = -1;

	if (m_setFreeLights.size() > 0)
	{
		l_iReturn = (* m_setFreeLights.begin());
		m_setFreeLights.erase( m_setFreeLights.begin());

	}

	return l_iReturn;
}

void ShaderProgramBase :: FreeLight( int p_iIndex)
{
	CASTOR_TRACK;

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

void ShaderProgramBase :: SetAmbientLight( const Point4f & p_crColour)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pAmbientLight->SetValue( p_crColour, 0);
	}
}

void ShaderProgramBase :: SetLightAmbient( int p_iIndex, const Point4f & p_crColour)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pAmbients->SetValue( p_crColour, p_iIndex);
	}
}

void ShaderProgramBase :: SetLightDiffuse( int p_iIndex, const Point4f & p_crColour)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pDiffuses->SetValue( p_crColour, p_iIndex);
	}
}

void ShaderProgramBase :: SetLightSpecular( int p_iIndex, const Point4f & p_crColour)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pSpeculars->SetValue( p_crColour, p_iIndex);
	}
}

void ShaderProgramBase :: SetLightPosition( int p_iIndex, const Point4f & p_ptPosition)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pPositions->SetValue( p_ptPosition, p_iIndex);
	}
}

void ShaderProgramBase :: SetLightOrientation( int p_iIndex, Matrix4x4r const & p_mtxOrientation)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pOrientations->SetValue( Matrix<float, 4, 4>( p_mtxOrientation), p_iIndex);
	}
}

void ShaderProgramBase :: SetLightAttenuation( int p_iIndex, const Point3f & p_ptAtt)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pAttenuations->SetValue( p_ptAtt, p_iIndex);
	}
}

void ShaderProgramBase :: SetLightExponent( int p_iIndex, float p_fExp)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pExponents->SetValue( p_fExp, p_iIndex);
	}
}

void ShaderProgramBase :: SetLightCutOff( int p_iIndex, float p_fCut)
{
	CASTOR_TRACK;

	if (m_bLinked)
	{
		m_pCutOffs->SetValue( p_fCut, p_iIndex);
	}
}

void ShaderProgramBase :: SetMaterialAmbient( const Point4f & p_crColour)
{
	CASTOR_TRACK;

	if (m_bLinked && m_pAmbient->GetValue() != p_crColour)
	{
		m_pAmbient->SetValue( p_crColour);
	}
}

void ShaderProgramBase :: SetMaterialDiffuse( const Point4f & p_crColour)
{
	CASTOR_TRACK;

	if (m_bLinked && m_pDiffuse->GetValue() != p_crColour)
	{
		m_pDiffuse->SetValue( p_crColour);
	}
}

void ShaderProgramBase :: SetMaterialSpecular( const Point4f & p_crColour)
{
	CASTOR_TRACK;

	if (m_bLinked && m_pSpecular->GetValue() != p_crColour)
	{
		m_pSpecular->SetValue( p_crColour);
	}
}

void ShaderProgramBase :: SetMaterialEmissive( const Point4f & p_crColour)
{
	CASTOR_TRACK;

	if (m_bLinked && m_pEmissive->GetValue() != p_crColour)
	{
		m_pEmissive->SetValue( p_crColour);
	}
}

void ShaderProgramBase :: SetMaterialShininess( float p_fShine)
{
	CASTOR_TRACK;

	if (m_bLinked && m_pShininess->GetValue() != p_fShine)
	{
		m_pShininess->SetValue( p_fShine);
	}
}

void ShaderProgramBase :: ResetToCompile()
{
	CASTOR_TRACK;
	m_bLinked = false;
	m_bError = false;
}

void ShaderProgramBase :: ApplyAllVariables()
{
	CASTOR_TRACK;
	std::for_each( m_listFrameVariables.begin(), m_listFrameVariables.end(), VariableApply());
}

void ShaderProgramBase :: _addFrameVariable( FrameVariablePtr p_pVariable, ShaderObjectPtr p_pTarget)
{
	CASTOR_TRACK;
	CASTOR_ASSERT( p_pTarget && p_pVariable);
	m_listFrameVariables.push_back( p_pVariable);
	p_pTarget->AddFrameVariable( p_pVariable);
}

//*************************************************************************************************

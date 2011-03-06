#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl4ShaderProgram.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"
#include "Gl3RenderSystem/Gl3Context.h"
#include "Gl3RenderSystem/Gl3Buffer.h"
#include "Gl3RenderSystem/Gl4LightRenderer.h"

using namespace Castor3D;

Gl4ShaderProgram :: Gl4ShaderProgram()
	:	GlShaderProgram()
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

Gl4ShaderProgram :: Gl4ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	GlShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile)
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

Gl4ShaderProgram :: ~Gl4ShaderProgram()
{
	m_mapUniformBuffers.clear();
}

bool Gl4ShaderProgram :: Link()
{
	if ( ! Gl3RenderSystem::UseShaders() || m_bError)
	{
		return false;
	}

	if (m_isLinked)  // already linked, detach everything first
	{
		Logger::LogWarning( CU_T( "Gl4ShaderProgram :: Link - Object is already linked, trying to link again"));

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
/*
	glBindFragDataLocation( m_programObject, 1, "out_FragColor");
	CheckGlError( CU_T( "Gl4ShaderProgram :: Link - glBindFragDataLocation"));
*/
	GLint linked = 0;
	CheckGlError( glLinkProgram( m_programObject), CU_T( "Gl4ShaderProgram :: Link - glLinkProgram"));
	CheckGlError( glGetProgramiv( m_programObject, GL_LINK_STATUS, & linked), CU_T( "Gl4ShaderProgram :: Link - glGetProgramiv"));

	if (linked)
	{
		m_isLinked = true;

		m_pLightsUniformBuffer = BufferManager::CreateBuffer<unsigned char, GlUniformBufferObject>( CU_T( "in_Lights"));
		m_pLightsUniformBuffer->Initialise( this);
		m_pAmbients			= m_pLightsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(		CU_T( "in_LightsAmbient"),		8);
		m_pDiffuses			= m_pLightsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(		CU_T( "in_LightsDiffuse"),		8);
		m_pSpeculars		= m_pLightsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(		CU_T( "in_LightsSpecular"),		8);
		m_pPositions		= m_pLightsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(		CU_T( "in_LightsPosition"),		8);
		m_pAttenuations		= m_pLightsUniformBuffer->CreateVariable<GlUboPoint3fVariable>(		CU_T( "in_LightsAttenuation"),	8);
		m_pOrientations		= m_pLightsUniformBuffer->CreateVariable<GlUboMatrix4x4fVariable>(	CU_T( "in_LightsOrientation"),	8);
		m_pExponents		= m_pLightsUniformBuffer->CreateVariable<GlUboFloatVariable>(		CU_T( "in_LightsExponent"),		8);
		m_pCutOffs			= m_pLightsUniformBuffer->CreateVariable<GlUboFloatVariable>(		CU_T( "in_LightsCutOff"),		8);

		if (m_pAmbients != NULL)
		{
			m_pAmbients->SetValues(		m_vAmbients);
		}
		if (m_pDiffuses != NULL)
		{
			m_pDiffuses->SetValues(		m_vDiffuses);
		}
		if (m_pSpeculars != NULL)
		{
			m_pSpeculars->SetValues(	m_vSpeculars);
		}
		if (m_pPositions != NULL)
		{
			m_pPositions->SetValues(	m_vPositions);
		}
		if (m_pAttenuations != NULL)
		{
			m_pAttenuations->SetValues(	m_vAttenuations);
		}
		if (m_pOrientations != NULL)
		{
			m_pOrientations->SetValues(	m_mOrientations);
		}
		if (m_pExponents != NULL)
		{
			m_pExponents->SetValues(	m_rExponents);
		}
		if (m_pCutOffs != NULL)
		{
			m_pCutOffs->SetValues(		m_rCutOffs);
		}

		m_mapUniformBuffers.insert( GlUniformBufferObjectPtrStrMap::value_type(  CU_T( "in_Lights"), m_pLightsUniformBuffer));

		m_pMatsUniformBuffer = BufferManager::CreateBuffer<unsigned char, GlUniformBufferObject>( CU_T( "in_Material"));
		m_pMatsUniformBuffer->Initialise( this);
		m_pAmbient		= m_pMatsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(	CU_T( "in_MatAmbient"),		1);
		m_pDiffuse		= m_pMatsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(	CU_T( "in_MatDiffuse"),		1);
		m_pEmissive		= m_pMatsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(	CU_T( "in_MatEmissive"),	1);
		m_pSpecular		= m_pMatsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(	CU_T( "in_MatSpecular"),	1);
		m_pShininess	= m_pMatsUniformBuffer->CreateVariable<GlUboFloatVariable>(		CU_T( "in_MatShininess"),	1);

		if (m_pAmbient != NULL)
		{
			m_pAmbient->SetValues( & m_vAmbient);
		}
		if (m_pDiffuse != NULL)
		{
			m_pDiffuse->SetValues( & m_vDiffuse);
		}
		if (m_pEmissive != NULL)
		{
			m_pEmissive->SetValues( & m_vEmissive);
		}
		if (m_pSpecular != NULL)
		{
			m_pSpecular->SetValues( & m_vSpecular);
		}
		if (m_pShininess != NULL)
		{
			m_pShininess->SetValues( & m_rShininess);
		}

		m_mapUniformBuffers.insert( GlUniformBufferObjectPtrStrMap::value_type( CU_T( "in_Material"), m_pMatsUniformBuffer));
	}
	else
	{
		m_bError = true;
		String l_strLog;
		RetrieveLinkerLog( l_strLog);
		Logger::LogError( CU_T( "Gl4ShaderProgram :: Link - Linker error"));
	}

	return m_isLinked;
}

int Gl4ShaderProgram :: AssignLight()
{
	int l_iReturn = GL_INVALID_INDEX;

	if (m_setFreeLights.size() > 0)
	{
		l_iReturn = (* m_setFreeLights.begin());
		m_setFreeLights.erase( m_setFreeLights.begin());

	}

	return l_iReturn;
}

void Gl4ShaderProgram :: FreeLight( int p_iIndex)
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

void Gl4ShaderProgram :: Begin()
{
	Gl3RenderSystem::GetSingletonPtr()->SetCurrentShaderProgram( this);
	GlShaderProgram::Begin();
}

void Gl4ShaderProgram :: ApplyAllVariables()
{
	GlShaderProgram::ApplyAllVariables();

	for (GlUniformBufferObjectPtrStrMap::iterator l_it = m_mapUniformBuffers.begin() ; l_it != m_mapUniformBuffers.end() ; ++l_it)
	{
		l_it->second->Activate();
	}
}

void Gl4ShaderProgram :: End()
{
	if ( ! Gl3RenderSystem::UseShaders() || ! m_enabled)
	{
		return;
	}

	for (GlUniformBufferObjectPtrStrMap::iterator l_it = m_mapUniformBuffers.begin() ; l_it != m_mapUniformBuffers.end() ; ++l_it)
	{
		l_it->second->Deactivate();
	}

	CheckGlError( glUseProgram( 0), CU_T( "Gl4ShaderProgram :: End - glUseProgram"));
}

GlUniformBufferObjectPtr Gl4ShaderProgram :: GetUniformBuffer( const String & p_strName)
{
	GlUniformBufferObjectPtr l_pReturn;

	if (m_mapUniformBuffers.find( p_strName) != m_mapUniformBuffers.end())
	{
		l_pReturn = m_mapUniformBuffers.find( p_strName)->second;
	}

	return l_pReturn;
}

void Gl4ShaderProgram :: SetLightValues( int p_iIndex, Gl4LightRenderer * p_pLightRenderer)
{
	if (p_iIndex >= 0 && p_iIndex < 8)
	{
		m_vAmbients[p_iIndex] 		= p_pLightRenderer->GetAmbient();
		m_vDiffuses[p_iIndex] 		= p_pLightRenderer->GetDiffuse();
		m_vSpeculars[p_iIndex] 		= p_pLightRenderer->GetSpecular();
		m_vPositions[p_iIndex] 		= p_pLightRenderer->GetPosition();
		m_vAttenuations[p_iIndex] 	= p_pLightRenderer->GetAttenuation();
		m_mOrientations[p_iIndex] 	= p_pLightRenderer->GetOrientation();
		m_rExponents[p_iIndex] 		= p_pLightRenderer->GetExponent();
		m_rCutOffs[p_iIndex]		= p_pLightRenderer->GetCutOff();

		m_pAmbients->SetValue(		m_vAmbients[p_iIndex],		p_iIndex);
		m_pDiffuses->SetValue(		m_vDiffuses[p_iIndex],		p_iIndex);
		m_pSpeculars->SetValue(		m_vSpeculars[p_iIndex],		p_iIndex);
		m_pPositions->SetValue(		m_vPositions[p_iIndex],		p_iIndex);
		m_pAttenuations->SetValue(	m_vAttenuations[p_iIndex],	p_iIndex);
		m_pOrientations->SetValue(	m_mOrientations[p_iIndex],	p_iIndex);
		m_pExponents->SetValue(		m_rExponents[p_iIndex],		p_iIndex);
		m_pCutOffs->SetValue(		m_rCutOffs[p_iIndex],		p_iIndex);
	}
}

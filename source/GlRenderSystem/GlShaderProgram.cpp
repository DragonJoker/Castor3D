#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/GlShaderObject.hpp"
#include "GlRenderSystem/GlFrameVariable.hpp"
#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/GlLightRenderer.hpp"
#include "GlRenderSystem/GlFrameVariable.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

GlShaderProgram :: GlShaderProgram()
	:	GlslShaderProgram(),
		m_programObject( 0)
{
}

GlShaderProgram :: GlShaderProgram( const StringArray & p_shaderFiles)
	:	GlslShaderProgram( p_shaderFiles),
		m_programObject( 0)
{
}

GlShaderProgram :: ~GlShaderProgram()
{
	if (RenderSystem::UseShaders())
	{
		if (m_programObject != 0)
		{
			OpenGl::DeleteProgram( m_programObject);
		}
	}
}

void GlShaderProgram :: Initialise()
{
	CASTOR_TRACK;
	if ( ! m_bLinked)
	{
		if (RenderSystem::UseShaders())
		{
			m_programObject = OpenGl::CreateProgram();
		}

		GlslShaderProgram::Initialise();
	}
}

bool GlShaderProgram :: Link()
{
	CASTOR_TRACK;
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError)
	{
		l_bReturn = true;

		for (int i = eSHADER_TYPE_VERTEX ; i < eSHADER_TYPE_COUNT ; i++)
		{
			if (m_pShaders[i])
			{
				static_pointer_cast<GlShaderObject>( m_pShaders[i])->AttachTo( this);
			}
		}

		GLint l_iLinked = 0;
		GLint l_iValidated = 0;
		l_bReturn &= OpenGl::LinkProgram( m_programObject);
		l_bReturn &= OpenGl::GetProgramiv( m_programObject, GL_LINK_STATUS, & l_iLinked);
		l_bReturn &= OpenGl::GetProgramiv( m_programObject, GL_VALIDATE_STATUS, & l_iValidated);

		RetrieveLinkerLog( m_linkerLog);

		if (m_linkerLog.size() > 0)
		{
			Logger::LogMessage( m_linkerLog);
		}

		if (l_iLinked && l_iValidated && m_linkerLog.find( "ERROR") == String::npos)
		{
#if CASTOR_USE_OPENGL4
			m_pLightsUniformBuffer = Root::GetSingletonPtr()->GetBufferManager()->CreateBuffer<unsigned char, GlUniformBufferObject>( cuT( "in_Lights"));
			m_pLightsUniformBuffer->Initialise( this);
			m_pAmbients			= m_pLightsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(		cuT( "in_LightsAmbient"),		8);
			m_pDiffuses			= m_pLightsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(		cuT( "in_LightsDiffuse"),		8);
			m_pSpeculars		= m_pLightsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(		cuT( "in_LightsSpecular"),		8);
			m_pPositions		= m_pLightsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(		cuT( "in_LightsPosition"),		8);
			m_pAttenuations		= m_pLightsUniformBuffer->CreateVariable<GlUboPoint3fVariable>(		cuT( "in_LightsAttenuation"),	8);
			m_pOrientations		= m_pLightsUniformBuffer->CreateVariable<GlUboMatrix4x4fVariable>(	cuT( "in_LightsOrientation"),	8);
			m_pExponents		= m_pLightsUniformBuffer->CreateVariable<GlUboFloatVariable>(		cuT( "in_LightsExponent"),		8);
			m_pCutOffs			= m_pLightsUniformBuffer->CreateVariable<GlUboFloatVariable>(		cuT( "in_LightsCutOff"),		8);

			if (m_pAmbients)
			{
				m_pAmbients->SetValues(		m_vAmbients);
			}
			if (m_pDiffuses)
			{
				m_pDiffuses->SetValues(		m_vDiffuses);
			}
			if (m_pSpeculars)
			{
				m_pSpeculars->SetValues(	m_vSpeculars);
			}
			if (m_pPositions)
			{
				m_pPositions->SetValues(	m_vPositions);
			}
			if (m_pAttenuations)
			{
				m_pAttenuations->SetValues(	m_vAttenuations);
			}
			if (m_pOrientations)
			{
				m_pOrientations->SetValues(	m_mOrientations);
			}
			if (m_pExponents)
			{
				m_pExponents->SetValues(	m_rExponents);
			}
			if (m_pCutOffs)
			{
				m_pCutOffs->SetValues(		m_rCutOffs);
			}

			m_mapUniformBuffers.insert( GlUniformBufferObjectPtrStrMap::value_type(  cuT( "in_Lights"), m_pLightsUniformBuffer));

			m_pMatsUniformBuffer = Root::GetSingletonPtr()->GetBufferManager()->CreateBuffer<unsigned char, GlUniformBufferObject>( cuT( "in_Material"));
			m_pMatsUniformBuffer->Initialise( this);
			m_pAmbient		= m_pMatsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(	cuT( "in_MatAmbient"),		1);
			m_pDiffuse		= m_pMatsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(	cuT( "in_MatDiffuse"),		1);
			m_pEmissive		= m_pMatsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(	cuT( "in_MatEmissive"),	1);
			m_pSpecular		= m_pMatsUniformBuffer->CreateVariable<GlUboPoint4fVariable>(	cuT( "in_MatSpecular"),	1);
			m_pShininess	= m_pMatsUniformBuffer->CreateVariable<GlUboFloatVariable>(		cuT( "in_MatShininess"),	1);

			if (m_pAmbient)
			{
				m_pAmbient->SetValues( & m_vAmbient);
			}
			if (m_pDiffuse)
			{
				m_pDiffuse->SetValues( & m_vDiffuse);
			}
			if (m_pEmissive)
			{
				m_pEmissive->SetValues( & m_vEmissive);
			}
			if (m_pSpecular)
			{
				m_pSpecular->SetValues( & m_vSpecular);
			}
			if (m_pShininess)
			{
				m_pShininess->SetValues( & m_rShininess);
			}

			m_mapUniformBuffers.insert( GlUniformBufferObjectPtrStrMap::value_type( cuT( "in_Material"), m_pMatsUniformBuffer));
			m_bLinked = true;
#else
			ShaderProgramBase::Link();
#endif
		}
		else
		{
			m_bError = true;
		}

		l_bReturn = m_bLinked;
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

	bool l_bReturn = true;
	l_bReturn &= OpenGl::GetProgramiv(m_programObject, GL_INFO_LOG_LENGTH , & l_iLength);

	if (l_iLength > 1)
	{
		char * l_pTmp = new char[l_iLength];
		l_bReturn &= OpenGl::GetProgramInfoLog( m_programObject, l_iLength, & l_uiLength, l_pTmp);
		strLog = l_pTmp;
		delete [] l_pTmp;
	}
}

void GlShaderProgram :: Begin()
{
	CASTOR_TRACK;
	if ( ! RenderSystem::UseShaders() || m_programObject == 0 || ! m_bEnabled || ! m_bLinked || m_bError)
	{
		return;
	}

	OpenGl::UseProgram( m_programObject);

	ShaderProgramBase::Begin();
}

void GlShaderProgram :: ApplyAllVariables()
{
	CASTOR_TRACK;
	ShaderProgramBase::ApplyAllVariables();

#if CASTOR_USE_OPENGL4
	for (GlUniformBufferObjectPtrStrMap::iterator l_it = m_mapUniformBuffers.begin() ; l_it != m_mapUniformBuffers.end() ; ++l_it)
	{
		l_it->second->Activate();
	}
#endif
}

void GlShaderProgram :: End()
{
	CASTOR_TRACK;
#if CASTOR_USE_OPENGL4
	for (GlUniformBufferObjectPtrStrMap::iterator l_it = m_mapUniformBuffers.begin() ; l_it != m_mapUniformBuffers.end() ; ++l_it)
	{
		l_it->second->Deactivate();
	}
#endif

	ShaderProgramBase::End();
}
#if CASTOR_USE_OPENGL4
GlUniformBufferObjectPtr GlShaderProgram :: GetUniformBuffer( String const & p_strName)
{
	GlUniformBufferObjectPtr l_pReturn;

	if (m_mapUniformBuffers.find( p_strName) != m_mapUniformBuffers.end())
	{
		l_pReturn = m_mapUniformBuffers.find( p_strName)->second;
	}

	return l_pReturn;
}

void GlShaderProgram :: SetLightValues( int p_iIndex, GlLightRenderer * p_pLightRenderer)
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
#endif

int GlShaderProgram :: GetAttributeLocation( String const & p_strName)const
{
	int l_iReturn = GL_INVALID_INDEX;

	if (m_programObject != GL_INVALID_INDEX && glIsProgram( m_programObject))
	{
		l_iReturn = OpenGl::GetAttribLocation( m_programObject, p_strName.char_str());
	}

	return l_iReturn;
}

ShaderObjectPtr GlShaderProgram :: _createObject( eSHADER_TYPE p_eType)
{
	ShaderObjectPtr l_pReturn( new GlShaderObject( this, p_eType));
	return l_pReturn;
}

shared_ptr<OneIntFrameVariable> GlShaderProgram :: _create1IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneIntFrameVariable> l_pReturn( new GlOneFrameVariable<int>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2iFrameVariable> GlShaderProgram :: _create2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2iFrameVariable> l_pReturn( new GlPointFrameVariable<int, 2>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3iFrameVariable> GlShaderProgram :: _create3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3iFrameVariable> l_pReturn( new GlPointFrameVariable<int, 3>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4iFrameVariable> GlShaderProgram :: _create4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4iFrameVariable> l_pReturn( new GlPointFrameVariable<int, 4>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<OneUIntFrameVariable> GlShaderProgram :: _create1UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneUIntFrameVariable> l_pReturn( new GlOneFrameVariable<unsigned int>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2uiFrameVariable> GlShaderProgram :: _create2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2uiFrameVariable> l_pReturn( new GlPointFrameVariable<unsigned int, 2>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3uiFrameVariable> GlShaderProgram :: _create3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3uiFrameVariable> l_pReturn( new GlPointFrameVariable<unsigned int, 3>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4uiFrameVariable> GlShaderProgram :: _create4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4uiFrameVariable> l_pReturn( new GlPointFrameVariable<unsigned int, 4>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<OneFloatFrameVariable> GlShaderProgram :: _create1FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneFloatFrameVariable> l_pReturn( new GlOneFrameVariable<float>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2fFrameVariable> GlShaderProgram :: _create2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2fFrameVariable> l_pReturn( new GlPointFrameVariable<float, 2>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3fFrameVariable> GlShaderProgram :: _create3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3fFrameVariable> l_pReturn( new GlPointFrameVariable<float, 3>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4fFrameVariable> GlShaderProgram :: _create4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4fFrameVariable> l_pReturn( new GlPointFrameVariable<float, 4>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<OneDoubleFrameVariable> GlShaderProgram :: _create1DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<OneDoubleFrameVariable>();
}

shared_ptr<Point2dFrameVariable> GlShaderProgram :: _create2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point2dFrameVariable>();
}

shared_ptr<Point3dFrameVariable> GlShaderProgram :: _create3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point3dFrameVariable>();
}

shared_ptr<Point4dFrameVariable> GlShaderProgram :: _create4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point4dFrameVariable>();
}

shared_ptr<Matrix2x2iFrameVariable> GlShaderProgram :: _create2x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x2iFrameVariable>();
}

shared_ptr<Matrix2x3iFrameVariable> GlShaderProgram :: _create2x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x3iFrameVariable>();
}

shared_ptr<Matrix2x4iFrameVariable> GlShaderProgram :: _create2x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x4iFrameVariable>();
}

shared_ptr<Matrix3x2iFrameVariable> GlShaderProgram :: _create3x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x2iFrameVariable>();
}

shared_ptr<Matrix3x3iFrameVariable> GlShaderProgram :: _create3x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x3iFrameVariable>();
}

shared_ptr<Matrix3x4iFrameVariable> GlShaderProgram :: _create3x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x4iFrameVariable>();
}

shared_ptr<Matrix4x2iFrameVariable> GlShaderProgram :: _create4x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x2iFrameVariable>();
}

shared_ptr<Matrix4x3iFrameVariable> GlShaderProgram :: _create4x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x3iFrameVariable>();
}

shared_ptr<Matrix4x4iFrameVariable> GlShaderProgram :: _create4x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x4iFrameVariable>();
}

shared_ptr<Matrix2x2uiFrameVariable> GlShaderProgram :: _create2x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x2uiFrameVariable>();
}

shared_ptr<Matrix2x3uiFrameVariable> GlShaderProgram :: _create2x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x3uiFrameVariable>();
}

shared_ptr<Matrix2x4uiFrameVariable> GlShaderProgram :: _create2x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x4uiFrameVariable>();
}

shared_ptr<Matrix3x2uiFrameVariable> GlShaderProgram :: _create3x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x2uiFrameVariable>();
}

shared_ptr<Matrix3x3uiFrameVariable> GlShaderProgram :: _create3x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x3uiFrameVariable>();
}

shared_ptr<Matrix3x4uiFrameVariable> GlShaderProgram :: _create3x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x4uiFrameVariable>();
}

shared_ptr<Matrix4x2uiFrameVariable> GlShaderProgram :: _create4x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x2uiFrameVariable>();
}

shared_ptr<Matrix4x3uiFrameVariable> GlShaderProgram :: _create4x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x3uiFrameVariable>();
}

shared_ptr<Matrix4x4uiFrameVariable> GlShaderProgram :: _create4x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x4uiFrameVariable>();
}

shared_ptr<Matrix2x2fFrameVariable> GlShaderProgram :: _create2x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x2fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 2, 2>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x3fFrameVariable> GlShaderProgram :: _create2x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x3fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 2, 3>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x4fFrameVariable> GlShaderProgram :: _create2x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x4fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 2, 4>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x2fFrameVariable> GlShaderProgram :: _create3x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x2fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 3, 2>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x3fFrameVariable> GlShaderProgram :: _create3x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x3fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 3, 3>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x4fFrameVariable> GlShaderProgram :: _create3x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x4fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 3, 4>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x2fFrameVariable> GlShaderProgram :: _create4x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x2fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 4, 2>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x3fFrameVariable> GlShaderProgram :: _create4x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x3fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 4, 3>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x4fFrameVariable> GlShaderProgram :: _create4x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x4fFrameVariable> l_pReturn( new GlMatrixFrameVariable<float, 4, 4>( this, static_cast<GlShaderObject *>( p_pObject.get()), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x2dFrameVariable> GlShaderProgram :: _create2x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x2dFrameVariable>();
}

shared_ptr<Matrix2x3dFrameVariable> GlShaderProgram :: _create2x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x3dFrameVariable>();
}

shared_ptr<Matrix2x4dFrameVariable> GlShaderProgram :: _create2x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x4dFrameVariable>();
}

shared_ptr<Matrix3x2dFrameVariable> GlShaderProgram :: _create3x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x2dFrameVariable>();
}

shared_ptr<Matrix3x3dFrameVariable> GlShaderProgram :: _create3x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x3dFrameVariable>();
}

shared_ptr<Matrix3x4dFrameVariable> GlShaderProgram :: _create3x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x4dFrameVariable>();
}

shared_ptr<Matrix4x2dFrameVariable> GlShaderProgram :: _create4x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x2dFrameVariable>();
}

shared_ptr<Matrix4x3dFrameVariable> GlShaderProgram :: _create4x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x3dFrameVariable>();
}

shared_ptr<Matrix4x4dFrameVariable> GlShaderProgram :: _create4x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x4dFrameVariable>();
}

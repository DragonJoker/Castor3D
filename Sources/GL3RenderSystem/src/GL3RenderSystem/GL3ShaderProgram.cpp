#include "PrecompiledHeader.h"

#include "GL3ShaderProgram.h"
#include "GL3RenderSystem.h"
#include "GL3Buffer.h"
#include "GL3LightRenderer.h"

using namespace Castor3D;

GL3ShaderProgram :: GL3ShaderProgram()
	:	GLShaderProgram()
{
}

GL3ShaderProgram :: GL3ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	GLShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile)
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

GL3ShaderProgram :: ~GL3ShaderProgram()
{
}

bool GL3ShaderProgram :: Link()
{
	if (GLShaderProgram::Link())
	{
		SharedPtr< OneFrameVariable<float> > l_fUniform;
		SharedPtr< PointFrameVariable<float, 4> > l_pt4Uniform;
		SharedPtr< PointFrameVariable<float, 3> > l_pt3Uniform;
		SharedPtr< MatrixFrameVariable<float, 4, 4> > l_mtxUniform;

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_AmbientLight"));
		AddFrameVariable( l_pt4Uniform);
		m_pAmbientLight = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_AmbientLight")));

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 8));
		l_pt4Uniform->SetName( CU_T( "in_LightsAmbient"));
		AddFrameVariable( l_pt4Uniform);
		m_pAmbients = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_LightsAmbient")));

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 8));
		l_pt4Uniform->SetName( CU_T( "in_LightsDiffuse"));
		AddFrameVariable( l_pt4Uniform);
		m_pDiffuses = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_LightsDiffuse")));

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 8));
		l_pt4Uniform->SetName( CU_T( "in_LightsSpecular"));
		AddFrameVariable( l_pt4Uniform);
		m_pSpeculars = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_LightsSpecular")));

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 8));
		l_pt4Uniform->SetName( CU_T( "in_LightsPosition"));
		AddFrameVariable( l_pt4Uniform);
		m_pPositions = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_LightsPosition")));

		l_mtxUniform = SharedPtr< MatrixFrameVariable<float, 4, 4> >( new MatrixFrameVariable<float, 4, 4>( 8));
		l_mtxUniform->SetName( CU_T( "in_LightsOrientation"));
		AddFrameVariable( l_mtxUniform);
		m_pOrientations = static_pointer_cast< GLMatrixFrameVariable<float, 4, 4> >( GetGLFrameVariable( CU_T( "in_LightsOrientation")));

		l_pt3Uniform = SharedPtr< PointFrameVariable<float, 3> >( new PointFrameVariable<float, 3>( 8));
		l_pt3Uniform->SetName( CU_T( "in_LightsAttenuation"));
		AddFrameVariable( l_pt3Uniform);
		m_pAttenuations = static_pointer_cast< GLPointFrameVariable<float, 3> >( GetGLFrameVariable( CU_T( "in_LightsAttenuation")));

		l_fUniform = SharedPtr< OneFrameVariable<float> >( new OneFrameVariable<float>( 8));
		l_fUniform->SetName( CU_T( "in_LightsExponent"));
		AddFrameVariable( l_fUniform);
		m_pExponents = static_pointer_cast< GLOneFrameVariable<float> >( GetGLFrameVariable( CU_T( "in_LightsExponent")));

		l_fUniform = SharedPtr< OneFrameVariable<float> >( new OneFrameVariable<float>( 8));
		l_fUniform->SetName( CU_T( "in_LightsCutOff"));
		AddFrameVariable( l_fUniform);
		m_pCutOffs = static_pointer_cast< GLOneFrameVariable<float> >( GetGLFrameVariable( CU_T( "in_LightsCutOff")));

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_MatAmbient"));
		AddFrameVariable( l_pt4Uniform);
		m_pAmbient = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_MatAmbient")));

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_MatDiffuse"));
		AddFrameVariable( l_pt4Uniform);
		m_pDiffuse = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_MatDiffuse")));

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_MatEmissive"));
		AddFrameVariable( l_pt4Uniform);
		m_pEmissive = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_MatEmissive")));

		l_pt4Uniform = SharedPtr< PointFrameVariable<float, 4> >( new PointFrameVariable<float, 4>( 1));
		l_pt4Uniform->SetName( CU_T( "in_MatSpecular"));
		AddFrameVariable( l_pt4Uniform);
		m_pSpecular = static_pointer_cast< GLPointFrameVariable<float, 4> >( GetGLFrameVariable( CU_T( "in_MatSpecular")));

		l_fUniform = SharedPtr< OneFrameVariable<float> >( new OneFrameVariable<float>( 1));
		l_fUniform->SetName( CU_T( "in_MatShininess"));
		AddFrameVariable( l_fUniform);
		m_pShininess = static_pointer_cast< GLOneFrameVariable<float> >( GetGLFrameVariable( CU_T( "in_MatShininess")));
	}

	return m_isLinked;
}

void GL3ShaderProgram :: Begin()
{
	GL3RenderSystem::GetSingletonPtr()->SetCurrentShaderProgram( this);
	GLShaderProgram::Begin();
}

int GL3ShaderProgram :: AssignLight()
{
	int l_iReturn = GL_INVALID_INDEX;

	if (m_setFreeLights.size() > 0)
	{
		l_iReturn = (* m_setFreeLights.begin());
		m_setFreeLights.erase( m_setFreeLights.begin());

	}

	return l_iReturn;
}

void GL3ShaderProgram :: FreeLight( int p_iIndex)
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

void GL3ShaderProgram :: SetAmbientLight( const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pAmbientLight->SetValue( p_crColour, 0);
	}
}

void GL3ShaderProgram :: SetLightAmbient( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pAmbients->SetValue( p_crColour, p_iIndex);
	}
}

void GL3ShaderProgram :: SetLightDiffuse( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pDiffuses->SetValue( p_crColour, p_iIndex);
	}
}

void GL3ShaderProgram :: SetLightSpecular( int p_iIndex, const Point4f & p_crColour)
{
	if (m_isLinked)
	{
		m_pSpeculars->SetValue( p_crColour, p_iIndex);
	}
}

void GL3ShaderProgram :: SetLightPosition( int p_iIndex, const Point4f & p_ptPosition)
{
	if (m_isLinked)
	{
		m_pPositions->SetValue( p_ptPosition, p_iIndex);
	}
}

void GL3ShaderProgram :: SetLightOrientation( int p_iIndex, const Matrix4x4r & p_mtxOrientation)
{
	if (m_isLinked)
	{
		m_pOrientations->SetValue( Matrix<float, 4, 4>( p_mtxOrientation), p_iIndex);
	}
}

void GL3ShaderProgram :: SetLightAttenuation( int p_iIndex, const Point3f & p_ptAtt)
{
	if (m_isLinked)
	{
		m_pAttenuations->SetValue( p_ptAtt, p_iIndex);
	}
}

void GL3ShaderProgram :: SetLightExponent( int p_iIndex, float p_fExp)
{
	if (m_isLinked)
	{
		m_pExponents->SetValue( p_fExp, p_iIndex);
	}
}

void GL3ShaderProgram :: SetLightCutOff( int p_iIndex, float p_fCut)
{
	if (m_isLinked)
	{
		m_pCutOffs->SetValue( p_fCut, p_iIndex);
	}
}

void GL3ShaderProgram :: SetMaterialAmbient( const Point4f & p_crColour)
{
	if (m_isLinked && m_pAmbient->GetValue() != p_crColour)
	{
		m_pAmbient->SetValue( p_crColour);
	}
}

void GL3ShaderProgram :: SetMaterialDiffuse( const Point4f & p_crColour)
{
	if (m_isLinked && m_pDiffuse->GetValue() != p_crColour)
	{
		m_pDiffuse->SetValue( p_crColour);
	}
}

void GL3ShaderProgram :: SetMaterialSpecular( const Point4f & p_crColour)
{
	if (m_isLinked && m_pSpecular->GetValue() != p_crColour)
	{
		m_pSpecular->SetValue( p_crColour);
	}
}

void GL3ShaderProgram :: SetMaterialEmissive( const Point4f & p_crColour)
{
	if (m_isLinked && m_pEmissive->GetValue() != p_crColour)
	{
		m_pEmissive->SetValue( p_crColour);
	}
}

void GL3ShaderProgram :: SetMaterialShininess( float p_fShine)
{
	if (m_isLinked && m_pShininess->GetValue() != p_fShine)
	{
		m_pShininess->SetValue( p_fShine);
	}
}
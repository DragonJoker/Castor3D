#include "Castor3D/PrecompiledHeader.h"
#include "Castor3D/light/Light.h"

#include "Castor3D/scene/SceneNode.h"

using namespace Castor3D;

Light :: Light( SceneNodePtr p_pNode, const String & p_name)
	:	MovableObject( (SceneNode *)p_pNode.get(), p_name),
		m_enabled( false),
		m_ambient( 1, 1, 1, 1),
		m_diffuse( 0, 0, 0, 1),
		m_specular( 1, 1, 1, 1),
		m_ptPositionType( m_pSceneNode->GetPosition()[0], m_pSceneNode->GetPosition()[1], m_pSceneNode->GetPosition()[2], 0)
{
	m_eType = eLight;
	m_pSceneNode->GetPosition().LinkCoords( m_ptPositionType.ptr());
}

Light :: ~Light()
{
}

void Light :: Enable()
{
	m_pRenderer->Enable();
}

void Light :: Disable()
{
	m_pRenderer->Disable();
}

void Light :: Render( eDRAW_TYPE p_displayMode)
{
	_apply();
}

void Light :: EndRender()
{
	Disable();
}

void Light :: SetAmbient( float * p_ambient)
{
	m_ambient[0] = p_ambient[0];
	m_ambient[1] = p_ambient[1];
	m_ambient[2] = p_ambient[2];
}

void Light :: SetAmbient( float r, float g, float b)
{
	m_ambient[0] = r;
	m_ambient[1] = g;
	m_ambient[2] = b;
}

void Light :: SetAmbient( const Colour & p_ambient)
{
	m_ambient[0] = p_ambient[0];
	m_ambient[1] = p_ambient[1];
	m_ambient[2] = p_ambient[2];
}

void Light :: SetDiffuse( float * p_diffuse)
{
	m_diffuse[0] = p_diffuse[0];
	m_diffuse[1] = p_diffuse[1];
	m_diffuse[2] = p_diffuse[2];
}

void Light :: SetDiffuse( float r, float g, float b)
{
	m_diffuse[0] = r;
	m_diffuse[1] = g;
	m_diffuse[2] = b;
}

void Light :: SetDiffuse( const Colour & p_diffuse)
{
	m_diffuse[0] = p_diffuse[0];
	m_diffuse[1] = p_diffuse[1];
	m_diffuse[2] = p_diffuse[2];
}

void Light :: SetSpecular( float * p_specular)
{
	m_specular[0] = p_specular[0];
	m_specular[1] = p_specular[1];
	m_specular[2] = p_specular[2];
}

void Light :: SetSpecular( float r, float g, float b)
{
	m_specular[0] = r;
	m_specular[1] = g;
	m_specular[2] = b;
}

void Light :: SetSpecular( const Colour & p_specular)
{
	m_specular[0] = p_specular[0];
	m_specular[1] = p_specular[1];
	m_specular[2] = p_specular[2];
}

bool Light :: Write( File & p_pFile)const
{
	bool l_bReturn = p_pFile.Print( 256, "\tdiffuse %f %f %f %f\n", m_diffuse[0], m_diffuse[1], m_diffuse[2], m_diffuse[3]);

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\tspecular %f %f %f %f\n", m_specular[0], m_specular[1], m_specular[2], m_specular[3]);
	}

	return l_bReturn;
}

void Light :: AttachTo( SceneNode * p_pNode)
{
	if (m_pSceneNode != NULL)
	{
		m_pSceneNode->GetPosition().UnlinkCoords();
		m_pSceneNode->GetPosition()[0] = m_ptPositionType[0];
		m_pSceneNode->GetPosition()[1] = m_ptPositionType[1];
		m_pSceneNode->GetPosition()[2] = m_ptPositionType[2];
	}

	MovableObject::AttachTo( p_pNode);

	if (m_pSceneNode != NULL)
	{
		m_ptPositionType[0] = m_pSceneNode->GetPosition()[0];
		m_ptPositionType[1] = m_pSceneNode->GetPosition()[1];
		m_ptPositionType[2] = m_pSceneNode->GetPosition()[2];
		m_pSceneNode->GetPosition().LinkCoords( m_ptPositionType.ptr());
	}
	else
	{
		m_ptPositionType[0] = 0;
		m_ptPositionType[1] = 0;
		m_ptPositionType[2] = 0;
	}
}

void Light :: _initialise()
{
	m_pRenderer->ApplyAmbient();
	m_pRenderer->ApplyDiffuse();
	m_pRenderer->ApplySpecular();
}

void Light :: _apply()
{
	m_pRenderer->Enable();
	m_pRenderer->ApplyPosition();
	_initialise();
}

void Light :: _remove()
{
	m_pRenderer->Disable();
}
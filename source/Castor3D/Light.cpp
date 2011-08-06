#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Light.hpp"
#include "Castor3D/DirectionalLight.hpp"
#include "Castor3D/PointLight.hpp"
#include "Castor3D/SpotLight.hpp"
#include "Castor3D/SceneNode.hpp"

using namespace Castor3D;

//*************************************************************************************************

Factory<LightCategory, eLIGHT_TYPE>::obj_map Factory<LightCategory, eLIGHT_TYPE>::m_mapRegistered;

//*************************************************************************************************

bool Loader<LightCategory> :: Write( const LightCategory & p_light, File & p_file)
{
	Logger::LogMessage( cuT( "Writing Light ") + p_light.GetLight()->GetName());

	bool l_bReturn = p_file.WriteLine( "light " + p_light.GetLight()->GetName() + "\n{\n") > 0;

	if (l_bReturn)
	{
		switch (p_light.GetLightType())
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			l_bReturn = p_file.WriteLine( "\ttype directional\n") > 0;
			break;

		case eLIGHT_TYPE_POINT:
			l_bReturn = p_file.WriteLine( "\ttype point_light\n") > 0;
			break;

		case eLIGHT_TYPE_SPOT:
			l_bReturn = p_file.WriteLine( "\ttype spot_light\n") > 0;
			break;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\tambient ") > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Write( p_light.GetAmbient(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\n\tdiffuse ") > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Write( p_light.GetDiffuse(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\n\tspecular ") > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Write( p_light.GetSpecular(), p_file);
	}

	return l_bReturn;
}

//*************************************************************************************************

LightCategory :: LightCategory( eLIGHT_TYPE p_eLightType)
	:	m_eLightType( p_eLightType)
	,	m_ambient( Colour::FromRGBA( 0xFFFFFFFF))
	,	m_diffuse( Colour::FromRGBA( 0x000000FF))
	,	m_specular( Colour::FromRGBA( 0xFFFFFFFF))
{
}

LightCategory :: ~LightCategory()
{
}

void LightCategory :: SetAmbient( float * p_ambient)
{
	m_ambient[0] = p_ambient[0];
	m_ambient[1] = p_ambient[1];
	m_ambient[2] = p_ambient[2];
}

void LightCategory :: SetAmbient( float r, float g, float b)
{
	m_ambient[0] = r;
	m_ambient[1] = g;
	m_ambient[2] = b;
}

void LightCategory :: SetAmbient( Colour const & p_ambient)
{
	m_ambient[0] = p_ambient[0];
	m_ambient[1] = p_ambient[1];
	m_ambient[2] = p_ambient[2];
}

void LightCategory :: SetDiffuse( float * p_diffuse)
{
	m_diffuse[0] = p_diffuse[0];
	m_diffuse[1] = p_diffuse[1];
	m_diffuse[2] = p_diffuse[2];
}

void LightCategory :: SetDiffuse( float r, float g, float b)
{
	m_diffuse[0] = r;
	m_diffuse[1] = g;
	m_diffuse[2] = b;
}

void LightCategory :: SetDiffuse( Colour const & p_diffuse)
{
	m_diffuse[0] = p_diffuse[0];
	m_diffuse[1] = p_diffuse[1];
	m_diffuse[2] = p_diffuse[2];
}

void LightCategory :: SetSpecular( float * p_specular)
{
	m_specular[0] = p_specular[0];
	m_specular[1] = p_specular[1];
	m_specular[2] = p_specular[2];
}

void LightCategory :: SetSpecular( float r, float g, float b)
{
	m_specular[0] = r;
	m_specular[1] = g;
	m_specular[2] = b;
}

void LightCategory :: SetSpecular( Colour const & p_specular)
{
	m_specular[0] = p_specular[0];
	m_specular[1] = p_specular[1];
	m_specular[2] = p_specular[2];
}

BEGIN_SERIALISE_MAP( LightCategory, Serialisable)
	ADD_ELEMENT( m_ambient)
	ADD_ELEMENT( m_diffuse)
	ADD_ELEMENT( m_specular)
END_SERIALISE_MAP()

//*************************************************************************************************

Light :: Light( eLIGHT_TYPE p_eLightType)
	:	MovableObject( eMOVABLE_TYPE_LIGHT)
	,	m_enabled( false)
{
	Factory<LightCategory, eLIGHT_TYPE> l_factory;
	m_pCategory = l_factory.Create( p_eLightType);
	m_pCategory->SetLight( this);
}

Light :: Light( Scene * p_pScene, SceneNodePtr p_pNode, String const & p_name, eLIGHT_TYPE p_eLightType)
	:	MovableObject( p_pScene, p_pNode.get(), p_name, eMOVABLE_TYPE_LIGHT)
	,	m_enabled( false)
{
	Factory<LightCategory, eLIGHT_TYPE> l_factory;
	m_pCategory = l_factory.Create( p_eLightType);
	m_pCategory->SetLight( this);

	m_pCategory->SetPositionType( Point4f( m_pSceneNode->GetPosition()[0], m_pSceneNode->GetPosition()[1], m_pSceneNode->GetPosition()[2], 0.0f));
	m_pSceneNode->GetPosition().link( m_pCategory->GetPositionType().ptr());
}

Light :: ~Light()
{
}

void Light :: Register()
{
	Factory<LightCategory, eLIGHT_TYPE> l_factory;
	l_factory.Register( eLIGHT_TYPE_DIRECTIONAL,	DirectionalLightPtr( new DirectionalLight));
	l_factory.Register( eLIGHT_TYPE_POINT,			PointLightPtr( new PointLight));
	l_factory.Register( eLIGHT_TYPE_SPOT,			SpotLightPtr( new SpotLight));
}

void Light :: Enable()
{
	CASTOR_TRACK;
	m_pRenderer->Enable();
}

void Light :: Disable()
{
	CASTOR_TRACK;
	m_pRenderer->Disable();
}

void Light :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	m_pCategory->Render( m_pRenderer, p_displayMode);
}

void Light :: EndRender()
{
	CASTOR_TRACK;
	Disable();
}

void Light :: AttachTo( SceneNode * p_pNode)
{
	if (m_pSceneNode)
	{
		m_pSceneNode->GetPosition().unlink();
		m_pSceneNode->GetPosition()[0] = GetPositionType()[0];
		m_pSceneNode->GetPosition()[1] = GetPositionType()[1];
		m_pSceneNode->GetPosition()[2] = GetPositionType()[2];
	}

	MovableObject::AttachTo( p_pNode);

	if (m_pSceneNode)
	{
		GetPositionType()[0] = m_pSceneNode->GetPosition()[0];
		GetPositionType()[1] = m_pSceneNode->GetPosition()[1];
		GetPositionType()[2] = m_pSceneNode->GetPosition()[2];
		m_pSceneNode->GetPosition().link( GetPositionType().ptr());
	}
	else
	{
		GetPositionType()[0] = 0;
		GetPositionType()[1] = 0;
		GetPositionType()[2] = 0;
	}
}

BEGIN_SERIALISE_MAP( Light, MovableObject)
END_SERIALISE_MAP()

BEGIN_PRE_SERIALISE( Light, MovableObject)
	m_pFile->Write( GetLightType());
END_PRE_SERIALISE()

BEGIN_POST_UNSERIALISE( Light, MovableObject)
	m_pSceneNode->GetPosition().link( GetPositionType().ptr());
END_POST_UNSERIALISE()

//*************************************************************************************************
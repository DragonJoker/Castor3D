#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Light.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/DirectionalLight.hpp"
#include "Castor3D/PointLight.hpp"
#include "Castor3D/SpotLight.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Pipeline.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

LightCategory::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< LightCategory, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool LightCategory::TextLoader :: operator ()( LightCategory const & p_light, TextFile & p_file)
{
	Logger::LogMessage( cuT( "Writing Light ") + p_light.GetLight()->GetName());

	bool l_bReturn = p_file.WriteText( cuT( "light " ) + p_light.GetLight()->GetName() + cuT( "\n{\n" ) ) > 0;

	if (l_bReturn)
	{
		switch (p_light.GetLightType())
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			l_bReturn = p_file.WriteText( cuT( "\ttype directional\n" ) ) > 0;
			break;

		case eLIGHT_TYPE_POINT:
			l_bReturn = p_file.WriteText( cuT( "\ttype point_light\n" ) ) > 0;
			break;

		case eLIGHT_TYPE_SPOT:
			l_bReturn = p_file.WriteText( cuT( "\ttype spot_light\n" ) ) > 0;
			break;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\tambient " ) ) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Colour::TextLoader()( p_light.GetAmbient(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\n\tdiffuse " ) ) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Colour::TextLoader()( p_light.GetDiffuse(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\n\tspecular " ) ) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Colour::TextLoader()( p_light.GetSpecular(), p_file);
	}

	return l_bReturn;
}

//*************************************************************************************************

LightCategory :: LightCategory( eLIGHT_TYPE p_eLightType )
	:	m_eLightType		( p_eLightType						)
	,	m_ambient			( Colour::from_rgba( 0x000000FF )	)
	,	m_diffuse			( Colour::from_rgba( 0x000000FF )	)
	,	m_specular			( Colour::from_rgba( 0xFFFFFFFF )	)
	,	m_ptPositionType	( 0.0f, 0.0f, 1.0f, 0.0f			)
{
	switch( p_eLightType )
	{
	case eLIGHT_TYPE_DIRECTIONAL:
		m_ptPositionType[3] = 0.0f;

	default:
		m_ptPositionType[3] = 1.0f;
	}
}

LightCategory :: ~LightCategory()
{
}

void LightCategory :: SetAmbient( float * p_ambient)
{
	m_ambient.red()		= p_ambient[0];
	m_ambient.green()	= p_ambient[1];
	m_ambient.blue()	= p_ambient[2];
}

void LightCategory :: SetAmbient( float r, float g, float b)
{
	m_ambient.red()		= r;
	m_ambient.green()	= g;
	m_ambient.blue()	= b;
}

void LightCategory :: SetAmbient( Colour const & p_ambient)
{
	m_ambient.red()		= p_ambient.red()	;
	m_ambient.green()	= p_ambient.green()	;
	m_ambient.blue()	= p_ambient.blue()	;
}

void LightCategory :: SetDiffuse( float * p_diffuse)
{
	m_diffuse.red()		= p_diffuse[0];
	m_diffuse.green()	= p_diffuse[1];
	m_diffuse.blue()	= p_diffuse[2];
}

void LightCategory :: SetDiffuse( float r, float g, float b)
{
	m_diffuse.red()		= r;
	m_diffuse.green()	= g;
	m_diffuse.blue()	= b;
}

void LightCategory :: SetDiffuse( Colour const & p_diffuse)
{
	m_diffuse.red()		= p_diffuse.red()	;
	m_diffuse.green()	= p_diffuse.green()	;
	m_diffuse.blue()	= p_diffuse.blue()	;
}

void LightCategory :: SetSpecular( float * p_specular)
{
	m_specular.red()	= p_specular[0];
	m_specular.green()	= p_specular[1];
	m_specular.blue()	= p_specular[2];
}

void LightCategory :: SetSpecular( float r, float g, float b)
{
	m_specular.red()	= r;
	m_specular.green()	= g;
	m_specular.blue()	= b;
}

void LightCategory :: SetSpecular( Colour const & p_specular)
{
	m_specular.red()	= p_specular.red()		;
	m_specular.green()	= p_specular.green()	;
	m_specular.blue()	= p_specular.blue()		;
}

//*************************************************************************************************

Light :: Light( Scene * p_pScene, LightFactory & p_factory, eLIGHT_TYPE p_eLightType )
	:	MovableObject						( p_pScene, eMOVABLE_TYPE_LIGHT	)
	,	Renderable< Light, LightRenderer >	( p_pScene->GetEngine()			)
	,	m_enabled							( false							)
{
	m_pCategory = p_factory.Create( p_eLightType );
	m_pCategory->SetLight( this );
	GetRenderer()->Initialise();
}

Light :: Light( LightFactory & p_factory, Scene * p_pScene, SceneNodeSPtr p_pNode, String const & p_name, eLIGHT_TYPE p_eLightType)
	:	MovableObject						( p_pScene, p_pNode.get(), p_name, eMOVABLE_TYPE_LIGHT	)
	,	Renderable< Light, LightRenderer >	( p_pScene->GetEngine()									)
	,	m_enabled							( false													)
{
	m_pCategory = p_factory.Create( p_eLightType );
	m_pCategory->SetLight( this );
	m_pCategory->SetPositionType( Point4f( m_pSceneNode->GetPosition()[0], m_pSceneNode->GetPosition()[1], m_pSceneNode->GetPosition()[2], real( 0.0 ) ) );
	GetRenderer()->Initialise();
}

Light :: ~Light()
{
}

void Light :: Register( LightFactory & p_factory )
{
	p_factory.Register( eLIGHT_TYPE_DIRECTIONAL,	DirectionalLight::Create	);
	p_factory.Register( eLIGHT_TYPE_POINT,			PointLight::Create			);
	p_factory.Register( eLIGHT_TYPE_SPOT,			SpotLight::Create			);
}

void Light :: Enable()
{
	if( ! m_pRenderer.expired() )
	{
		m_pRenderer.lock()->Enable();
	}
}

void Light :: Disable()
{
	if( ! m_pRenderer.expired() )
	{
		m_pRenderer.lock()->Disable();
	}
}

void Light :: Enable( ShaderProgramBase * p_pProgram )
{
	if( ! m_pRenderer.expired() )
	{
		m_pRenderer.lock()->EnableShader( p_pProgram );
	}
}

void Light :: Disable( ShaderProgramBase * p_pProgram )
{
	if( ! m_pRenderer.expired() )
	{
		m_pRenderer.lock()->DisableShader( p_pProgram );
	}
}

void Light :: Render()
{
	if( ! m_pRenderer.expired() )
	{
		if( m_pSceneNode )
		{
			m_pCategory->SetPosition( Point3f( m_pSceneNode->GetPosition()[0], m_pSceneNode->GetPosition()[1], m_pSceneNode->GetPosition()[2] ) );
		}

		m_pCategory->Render( m_pRenderer.lock() );
	}
}

void Light :: EndRender()
{
	Disable();
}

void Light :: Render( ShaderProgramBase * p_pProgram )
{
	if( ! m_pRenderer.expired() )
	{
		if( m_pSceneNode )
		{
			Point3r l_position = m_pSceneNode->GetDerivedPosition();
//			l_position = GetEngine()->GetRenderSystem()->GetPipeline()->GetMatrix( eMTXMODE_VIEW ) * l_position;
			m_pCategory->SetPosition( l_position );
		}

		m_pCategory->Render( m_pRenderer.lock(), p_pProgram );
	}
}

void Light :: EndRender( ShaderProgramBase * p_pProgram )
{
	Disable( p_pProgram );
}

void Light :: AttachTo( SceneNode * p_pNode)
{
	Point4f l_ptPosType = GetPositionType();

	if( m_pSceneNode )
	{
		m_pSceneNode->GetPosition()[0] = l_ptPosType[0];
		m_pSceneNode->GetPosition()[1] = l_ptPosType[1];
		m_pSceneNode->GetPosition()[2] = l_ptPosType[2];
	}

	MovableObject::AttachTo( p_pNode);

	if( m_pSceneNode )
	{
		l_ptPosType[0] = float( m_pSceneNode->GetPosition()[0] );
		l_ptPosType[1] = float( m_pSceneNode->GetPosition()[1] );
		l_ptPosType[2] = float( m_pSceneNode->GetPosition()[2] );
	}
	else
	{
		l_ptPosType[0] = 0;
		l_ptPosType[1] = 0;
		l_ptPosType[2] = 0;
	}

	m_pCategory->SetPositionType( l_ptPosType );
}

//*************************************************************************************************

LightRenderer :: LightRenderer( RenderSystem * p_pRenderSystem )
	:	Renderer<Light, LightRenderer>( p_pRenderSystem )
{
}

LightRenderer :: ~LightRenderer()
{
	Cleanup();
}

void LightRenderer :: Initialise()
{
}

void LightRenderer :: Cleanup()
{
}

//*************************************************************************************************

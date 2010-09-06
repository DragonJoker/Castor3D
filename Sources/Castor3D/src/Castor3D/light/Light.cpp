#include "PrecompiledHeader.h"

#include "light/Module_Light.h"

#include "light/Light.h"
#include "scene/Scene.h"

#include "render_system/RenderSystem.h"
#include "render_system/LightRenderer.h"
#include "main/Root.h"

#include "Log.h"

using namespace Castor3D;

Light :: Light( LightRenderer * p_renderer, const String & p_name)
	:	m_name( p_name),
		m_enabled( false),
		m_renderer( p_renderer),
		m_position( 0, 0, 1, 1),
		m_ambient( 1, 1, 1, 1),
		m_diffuse( 0, 0, 0, 1),
		m_specular( 1, 1, 1, 1)
{
	m_renderer->SetTarget( this);
}

Light :: ~Light()
{
}

void Light :: Enable()
{
	m_renderer->Enable();
}

void Light :: Disable()
{
	m_renderer->Disable();
}

void Light :: Render()
{
	_apply();
}

void Light :: _initialise()
{
	m_renderer->ApplyAmbient( m_ambient.ptr());
	m_renderer->ApplyDiffuse( m_diffuse.ptr());
	m_renderer->ApplySpecular( m_specular.ptr());
}

void Light :: _apply()
{
	m_renderer->Enable();
	m_renderer->ApplyPosition( m_position.ptr());
	_initialise();
}

void Light :: _remove()
{
	m_renderer->Disable();
}

void Light :: SetPosition( float * p_vertex)
{
	m_position.x = p_vertex[0];
	m_position.y = p_vertex[1];
	m_position.z = p_vertex[2];
	m_renderer->ApplyPosition( m_position.ptr());
}

void Light :: SetPosition( float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	m_renderer->ApplyPosition( m_position.ptr());
}

void Light :: SetPosition( const Vector3f & p_vector)
{
	m_position.x = p_vector.x;
	m_position.y = p_vector.y;
	m_position.z = p_vector.z;
	m_renderer->ApplyPosition( m_position.ptr());
}

void Light :: Translate( float * p_vector)
{
	m_position.x += p_vector[0];
	m_position.y += p_vector[1];
	m_position.z += p_vector[2];
	m_renderer->ApplyPosition( m_position.ptr());
}

void Light :: Translate( float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
	m_renderer->ApplyPosition( m_position.ptr());
}

void Light :: Translate( const Vector3f & p_vector)
{
	m_position.x += p_vector.x;
	m_position.y += p_vector.y;
	m_position.z += p_vector.z;
	m_renderer->ApplyPosition( m_position.ptr());
}

void Light :: SetAmbient( float * p_ambient)
{
	m_ambient.x = p_ambient[0];
	m_ambient.y = p_ambient[1];
	m_ambient.z = p_ambient[2];
	m_renderer->ApplyAmbient( m_ambient.ptr());
}

void Light :: SetAmbient( float r, float g, float b)
{
	m_ambient.x = r;
	m_ambient.y = g;
	m_ambient.z = b;
	m_renderer->ApplyAmbient( m_ambient.ptr());
}

void Light :: SetAmbient( const Vector3f & p_ambient)
{
	m_ambient.x = p_ambient.x;
	m_ambient.y = p_ambient.y;
	m_ambient.z = p_ambient.z;
	m_renderer->ApplyAmbient( m_ambient.ptr());
}

void Light :: SetDiffuse( float * p_diffuse)
{
	m_diffuse.x = p_diffuse[0];
	m_diffuse.y = p_diffuse[1];
	m_diffuse.z = p_diffuse[2];
	m_renderer->ApplyDiffuse( m_diffuse.ptr());
}

void Light :: SetDiffuse( float r, float g, float b)
{
	m_diffuse.x = r;
	m_diffuse.y = g;
	m_diffuse.z = b;
	m_renderer->ApplyDiffuse( m_diffuse.ptr());
}

void Light :: SetDiffuse( const Vector3f & p_diffuse)
{
	m_diffuse.x = p_diffuse.x;
	m_diffuse.y = p_diffuse.y;
	m_diffuse.z = p_diffuse.z;
	m_renderer->ApplyDiffuse( m_diffuse.ptr());
}

void Light :: SetSpecular( float * p_specular)
{
	m_specular.x = p_specular[0];
	m_specular.y = p_specular[1];
	m_specular.z = p_specular[2];
	m_renderer->ApplySpecular( m_specular.ptr());
}

void Light :: SetSpecular( float r, float g, float b)
{
	m_specular.x = r;
	m_specular.y = g;
	m_specular.z = b;
	m_renderer->ApplySpecular( m_specular.ptr());
}

void Light :: SetSpecular( const Vector3f & p_specular)
{
	m_specular.x = p_specular.x;
	m_specular.y = p_specular.y;
	m_specular.z = p_specular.z;
	m_renderer->ApplySpecular( m_specular.ptr());
}

bool Light :: Write( FileIO * p_pFile)const
{
	bool l_bReturn = p_pFile->Print( 256, "\tposition %f %f %f\n", m_position.x, m_position.y, m_position.z);

	if (l_bReturn)
	{
		l_bReturn = p_pFile->Print( 256, "\tdiffuse %f %f %f %f\n", m_diffuse.r, m_diffuse.g, m_diffuse.b, m_diffuse.a);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile->Print( 256, "\tspecular %f %f %f %f\n", m_specular.r, m_specular.g, m_specular.b, m_specular.a);
	}

	return l_bReturn;
}

bool Light :: Read( FileIO & p_file, Scene * p_scene)
{
	size_t l_nameLength = 0;
	bool l_bReturn = (p_file.Read<size_t>( l_nameLength) > 0);

	if (l_bReturn)
	{
		Char * l_name = new Char[l_nameLength+1];
		l_bReturn = (p_file.ReadArray<Char>( l_name, l_nameLength) > 0);

		if (l_bReturn)
		{
			l_name[l_nameLength] = 0;
			m_name = l_name;
		}

		delete [] l_name;
	}

	if (l_bReturn)
	{
		Log::LogMessage( C3D_T( "Reading Light ") + m_name);
		l_bReturn = (p_file.Read<bool>( m_enabled) > 0);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray<float>( m_position.ptr(), 4) > 0);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray<float>( m_diffuse.ptr(), 4) > 0);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray<float>( m_specular.ptr(), 4) > 0);
	}

	p_scene->AddLight( this);

	return l_bReturn;
}
#include "PrecompiledHeader.h"
#include "light/Light.h"

using namespace Castor3D;

Light :: Light( const String & p_name)
	:	m_name( p_name),
		m_enabled( false),
		m_position( 0, 0, 1, 1),
		m_ambient( 1, 1, 1, 1),
		m_diffuse( 0, 0, 0, 1),
		m_specular( 1, 1, 1, 1)
{
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

void Light :: Apply( eDRAW_TYPE p_displayMode)
{
	_apply();
}

void Light :: _initialise()
{
	m_pRenderer->ApplyAmbient( m_ambient.ptr());
	m_pRenderer->ApplyDiffuse( m_diffuse.ptr());
	m_pRenderer->ApplySpecular( m_specular.ptr());
}

void Light :: _apply()
{
	m_pRenderer->Enable();
	m_pRenderer->ApplyPosition( m_position.ptr());
	_initialise();
}

void Light :: _remove()
{
	m_pRenderer->Disable();
}

void Light :: SetPosition( float * p_vertex)
{
	m_position[0] = p_vertex[0];
	m_position[1] = p_vertex[1];
	m_position[2] = p_vertex[2];
	m_pRenderer->ApplyPosition( m_position.ptr());
}

void Light :: SetPosition( float x, float y, float z)
{
	m_position[0] = x;
	m_position[1] = y;
	m_position[2] = z;
	m_pRenderer->ApplyPosition( m_position.ptr());
}

void Light :: SetPosition( const Point<float, 3> & p_vector)
{
	m_position[0] = p_vector[0];
	m_position[1] = p_vector[1];
	m_position[2] = p_vector[2];
	m_pRenderer->ApplyPosition( m_position.ptr());
}

void Light :: Translate( float * p_vector)
{
	m_position[0] += p_vector[0];
	m_position[1] += p_vector[1];
	m_position[2] += p_vector[2];
	m_pRenderer->ApplyPosition( m_position.ptr());
}

void Light :: Translate( float x, float y, float z)
{
	m_position[0] += x;
	m_position[1] += y;
	m_position[2] += z;
	m_pRenderer->ApplyPosition( m_position.ptr());
}

void Light :: Translate( const Point<float, 3> & p_vector)
{
	m_position[0] += p_vector[0];
	m_position[1] += p_vector[1];
	m_position[2] += p_vector[2];
	m_pRenderer->ApplyPosition( m_position.ptr());
}

void Light :: SetAmbient( float * p_ambient)
{
	m_ambient[0] = p_ambient[0];
	m_ambient[1] = p_ambient[1];
	m_ambient[2] = p_ambient[2];
	m_pRenderer->ApplyAmbient( m_ambient.ptr());
}

void Light :: SetAmbient( float r, float g, float b)
{
	m_ambient[0] = r;
	m_ambient[1] = g;
	m_ambient[2] = b;
	m_pRenderer->ApplyAmbient( m_ambient.ptr());
}

void Light :: SetAmbient( const Colour & p_ambient)
{
	m_ambient[0] = p_ambient[0];
	m_ambient[1] = p_ambient[1];
	m_ambient[2] = p_ambient[2];
	m_pRenderer->ApplyAmbient( m_ambient.ptr());
}

void Light :: SetDiffuse( float * p_diffuse)
{
	m_diffuse[0] = p_diffuse[0];
	m_diffuse[1] = p_diffuse[1];
	m_diffuse[2] = p_diffuse[2];
	m_pRenderer->ApplyDiffuse( m_diffuse.ptr());
}

void Light :: SetDiffuse( float r, float g, float b)
{
	m_diffuse[0] = r;
	m_diffuse[1] = g;
	m_diffuse[2] = b;
	m_pRenderer->ApplyDiffuse( m_diffuse.ptr());
}

void Light :: SetDiffuse( const Colour & p_diffuse)
{
	m_diffuse[0] = p_diffuse[0];
	m_diffuse[1] = p_diffuse[1];
	m_diffuse[2] = p_diffuse[2];
	m_pRenderer->ApplyDiffuse( m_diffuse.ptr());
}

void Light :: SetSpecular( float * p_specular)
{
	m_specular[0] = p_specular[0];
	m_specular[1] = p_specular[1];
	m_specular[2] = p_specular[2];
	m_pRenderer->ApplySpecular( m_specular.ptr());
}

void Light :: SetSpecular( float r, float g, float b)
{
	m_specular[0] = r;
	m_specular[1] = g;
	m_specular[2] = b;
	m_pRenderer->ApplySpecular( m_specular.ptr());
}

void Light :: SetSpecular( const Colour & p_specular)
{
	m_specular[0] = p_specular[0];
	m_specular[1] = p_specular[1];
	m_specular[2] = p_specular[2];
	m_pRenderer->ApplySpecular( m_specular.ptr());
}

bool Light :: Write( File & p_pFile)const
{
	bool l_bReturn = p_pFile.Print( 256, "\tposition %f %f %f\n", m_position[0], m_position[1], m_position[2]);

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\tdiffuse %f %f %f %f\n", m_diffuse[0], m_diffuse[1], m_diffuse[2], m_diffuse[3]);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.Print( 256, "\tspecular %f %f %f %f\n", m_specular[0], m_specular[1], m_specular[2], m_specular[3]);
	}

	return l_bReturn;
}

bool Light :: Read( File & p_file)
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
		Log::LogMessage( CU_T( "Reading Light ") + m_name);
		l_bReturn = (p_file.Read<bool>( m_enabled) > 0);
	}

	if (l_bReturn)
	{
		l_bReturn = m_position.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_diffuse.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_specular.Read( p_file);
	}

	return l_bReturn;
}
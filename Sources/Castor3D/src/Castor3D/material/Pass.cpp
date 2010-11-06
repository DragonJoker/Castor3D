#include "PrecompiledHeader.h"
#include "material/Pass.h"

#include "material/TextureUnit.h"
#include "shader/ShaderProgram.h"
#include "render_system/RenderSystem.h"
#include "main/Root.h"
#include "material/Material.h"

using namespace Castor3D;

Pass :: Pass( Material * p_parent)
	:	m_shininess( 50.0),
		m_doubleFace( false),
//		m_shaderProgram( NULL),
		m_pParent( p_parent)
{
	m_textureUnits.clear();
	SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f);
	SetEmissive( 0.0f, 0.0f, 0.0f, 1.0f);
	SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
	SetSpecular( 1.0f, 1.0f, 1.0f, 1.0f);
}

Pass :: ~Pass()
{
	m_textureUnits.clear();
//	vector::deleteAll( m_textureUnits);
}

void Pass :: Initialise()
{
	for (size_t i = 0 ; i < m_textureUnits.size() ; i++)
	{
		m_textureUnits[i]->Initialise();
	}

	m_pRenderer->Initialise();
}

void Pass :: Apply( eDRAW_TYPE p_displayMode)
{
	if ( ! m_shaderProgram.expired())
	{
		ShaderProgramPtr l_pShader = m_shaderProgram.lock();
		l_pShader->Initialise();
		l_pShader->Begin();
	}

	m_pRenderer->Apply( p_displayMode);

	for (size_t i = 0 ; i < m_textureUnits.size() ; i++)
	{
		m_textureUnits[i]->Apply( p_displayMode);
	}
}

void Pass :: Apply2D( eDRAW_TYPE p_displayMode)
{
	if ( ! m_shaderProgram.expired())
	{
		ShaderProgramPtr l_pShader = m_shaderProgram.lock();
		l_pShader->Initialise();
		l_pShader->Begin();
	}

	m_pRenderer->Apply2D( p_displayMode);

	for (size_t i = 0 ; i < m_textureUnits.size() ; i++)
	{
		m_textureUnits[i]->Apply( p_displayMode);
	}
}

void Pass :: Remove()
{

	for (size_t i = 0 ; i < m_textureUnits.size() ; i++)
	{
		m_textureUnits[i]->Remove();
	}

	m_pRenderer->Remove();

	if ( ! m_shaderProgram.expired())
	{
		ShaderProgramPtr l_pShader = m_shaderProgram.lock();
		l_pShader->End();
	}
}

void Pass :: SetTexBaseColour( float p_r, float p_g, float p_b, float p_a)
{
	m_texBaseColour = Colour( p_r, p_g, p_b, p_a);

	for (size_t i = 0 ; i < m_textureUnits.size() ; i++)
	{
		m_textureUnits[i]->SetPrimaryColour( m_texBaseColour);
	}
}

void Pass :: SetTexBaseColour( const Colour & p_crColour)
{
	m_texBaseColour = Colour( p_crColour);

	for (size_t i = 0 ; i < m_textureUnits.size() ; i++)
	{
		m_textureUnits[i]->SetPrimaryColour( m_texBaseColour);
	}
}

void Pass :: AddTextureUnit( TextureUnitPtr p_texUnit)
{
	if ( ! p_texUnit.null())
	{
		m_textureUnits.push_back( p_texUnit);
		p_texUnit->SetIndex( static_cast <unsigned int>( m_textureUnits.size() - 1));
		p_texUnit->SetPrimaryColour( m_texBaseColour.ptr());
	}
}

bool Pass :: DestroyTextureUnit( size_t p_index)
{
	bool l_bReturn = false;
	Log::LogMessage( CU_T( "Destroying TextureUnit %d"), p_index);

	if (p_index < m_textureUnits.size())
	{
		TextureUnitPtrArray::iterator l_it = m_textureUnits.begin();
		TextureUnitPtr l_texUnit = ( * (l_it + p_index));
		m_textureUnits.erase( l_it + p_index);
//		delete l_texUnit;
		l_texUnit.reset();
		unsigned int i = static_cast <unsigned int>( p_index);

		for (l_it = m_textureUnits.begin() + p_index ; l_it != m_textureUnits.end() ; ++l_it)
		{
			l_texUnit = (* l_it);
			l_texUnit->SetIndex( i++);
		}

		l_bReturn = true;
	}

	return l_bReturn;
}

TextureUnitPtr Pass :: GetTextureUnit( unsigned int p_index)
{
	TextureUnitPtr l_pReturn;

	if (p_index < m_textureUnits.size())
	{
		l_pReturn = m_textureUnits[p_index];
	}

	return l_pReturn;
}

String Pass :: GetTexturePath( unsigned int p_index)
{
	String l_res = C3DEmptyString;

	if (p_index < m_textureUnits.size())
	{
		TextureUnitPtr l_texUnit = m_textureUnits[p_index];
		l_res = l_texUnit->GetTexturePath();
	}

	return l_res;
}

bool Pass :: Write( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = p_pFile.WriteLine( "\tpass\n\t{\n");

	if (l_bReturn)
	{
		p_pFile.Print( 256, "\t\tambient %f %f %f %f\n", m_ambient[0], m_ambient[1], m_ambient[2], m_ambient[3]);
	}
	
	if (l_bReturn)
	{
		p_pFile.Print( 256, "\t\tdiffuse %f %f %f %f\n", m_diffuse[0], m_diffuse[1], m_diffuse[2], m_diffuse[3]);
	}
	
	if (l_bReturn)
	{
		p_pFile.Print( 256, "\t\temissive %f %f %f %f\n", m_emissive[0], m_emissive[1], m_emissive[2], m_emissive[3]);
	}
	
	if (l_bReturn)
	{
		p_pFile.Print( 256, "\t\tspecular %f %f %f %f\n", m_specular[0], m_specular[1], m_specular[2], m_specular[3]);
	}
	
	if (l_bReturn)
	{
		p_pFile.Print( 256, "\t\tshininess %f\n", m_shininess);
	}
	
	if (l_bReturn)
	{
		p_pFile.Print( 256, "\t\ttex_base %f %f %f %f\n", m_texBaseColour[0], m_texBaseColour[1], m_texBaseColour[2], m_texBaseColour[3]);
	}
	
	if (l_bReturn)
	{
		p_pFile.WriteLine( "\t\tdouble_face " + String( m_doubleFace ? "true" : "false") + "\n");
	}

	if (l_bReturn)
	{
		size_t l_nbTextureUnits = m_textureUnits.size();
		bool l_bFirst = true;

		for (size_t i = 0 ; i < l_nbTextureUnits && l_bReturn ; i++)
		{
			if (l_bFirst)
			{
				l_bFirst = false;
			}
			else
			{
				p_pFile.WriteLine( "\n");
			}

			l_bReturn = m_textureUnits[i]->Write( p_pFile);
		}
	}
	
	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "\t}\n");
	}

	return l_bReturn;
}

bool Pass :: Read( Castor::Utils::File & p_file)
{
	bool l_bReturn = m_ambient.Read( p_file);

	if (l_bReturn)
	{
		l_bReturn = m_diffuse.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_specular.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_emissive.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_shininess) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = m_texBaseColour.Read( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read( m_doubleFace) == sizeof( bool));
	}

	if (l_bReturn)
	{
		size_t l_nbTextureUnits = 0;
		l_bReturn = (p_file.Read<size_t>( l_nbTextureUnits) == sizeof( size_t));

		TextureUnitPtr l_textureUnit;

		for (size_t i = 0 ; i < l_nbTextureUnits && l_bReturn ; i++)
		{
			l_textureUnit = new TextureUnit();

			l_bReturn = l_textureUnit->Read( p_file);

			if (l_bReturn)
			{
				AddTextureUnit( l_textureUnit);
			}
		}
	}

	return l_bReturn;
}

void Pass :: SetShader( ShaderProgramPtr p_program)
{
	m_shaderProgram = p_program;
}

ShaderProgramPtr Pass :: GetShader()const
{
	return m_shaderProgram.lock();
}

bool Pass :: HasShader()const
{
	return ! m_shaderProgram.expired();
}
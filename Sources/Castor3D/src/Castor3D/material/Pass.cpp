#include "PrecompiledHeader.h"

#include "material/Module_Material.h"

#include "material/Pass.h"
#include "material/TextureUnit.h"
#include "material/TextureEnvironment.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/MaterialRenderer.h"
#include "shader/ShaderProgram.h"

#include "Log.h"

using namespace Castor3D;

Pass :: Pass( PassRenderer * p_renderer, Material * p_parent)
	:	m_shininess( 50.0),
		m_doubleFace( false),
		m_parent( p_parent),
		m_renderer( p_renderer),
		m_shaderProgram( NULL)
{
	m_renderer->SetTarget( this);
	m_textureUnits.clear();
	SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f);
	SetEmissive( 0.0f, 0.0f, 0.0f, 1.0f);
	SetAmbient( 0.0f, 0.0f, 0.0f, 1.0f);
	SetSpecular( 1.0f, 1.0f, 1.0f, 1.0f);
}

Pass :: ~Pass()
{
	vector::deleteAll( m_textureUnits);
}

void Pass :: Initialise()
{
	vector::cycle( m_textureUnits, & TextureUnit::Initialise);
	m_renderer->Initialise();
}

void Pass :: Apply( Submesh * p_submesh, DrawType p_displayMode)
{
	if (m_shaderProgram != NULL)
	{
		m_shaderProgram->Initialise();
		m_shaderProgram->Begin();
	}

	m_renderer->Apply( p_submesh, p_displayMode);
	vector::cycle( m_textureUnits, & TextureUnit::Apply);
}

void Pass :: Apply( DrawType p_displayMode)
{
	if (m_shaderProgram != NULL)
	{
		m_shaderProgram->Initialise();
		m_shaderProgram->Begin();
	}

	m_renderer->Apply( p_displayMode);
	vector::cycle( m_textureUnits, & TextureUnit::Apply);
}

void Pass :: Remove()
{
	vector::cycle( m_textureUnits, & TextureUnit::Remove);
	m_renderer->Remove();

	if (m_shaderProgram != NULL)
	{
		m_shaderProgram->End();
	}
}

void Pass :: SetTexBaseColour( float p_r, float p_g, float p_b, float p_a)
{
	m_texBaseColour = Colour( p_r, p_g, p_b, p_a);

	vector::cycle( m_textureUnits, & TextureUnit::SetPrimaryColour, m_texBaseColour);
}

void Pass :: AddTextureUnit( TextureUnit * p_texUnit)
{
	if (p_texUnit != NULL)
	{
		m_textureUnits.push_back( p_texUnit);
		p_texUnit->SetIndex( static_cast <unsigned int>( m_textureUnits.size() - 1));
		p_texUnit->SetPrimaryColour( m_texBaseColour.ptr());
	}
}

bool Pass :: DestroyTextureUnit( size_t p_index)
{
	bool l_bReturn = false;
	Log::LogMessage( C3D_T( "Destroying TextureUnit %d"), p_index);

	if (p_index < m_textureUnits.size())
	{
		TextureUnitPtrArray::iterator l_it = m_textureUnits.begin();
		TextureUnit * l_texUnit = ( * (l_it + p_index));
		m_textureUnits.erase( l_it + p_index);
		delete l_texUnit;
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

TextureUnit * Pass :: GetTextureUnit( unsigned int p_index)
{
	if (p_index < m_textureUnits.size())
	{
		return m_textureUnits[p_index];
	}

	return NULL;
}

String Pass :: GetTexturePath( unsigned int p_index)
{
	String l_res = C3DEmptyString;

	if (p_index < m_textureUnits.size())
	{
		TextureUnit * l_texUnit = m_textureUnits[p_index];
		l_res = l_texUnit->GetTexturePath();
	}

	return l_res;
}

bool Pass :: Write( General::Utils::FileIO * p_pFile)const
{
	bool l_bReturn = p_pFile->WriteLine( "\tpass\n\t{\n");

	if (l_bReturn)
	{
		p_pFile->Print( 256, "\t\tambient %f %f %f %f\n", m_ambient.r, m_ambient.g, m_ambient.b, m_ambient.a);
	}
	
	if (l_bReturn)
	{
		p_pFile->Print( 256, "\t\tdiffuse %f %f %f %f\n", m_diffuse.r, m_diffuse.g, m_diffuse.b, m_diffuse.a);
	}
	
	if (l_bReturn)
	{
		p_pFile->Print( 256, "\t\temissive %f %f %f %f\n", m_emissive.r, m_emissive.g, m_emissive.b, m_emissive.a);
	}
	
	if (l_bReturn)
	{
		p_pFile->Print( 256, "\t\tspecular %f %f %f %f\n", m_specular.r, m_specular.g, m_specular.b, m_specular.a);
	}
	
	if (l_bReturn)
	{
		p_pFile->Print( 256, "\t\tshininess %f\n", m_shininess);
	}
	
	if (l_bReturn)
	{
		p_pFile->Print( 256, "\t\ttex_base %f %f %f %f\n", m_texBaseColour.r, m_texBaseColour.g, m_texBaseColour.b, m_texBaseColour.a);
	}
	
	if (l_bReturn)
	{
		p_pFile->WriteLine( "\t\tdouble_face " + String( m_doubleFace ? "true" : "false") + "\n");
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
				p_pFile->WriteLine( "\n");
			}

			l_bReturn = m_textureUnits[i]->Write( p_pFile);
		}
	}
	
	if (l_bReturn)
	{
		l_bReturn = p_pFile->WriteLine( "\t}\n");
	}

	return l_bReturn;
}

bool Pass :: Read( General::Utils::FileIO & p_file)
{
	bool l_bReturn = (p_file.ReadArray<float>( m_ambient.ptr(), 3) == sizeof( float) * 3);

	if (l_bReturn)
	{
		l_bReturn = (l_bReturn = p_file.ReadArray<float>( m_diffuse.ptr(), 3) == sizeof( float) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (l_bReturn = p_file.ReadArray<float>( m_specular.ptr(), 3) == sizeof( float) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray<float>( m_emissive.ptr(), 3) == sizeof( float) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read<float>( m_shininess) == sizeof( float));
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.ReadArray<float>( m_texBaseColour.ptr(), 3) == sizeof( float) * 3);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Read<bool>( m_doubleFace) == sizeof( bool));
	}

	if (l_bReturn)
	{
		size_t l_nbTextureUnits = 0;
		l_bReturn = (p_file.Read<size_t>( l_nbTextureUnits) == sizeof( size_t));

		TextureUnit * l_textureUnit;

		for (size_t i = 0 ; i < l_nbTextureUnits && l_bReturn ; i++)
		{
			l_textureUnit = new TextureUnit( Root::GetRenderSystem()->CreateTextureRenderer());

			l_bReturn = l_textureUnit->Read( p_file);

			if (l_bReturn)
			{
				this->AddTextureUnit( l_textureUnit);
			}
		}
	}

	return l_bReturn;
}
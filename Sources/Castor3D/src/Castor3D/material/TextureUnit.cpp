#include "PrecompiledHeader.h"

#include "material/Module_Material.h"

#include "material/TextureUnit.h"
#include "material/TextureEnvironment.h"
#include "render_system/RenderSystem.h"
#include "render_system/MaterialRenderer.h"
#include "main/Root.h"

#include "Log.h"

using namespace Castor3D;

TextureUnit :: TextureUnit( TextureRenderer * p_renderer)
	:	m_index( 0),
		m_textureInitialised( false),
		m_textured( false),
		m_renderer( p_renderer),
		m_image( NULL),
		m_mode( eNone),
		m_primaryColour( 1, 1, 1, 1)
{
	m_renderer->SetTarget( this);
	m_environment = new TextureEnvironment( Root::GetRenderSystem()->CreateTextureEnvironmentRenderer());
}

TextureUnit :: ~TextureUnit()
{
	delete m_environment;
}

void TextureUnit :: SetTexture2D( const String & p_name)
{
	if ( ! p_name.empty())
	{
		if (m_image != NULL)
		{
			m_image->Release();
		}

		m_image = ImageManager::GetSingletonPtr()->CreateImage( p_name, p_name);
		m_textureInitialised = false;
		m_textured = true;
		m_textureType = eTex2D;
	}
}

void TextureUnit :: Initialise()
{
	if (m_textured)
	{
		if (m_textureType == eTex2D &&  ! m_textureInitialised)
		{
			m_renderer->Initialise();
		}

		m_textureInitialised = true;
	}
}

void TextureUnit :: Apply()
{
	if (m_textured && m_textureInitialised)
	{
		m_renderer->Apply();
	}
}

void TextureUnit :: Remove()
{
	if (m_textured && m_textureInitialised)
	{
		m_renderer->Remove();
	}
}

bool TextureUnit :: Write( General::Utils::FileIO * p_pFile)const
{
	bool l_bReturn = true;

	if (m_textured)
	{
		if (l_bReturn)
		{
			l_bReturn = p_pFile->WriteLine( "\t\ttexture_unit\n\t\t{\n");
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile->Print( 256, "\t\t\tcolour %f %f %f %f\n", m_primaryColour.r, m_primaryColour.g, m_primaryColour.b, m_primaryColour.a);
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile->Print( 256, "\t\t\tmap_type %i\n", m_mode);
		}

		if (l_bReturn && m_image != NULL)
		{
			String l_strPath = m_image->GetPath();
			l_strPath.Replace( "\\", "/");
			l_bReturn = p_pFile->WriteLine( "\t\t\timage " + l_strPath + "\n");
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile->WriteLine( "\t\t}\n");
		}
	}

	return l_bReturn;
}

bool TextureUnit :: Read( General::Utils::FileIO & p_file)
{
	bool l_bReturn = (p_file.Read<bool>( m_textured) == sizeof( bool));

	if (l_bReturn && m_textured)
	{
		l_bReturn = (p_file.Read<eDIMENSION>( m_textureType) == sizeof( eDIMENSION));

		if (l_bReturn)
		{
			l_bReturn = (p_file.ReadArray<float>( m_primaryColour.ptr(), 4) == sizeof( float) * 4);
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Read<eMAP_MODE>( m_mode) == sizeof( eMAP_MODE));
		}

		if (l_bReturn)
		{
			l_bReturn = m_environment->Read( p_file);
		}

		if (l_bReturn)
		{
			size_t l_nameLength = 0;
			l_bReturn = (p_file.Read<size_t>( l_nameLength) == sizeof( size_t));

			if (l_bReturn)
			{
				Char * l_name = new Char[l_nameLength+1];
				l_bReturn = (p_file.ReadArray<Char>( l_name, l_nameLength) == l_nameLength);

				if (l_bReturn)
				{
					l_name[l_nameLength] = 0;
					SetTexture2D( l_name);
				}

				delete [] l_name;
			}
		}
	}

	return l_bReturn;
}

const unsigned char * TextureUnit :: GetImagePixels()const
{
	if (m_textured && m_image != NULL)
	{
		return m_image->GetBuffer();
	}

	return NULL;
}

unsigned int TextureUnit :: GetWidth()const
{
	if (m_textured && m_image != NULL)
	{
		return m_image->GetWidth();
	}

	return 0;
}

unsigned int TextureUnit :: GetHeight()const
{
	if (m_textured && m_image != NULL)
	{
		return m_image->GetHeight();
	}

	return 0;
}

String TextureUnit :: GetTexturePath()const
{
	if (m_textured && m_image != NULL)
	{
		return m_image->GetPath();
	}

	return C3DEmptyString;
}
#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/material/TextureEnvironment.h"
#include "Castor3D/main/Root.h"

using namespace Castor3D;

TextureUnit :: TextureUnit()
	:	m_uiIndex				( 0)
	,	m_bTextureInitialised	( false)
	,	m_bTextured				( false)
	,	m_eMode					( eNone)
	,	m_clrPrimaryColour		( 1, 1, 1, 1)
	,	m_eTextureType			( eTex2D)
	,	m_environment			( new TextureEnvironment)
{
}

TextureUnit :: TextureUnit( const TextureUnit & p_copy)
	:	m_uiIndex				( p_copy.m_uiIndex)
	,	m_pImage				( p_copy.m_pImage)
	,	m_environment			( p_copy.m_environment)
	,	m_eTextureType			( p_copy.m_eTextureType)
	,	m_bTextureInitialised	( p_copy.m_bTextureInitialised)
	,	m_bTextured				( p_copy.m_bTextured)
	,	m_clrPrimaryColour		( p_copy.m_clrPrimaryColour)
	,	m_eMode					( p_copy.m_eMode)
{
}

TextureUnit & TextureUnit :: operator =( const TextureUnit & p_copy)
{
	m_uiIndex				= p_copy.m_uiIndex;
	m_pImage				= p_copy.m_pImage;
	m_environment			= p_copy.m_environment;
	m_eTextureType			= p_copy.m_eTextureType;
	m_bTextureInitialised	= p_copy.m_bTextureInitialised;
	m_bTextured				= p_copy.m_bTextured;
	m_clrPrimaryColour		= p_copy.m_clrPrimaryColour;
	m_eMode					= p_copy.m_eMode;

	return * this;
}

TextureUnit :: ~TextureUnit()
{
}

void TextureUnit :: SetTexture2D( ImagePtr p_pImage)
{
	if (p_pImage != NULL)
	{
		if ( ! m_pImage == NULL)
		{
			m_pImage->Release();
		}

		m_pImage = p_pImage;
		m_bTextureInitialised = false;
		m_bTextured = true;
		m_eTextureType = eTex2D;
	}
}

void TextureUnit :: Initialise()
{
	if (m_bTextured)
	{
		if (m_eTextureType == eTex2D &&  ! m_bTextureInitialised)
		{
			m_pRenderer->Initialise();
		}

		m_bTextureInitialised = true;
	}
}

void TextureUnit :: Render( eDRAW_TYPE p_displayMode)
{
	if (m_bTextured && m_bTextureInitialised)
	{
		m_pRenderer->Render();
	}
}

void TextureUnit :: EndRender()
{
	if (m_bTextured && m_bTextureInitialised)
	{
		m_pRenderer->EndRender();
	}
}

bool TextureUnit :: Write( Castor::Utils::File & p_pFile)const
{
	bool l_bReturn = true;

	if (m_bTextured)
	{
		if (l_bReturn)
		{
			l_bReturn = p_pFile.WriteLine( "\t\ttexture_unit\n\t\t{\n");
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile.Print( 256, "\t\t\tcolour %f %f %f %f\n", m_clrPrimaryColour[0], m_clrPrimaryColour[1], m_clrPrimaryColour[2], m_clrPrimaryColour[3]);
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile.Print( 256, "\t\t\tmap_type %i\n", m_eMode);
		}

		if (l_bReturn && ! m_pImage == NULL)
		{
			String l_strPath = m_pImage->GetPath();
			l_strPath.Replace( "\\", "/");
			l_bReturn = p_pFile.WriteLine( "\t\t\timage " + l_strPath + "\n");
		}

		if (l_bReturn)
		{
			l_bReturn = p_pFile.WriteLine( "\t\t}\n");
		}
	}

	return l_bReturn;
}

bool TextureUnit :: Read( Castor::Utils::File & p_file, ImageManager * p_pManager)
{
	bool l_bReturn = (p_file.Read( m_bTextured) == sizeof( bool));

	if (l_bReturn && m_bTextured)
	{
		l_bReturn = (p_file.Read( m_eTextureType) == sizeof( eDIMENSION));

		if (l_bReturn)
		{
			l_bReturn = m_clrPrimaryColour.Read( p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = (p_file.Read( m_eMode) == sizeof( eMAP_MODE));
		}

		if (l_bReturn)
		{
			l_bReturn = m_environment->Read( p_file);
		}

		if (l_bReturn)
		{
			size_t l_nameLength = 0;
			l_bReturn = (p_file.Read( l_nameLength) == sizeof( size_t));

			if (l_bReturn)
			{
				Char * l_name = new Char[l_nameLength+1];
				l_bReturn = (p_file.ReadArray( l_name, l_nameLength) == l_nameLength);

				if (l_bReturn)
				{
					l_name[l_nameLength] = 0;
					SetTexture2D( p_pManager->CreateImage( l_name, l_name));
				}

				delete [] l_name;
			}
		}
	}

	return l_bReturn;
}

const unsigned char * TextureUnit :: GetImagePixels()const
{
	if (m_bTextured && ! m_pImage == NULL)
	{
		return m_pImage->GetBuffer();
	}

	return NULL;
}

unsigned char * TextureUnit :: GetImagePixels()
{
	if (m_bTextured && ! m_pImage == NULL)
	{
		return m_pImage->GetBuffer();
	}

	return NULL;
}

unsigned int TextureUnit :: GetWidth()const
{
	if (m_bTextured && ! m_pImage == NULL)
	{
		return m_pImage->GetWidth();
	}

	return 0;
}

unsigned int TextureUnit :: GetHeight()const
{
	if (m_bTextured && ! m_pImage == NULL)
	{
		return m_pImage->GetHeight();
	}

	return 0;
}

String TextureUnit :: GetTexturePath()const
{
	if (m_bTextured && ! m_pImage == NULL)
	{
		return m_pImage->GetPath();
	}

	return C3DEmptyString;
}
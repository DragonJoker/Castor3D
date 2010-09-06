#include "Image.h"

#include <FreeImagePlus.h>

using namespace General::Resource;

//*********************************************************************************************

Image * ImageLoader :: LoadFromFileIO( const String & p_file)
{
	String l_strPath = p_file;
	l_strPath.Replace( "\\", "/");
	fipImage * l_image = new fipImage();

	if ( ! l_image->load( l_strPath.char_str()))
	{
		delete l_image;
		return NULL;
	}

	l_image->convertTo32Bits();
	unsigned int l_width = l_image->getWidth();
	unsigned int l_height = l_image->getHeight();

	Buffer <unsigned char> * l_buffer = new Buffer <unsigned char>();
	l_buffer->InitialiseBuffer( l_width * l_height * 4, 0);
	unsigned char * l_pixels = l_image->accessPixels();

	for (unsigned int l_pix = 0 ; l_pix < l_width * l_height ; l_pix++)
	{
		l_buffer->Add( l_pixels[l_pix * 4 + 2]);
		l_buffer->Add( l_pixels[l_pix * 4 + 1]);
		l_buffer->Add( l_pixels[l_pix * 4 + 0]);
		l_buffer->Add( l_pixels[l_pix * 4 + 3]);
	}

	delete l_image;

	Image * l_newImage = new Image( p_file, l_buffer);
	l_newImage->SetWidth( l_width);
	l_newImage->SetHeight( l_height);

	return l_newImage;
}

//*********************************************************************************************

Image :: Image( const String & p_path, Buffer <unsigned char> * p_buffer)
	:	Resource(),
		m_path( p_path),
		m_buffer( p_buffer)
{
}

Image :: ~Image()
{
	delete m_buffer;
}

void Image :: Release()
{
	if (*m_references == 1)
	{
		ImageManager::GetSingletonPtr()->RemoveElement( this);
	}
	Resource::Release();
}

//*********************************************************************************************

Image * ImageManager :: CreateImage( const String & p_name, const String & p_path)
{
	Image * l_image = GetElementByName( p_name);
	if (l_image != NULL)
	{
		l_image->Ref();
	}
	else
	{
		ImageLoader l_loader;
		l_image = l_loader.LoadFromFileIO( p_path);
		if (l_image != NULL)
		{
			l_image->SetName( p_name);
			AddElement( l_image);
		}
	}
	return l_image;
}

//*********************************************************************************************
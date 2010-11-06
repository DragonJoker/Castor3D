#include "PrecompiledHeader.h"

#include "Image.h"
#include "ScopedPtr.h"

#include <FreeImagePlus.h>

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
	using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::Resource;

//*********************************************************************************************

ImagePtr ImageLoader :: LoadFromFile( const String & p_file)
{
	ImagePtr l_pReturn;
	String l_strPath = p_file;
	l_strPath.Replace( "\\", "/");
	ScopedPtr <fipImage> l_image( new fipImage());

	if (l_image->load( l_strPath.char_str()))
	{
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

		l_pReturn.reset( new Image( p_file, l_buffer));
		l_pReturn->SetWidth( l_width);
		l_pReturn->SetHeight( l_height);
	}

	return l_pReturn;
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
		ImageManager::GetSingletonPtr()->RemoveElement( ImagePtr( this));
	}
	Resource::Release();
}

//*********************************************************************************************

ImagePtr ImageManager :: CreateImage( const String & p_name, const String & p_path)
{
	ImagePtr l_image = GetElementByName( p_name);

	if ( ! l_image.null())
	{
		l_image->Ref();
	}
	else
	{
		ImageLoader l_loader;
		l_image = l_loader.LoadFromFile( p_path);

		if ( ! l_image.null())
		{
			l_image->SetName( p_name);
			AddElement( l_image);
		}
	}

	return l_image;
}

//*********************************************************************************************